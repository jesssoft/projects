#
# jesssoft
#

#=======================================================================
# create procedures.
#=======================================================================

######################
use pq_account_db;
######################

#-----------------------------------------------------------------------
# create pq_get_account(acc_name VARCHAR(12));
#-----------------------------------------------------------------------

DELIMITER $$
DROP PROCEDURE IF EXISTS pq_get_account$$

CREATE DEFINER=`pq_user`@`%` PROCEDURE pq_get_account(
		IN _acc_name VARCHAR(12))
BEGIN
	DECLARE var_acc_id INT UNSIGNED;
	DECLARE var_reg_time DATETIME;
	DECLARE var_left_chat_block_sec DATETIME;

	SELECT acc_id INTO var_acc_id FROM pq_account
		WHERE acc_name = _acc_name;

	SELECT reg_time INTO var_reg_time FROM pq_account_chat_block_list
		WHERE acc_id = var_acc_id;

	IF var_reg_time IS NOT NULL
	THEN
		SET var_left_chat_block_sec 
			= TIME_TO_SEC(TIMEDIFF(NOW(), var_reg_time));

		IF  var_left_chat_block_sec > (5*60) OR
			var_left_chat_block_sec < 0
		THEN
			DELETE FROM pq_account_chat_block_list
				WHERE acc_id = var_acc_id;
		END IF;
	END IF;

		
	IF EXISTS(SELECT * FROM pq_account_chat_block_list
			WHERE acc_id = var_acc_id)
	THEN
		#
		# chat blocked
		#
		SELECT a.acc_id
			, a.cash
			, a.private_cash
			, TIME_TO_SEC(TIMEDIFF(NOW(), b.reg_time))
				AS left_chat_block_sec 
			FROM pq_account a LEFT OUTER JOIN
				pq_account_chat_block_list b
				ON (a.acc_id = b.acc_id)
			WHERE acc_name = _acc_name;
	ELSE
		#
		# chat not blocked
		#
		SELECT acc_id
			, cash
			, private_cash
			, 0 AS left_chat_block_sec 
			FROM pq_account 
			WHERE acc_name = _acc_name;
	END IF;

END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_get_cash(acc_id);
#-----------------------------------------------------------------------

DELIMITER $$
DROP PROCEDURE IF EXISTS pq_get_cash$$

CREATE DEFINER=`pq_user`@`%` PROCEDURE pq_get_cash(
		IN _acc_id INT UNSIGNED)
BEGIN
	SELECT cash, private_cash
		FROM pq_account
		WHERE acc_id = _acc_id;
END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_get_bad_word();
#-----------------------------------------------------------------------

DELIMITER $$
DROP PROCEDURE IF EXISTS pq_get_bad_word$$

CREATE DEFINER=`pq_user`@`%` PROCEDURE pq_get_bad_word()
BEGIN
	SELECT bad_word FROM pq_bad_word;
END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_get_reserved_name();
#-----------------------------------------------------------------------

DELIMITER $$
DROP PROCEDURE IF EXISTS pq_get_reserved_name$$

CREATE DEFINER=`pq_user`@`%` PROCEDURE pq_get_reserved_name()
BEGIN
	SELECT reserved_name FROM pq_reserved_name;
END$$
DELIMITER ;

