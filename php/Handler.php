<?php
//
// jesssoft
//
require_once 'Base.php';
require_once 'Timestamp.php';
require_once 'Transaction.php';
require_once 'TransactionStats.php';
require_once 'ScorePost.php';
require_once 'LeaderboardGet.php';
require_once 'UserSave.php';
require_once 'UserLoad.php';

class Handler {
private $cmd;
private $method;
private $data;

public function __construct($cmd, $method) {
	$this->cmd = $cmd;
	$this->method = $method;

	if ($method === "POST")
		$this->data = file_get_contents("php://input");
	else
		$this->data = "";
}

public function run() {
	header("Content-Type: application/json");
	switch ($this->method) {
	case "GET":
		$this->run_get();
		break;
	case "POST":
		$this->run_post();
		break;
	default:
		Util::printNoSupportedMethodErrMsg();
	}
}

private function run_get() {
	switch ($this->cmd) {
	case "Timestamp":
		$obj = new Timestamp();
		$obj->run();
		break;
	default:
		Util::printNoSupportedEndpointErrMsg();
	}
}

private function run_post() {
	$msg = json_decode($this->data, TRUE);
	if ($msg == NULL) {
		Util::printNoPostErrMsg();
		return;
	}

	switch ($this->cmd) {
	case "Transaction":
		$obj = new Transaction();
		$obj->run($msg);
		break;
	case "TransactionStats":
		$obj = new TransactionStats();
		$obj->run($msg);
		break;
	case "ScorePost":
		$obj = new ScorePost();
		$obj->run($msg);
		break;
	case "LeaderboardGet":
		$obj = new LeaderboardGet();
		$obj->run($msg);
		break;
	case "UserSave":
		$obj = new UserSave();
		$obj->run($msg);
		break;
	case "UserLoad":
		$obj = new UserLoad();
		$obj->run($msg);
		break;
	default:
		Util::printNoSupportedEndpointErrMsg();
	}
}

}

?>
