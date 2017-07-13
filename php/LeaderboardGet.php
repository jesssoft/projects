<?php
//
// jesssoft
//
require_once 'Base.php';

class LeaderboardGet extends Base {
public function run($msg) {
	//
	// Input validation
	//
	if (!array_key_exists('UserId', $msg) ||
	    !array_key_exists('LeaderboardId', $msg) ||
	    !array_key_exists('Offset', $msg) ||
	    !array_key_exists('Limit', $msg)) {
		Util::printInvalidInputErrMsg();
		return;
	}

	//
	// Get data
	//
	$userId = $msg['UserId'];
	$leaderboardId = $msg['LeaderboardId'];
	$offset = $msg['Offset'];
	$limit = $msg['Limit'];

	//
	// Try to connect to db
	//
	if (!$this->initDb()) {
		Util::printDbConnectionErrMsg();
		return;
	}

	//
	// Select data from db
	//
	$this->printData($userId, $leaderboardId, $offset, $limit);
}

private function printData($userId, $leaderboardId, $offset, $limit) {
	//
	// Get the best score 
	//
	$sql = "select * from leaderboard where ".
	    "user_id = ".$userId." and ".
	    "leaderboard_id = ".$leaderboardId.
	    " order by score desc limit 1";
	$row = $this->query_row($sql);
	if (!$row) {
		Util::printErrMsg("The user does not exist");
		return false;
	}

	$bestScore = (int)$row['score'];

	//
	// Calculate this user's rank 
	//
	$sql = "select * from (select lb.*, ".
	    "@cur_rank := if(@cur_score = score, @cur_rank, @cur_rank + 1) ".
	    "as rank, @cur_score := score as dummy from leaderboard lb, ".
	    "(select @cur_rank := 0, @cur_score := -1) tmp where ".
	    "leaderboard_id = ".$leaderboardId." order by score desc) ".
	    "result where user_id = ".$userId." and score = ".$bestScore;
	$row = $this->query_row($sql);
	$rank = (int)$row['rank'];

	//
	// Select other users' data
	//
	$entries = Array();
	$sql = "select * from (select lb.*, ".
	    "@cur_rank := if(@cur_score = score, @cur_rank, @cur_rank + 1) ".
	    "as rank, @cur_score := score as dummy from leaderboard lb, ".
	    "(select @cur_rank := 0, @cur_score := -1) tmp where ".
	    "leaderboard_id = ".$leaderboardId." order by score desc) ".
	    "result limit ".$offset.",".$limit;
	$result = $this->query($sql);
	while ($row = $this->fetch_row($result)) {
		$ranker = Array("UserId" => (int)$row['user_id'],
				"Score" => (int)$row['score'],
				"Rank" => (int)$row['rank']);
		$entries[] = $ranker;
	}
	$this->freeResult($result);

	//
	// Output result
	//
	$ary = Array("UserId" => $userId, "LeaderboardId" => $leaderboardId,
	    "Score" => $bestScore, "Rank" => $rank,
	    "Entries" => $entries);

	Util::printResult($ary);
}

}

?>

