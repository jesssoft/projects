<?php
//
// jesssoft
//
require_once 'Base.php';

class ScorePost extends Base {
public function run($msg) {
	//
	// Input validation
	//
	if (!array_key_exists('UserId', $msg) ||
	    !array_key_exists('LeaderboardId', $msg) ||
	    !array_key_exists('Score', $msg)) {
		Util::printInvalidInputErrMsg();
		return;
	}

	//
	// Get data
	//
	$userId = $msg['UserId'];
	$leaderboardId = $msg['LeaderboardId'];
	$score = $msg['Score'];

	//
	// Try to connect to db
	//
	if (!$this->initDb()) {
		Util::printDbConnectionErrMsg();
		return;
	}

	//
	// Hight score checking
	//
	if (!$this->isHigherScore($userId, $leaderboardId, $score)) {
		$this->printData($userId, $leaderboardId);
		return;
	}

	//
	// Insert data into the db
	//
	if (!$this->insertData($userId, $leaderboardId, $score)) {
		Util::printErrMsg("Inserting data failed - ".
		    $this->getDbErrMsg());
		return;
	}

	//
	// Print result 
	//
	$this->printData($userId, $leaderboardId);
}

private function isHigherScore($userId, $leaderboardId, $score) {
	$sql = "select count(*) from leaderboard where ".
	    "user_id = ".$userId." and ".
	    "leaderboard_id = ".$leaderboardId. " and ".
	    "score >= ".$score;
	
	$flag = true;
	$row = $this->query_row($sql);
	
	if ($row[0] > 0)
		$flag = false;

	return $flag;
}

private function insertData($userId, $leaderboardId, $score) {
	$sql = "insert into leaderboard ".
	    "(user_id, leaderboard_id, score) values ".
	    "(".$userId.", ".$leaderboardId.", ".$score.")";
	
	return $this->query($sql);
}

private function printData($userId, $leaderboardId) {
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
	// ## Version 1 ##
	// Calculate this user's rank which has a unique value. 
	//
	/*
	$sql = "select * from (select lb.*, @cur_rank := @cur_rank + 1 ".
	    "as rank from leaderboard lb, (select @cur_rank := 0) tmp where ".
	    "leaderboard_id = ".$leaderboardId." order by score desc) ".
	    "result where user_id = ".$userId." and score = ".$bestScore;
	$row = $this->query_row($sql);
	$rank = (int)$row['rank'];
	*/

	//
	// ## Version 2 ##
	// Calculate this user's rank which can be a same value. 
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
	// Output result
	//
	$ary = Array("UserId" => $userId, "LeaderboardId" => $leaderboardId,
	    "Score" => $bestScore, "Rank" => $rank);

	Util::printResult($ary);
	return true;
}

}

?>

