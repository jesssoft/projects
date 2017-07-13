<?php
//
// jesssoft
//
require_once 'Base.php';

class Transaction extends Base {
public $scretKey = "key-name";

public function run($msg) {
	//
	// Input validation
	//
	if (!array_key_exists('TransactionId', $msg) ||
	    !array_key_exists('UserId', $msg) ||
	    !array_key_exists('CurrencyAmount', $msg) ||
	    !array_key_exists('Verifier', $msg)) {
		Util::printInvalidInputErrMsg();
		return;
	}

	//
	// Get data
	//
	$transactionId = $msg['TransactionId'];
	$userId = $msg['UserId'];
	$currencyAmount = $msg['CurrencyAmount'];
	$verifier = $msg['Verifier'];

	//
	// Validation
	//
	$hash = sha1($this->scretKey.$transactionId.$userId.$currencyAmount);
	if ($hash !== $verifier) {
		Util::printErrMsg("Invalid data - hash mismatched");
		return;
	}

	//
	// Try to connect to db
	//
	if (!$this->initDb()) {
		Util::printDbConnectionErrMsg();
		return;
	}

	//
	// Duplication checking
	//
	if ($this->isDuplicated($transactionId, $userId)) {
		Util::printErrMsg("Duplication");
		return;
	}

	//
	// Insert data into the db
	//
	if (!$this->insertData($transactionId, $userId, $currencyAmount)) {
		Util::printErrMsg("Inserting data failed - ".
		    $this->getDbErrMsg());
		return;
	}

	//
	// Success message
	//
	Util::printSuccess();
}

private function isDuplicated($transactionId, $userId) {
	$sql = "select count(*) from transaction where ".
	    "transaction_id = ".$transactionId." and ".
	    "user_id = ".$userId;
	
	$flag = false;
	$row = $this->query_row($sql);
	
	if ($row[0] > 0)
		$flag = true;

	return $flag;
}

private function insertData($transactionId, $userId, $currencyAmount) {
	$sql = "insert into transaction ".
	    "(transaction_id, user_id, currency_amount) values ".
	    "(".$transactionId.", ".$userId.", ".$currencyAmount.")";
	
	return $this->query($sql);
}

}

?>

