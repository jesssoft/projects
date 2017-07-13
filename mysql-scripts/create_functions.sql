#
# jesssoft
#

#=======================================================================
# create functions.
#=======================================================================

####################################
use pq_account_db;
####################################

#-----------------------------------------------------------------------
# create pq_init(svr_id int unsigned);
#-----------------------------------------------------------------------
DELIMITER $$
DROP FUNCTION IF EXISTS pq_init$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_init(
		_svr_id INT UNSIGNED)
RETURNS INT
BEGIN
	DELETE FROM pq_current_user WHERE svr_id = _svr_id;
	RETURN 0;
END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_login(svr_id int unsigned, acc_name VARCHAR(12)
#			, acc_passwd VARCHAR(12));
#-----------------------------------------------------------------------
DELIMITER $$
DROP FUNCTION IF EXISTS pq_login$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_login(
		_svr_id INT UNSIGNED
		, _acc_name VARCHAR(12)
		, _acc_passwd VARCHAR(12))
RETURNS INT
BEGIN
	# Existing user?
	DECLARE db_acc_id INT UNSIGNED;
	DECLARE db_acc_name VARCHAR(12);
	DECLARE db_acc_passwd VARCHAR(12);
	DECLARE db_now DATETIME;
	DECLARE db_cur_count INT UNSIGNED;
	DECLARE db_max_count INT UNSIGNED;
	DECLARE db_svr_block_state TINYINT UNSIGNED DEFAULT 0;

	SELECT NOW() INTO db_now;

	SELECT acc_id
		, acc_name
		, acc_passwd
		INTO db_acc_id
			, db_acc_name
			, db_acc_passwd
		FROM pq_account
		WHERE acc_name = _acc_name;

	IF ISNULL(db_acc_name)
	THEN	
		# No user.
		RETURN -1;
	END IF;

	IF STRCMP(db_acc_passwd, _acc_passwd) != 0
	THEN
		# Passwd mismatch.
		RETURN -2;
	END IF;

	# Already have logged in.
	IF EXISTS(SELECT acc_id FROM pq_current_user
			WHERE acc_id = db_acc_id)
	THEN
		# Duplication. 
		RETURN -3;
	END IF;

	# is in black list?
	IF EXISTS(SELECT acc_id FROM pq_account_black_list
			WHERE acc_id = db_acc_id)
	THEN
		# black list
		RETURN -4;
	END IF;

	# block exception?
	IF NOT EXISTS(SELECT acc_id FROM pq_mng_block_exception_account 
			WHERE acc_id = db_acc_id)
	THEN
		SELECT block_state INTO db_svr_block_state
			FROM pq_mng_svr
			WHERE svr_id = _svr_id;

		IF db_svr_block_state != 0
		THEN
			# This server is being blocked.
			RETURN -5;
		END IF;
	END IF;

	INSERT INTO pq_current_user (acc_id, svr_id, login_time)
		VALUES (db_acc_id, _svr_id, db_now);

	############################################################
	# data for management
	############################################################

	SELECT COUNT(*)INTO db_cur_count FROM pq_current_user
		WHERE svr_id = _svr_id;

	#
	# Average accounts of day
	#
	IF EXISTS(SELECT * FROM pq_mng_ave_acc
			WHERE svr_id = _svr_id
				AND DATE(day_time) = DATE(db_now))
	THEN
		UPDATE pq_mng_ave_acc SET sum_acc = sum_acc + db_cur_count,
					update_count = update_count + 1
			WHERE svr_id = _svr_id
				AND DATE(day_time) = DATE(db_now);
	ELSE
		INSERT INTO pq_mng_ave_acc (svr_id, day_time, sum_acc
						, update_count)
			VALUES (_svr_id, db_now, db_cur_count, 1);
	END IF;

	#
	# Max account of hour
	#
	SELECT max_acc INTO db_max_count FROM pq_mng_max_acc
		WHERE svr_id = _svr_id 
			AND DATE(hour_time) = DATE(db_now)
			AND HOUR(hour_time) = HOUR(db_now);

	IF ISNULL(db_max_count)
	THEN
		INSERT INTO pq_mng_max_acc (svr_id, hour_time, max_acc)
			VALUES (_svr_id, db_now, db_cur_count);
	ELSEIF db_cur_count > db_max_count
	THEN
		UPDATE pq_mng_max_acc SET max_acc = db_cur_count
			WHERE svr_id = _svr_id
				AND DATE(hour_time) = DATE(db_now)
				AND HOUR(hour_time) = HOUR(db_now);
	END IF;
	
	#
	# DAU
	#
	IF NOT EXISTS(SELECT * FROM pq_mng_dau
			WHERE DATE(login_time) = DATE(db_now)
				AND acc_id = db_acc_id)
	THEN
		INSERT INTO pq_mng_dau (login_time, acc_id)
			VALUES(db_now, db_acc_id);
	END IF;

	#
	# MAU
	#
	IF NOT EXISTS(SELECT * FROM pq_mng_mau
			WHERE YEAR(login_time) = YEAR(db_now)
				AND MONTH(login_time) = MONTH(db_now)
				AND acc_id = db_acc_id)
	THEN
		INSERT INTO pq_mng_mau (login_time, acc_id)
			VALUES(db_now, db_acc_id);
	END IF;

	# SUCCESS.
	RETURN 0;
END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_logout(acc_id int unsigned) 
#-----------------------------------------------------------------------

DELIMITER $$
DROP FUNCTION IF EXISTS pq_logout$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_logout(
		_acc_id INT UNSIGNED)
RETURNS INT
BEGIN

	IF NOT EXISTS(SELECT acc_id FROM pq_current_user
				WHERE acc_id = _acc_id)
	THEN	
		RETURN -1;
	END IF;

	DELETE FROM pq_current_user 
		WHERE acc_id = _acc_id;

	RETURN 0;

END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_save_cash() 
#-----------------------------------------------------------------------
DELIMITER $$
DROP FUNCTION IF EXISTS pq_save_cash$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_save_cash(
		_acc_id INT UNSIGNED
		, _cash INT UNSIGNED)
RETURNS INT
BEGIN

	UPDATE pq_account SET cash = _cash
		WHERE acc_id = _acc_id;

	RETURN 0;

END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_save_tutorial_log() 
#-----------------------------------------------------------------------
DELIMITER $$
DROP FUNCTION IF EXISTS pq_save_tutorial_log$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_save_tutorial_log(
		_ch_id INT UNSIGNED
		, _tutorial_log VARCHAR(128)
		, _tip_log VARCHAR(128))
RETURNS INT
BEGIN
	UPDATE pq_character SET tutorial_log = _tutorial_log
				, tip_log = _tip_log
		WHERE ch_id = _ch_id;

	RETURN 0;

END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_set_option(acc_id, option_no, option_val);
#-----------------------------------------------------------------------

DELIMITER $$
DROP FUNCTION IF EXISTS pq_set_option$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_set_option(
		_acc_id INT UNSIGNED
		, _option_no TINYINT UNSIGNED
		, _option_val VARCHAR(20))
RETURNS INT
BEGIN
	IF EXISTS(SELECT * FROM pq_option WHERE acc_id = _acc_id
					AND option_no = _option_no)
	THEN
		UPDATE pq_option SET option_no = _option_no
				, option_val = _option_val
				WHERE acc_id = _acc_id
					AND option_no = _option_no;
	ELSE
		INSERT INTO pq_option (acc_id, option_no, option_val)
			VALUES(_acc_id, _option_no, _option_val);
	END IF;

	RETURN 0;	
END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_create_char;
#-----------------------------------------------------------------------
DELIMITER $$
DROP FUNCTION IF EXISTS pq_create_char$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_create_char(
		_max_ch_count TINYINT UNSIGNED
		, _acc_id INT UNSIGNED
		, _svr_id INT UNSIGNED
		, _ch_name VARCHAR(12)
		, _job_class TINYINT UNSIGNED
		, _gender TINYINT UNSIGNED
		, _portrait_icon TINYINT UNSIGNED
		, _earth_mastery SMALLINT UNSIGNED
		, _fire_mastery SMALLINT UNSIGNED
		, _air_mastery SMALLINT UNSIGNED
		, _water_mastery SMALLINT UNSIGNED
		, _battle_mastery SMALLINT UNSIGNED
		, _cunning_mastery SMALLINT UNSIGNED
		, _morale_mastery SMALLINT UNSIGNED)
RETURNS INT
BEGIN
	DECLARE ch_count INT UNSIGNED;
	DECLARE ch_id_high_no INT UNSIGNED;
	DECLARE ch_id_created INT UNSIGNED;
	DECLARE mycastle_ret INT UNSIGNED;

	SELECT COUNT(*) INTO ch_count FROM pq_character
		WHERE acc_id = _acc_id 
			AND svr_id = _svr_id
			AND del_flag = 0;

	# error check: character count limit?
	IF ch_count >= _max_ch_count
	THEN
		# FULL
		RETURN -1;
	END IF;

	# error check: check the character name if exists.
	IF EXISTS(SELECT * FROM pq_character WHERE ch_name = _ch_name)
	THEN
		# DUPLICATION
		RETURN -2;
	END IF;

	# error check: check ch_id maximum number.
	SELECT ch_id INTO ch_id_high_no FROM pq_character
		ORDER BY ch_id desc LIMIT 1;
	IF ch_id_high_no IS NULL
	THEN
		SET ch_id_high_no = 0;
	END IF;

	IF ch_id_high_no >= 2100000000
	THEN
		# FULL of CHID
		RETURN -3;
	END IF;

	# insert record.
	INSERT INTO pq_character (
			acc_id
			, ch_name
			, guild_id
			, job_class
			, gender
			, portrait_icon
			, svr_id
			, creat_time
			, ch_desc
			, earth_mastery
			, fire_mastery
			, air_mastery
			, water_mastery
			, battle_mastery
			, cunning_mastery
			, morale_mastery
			, last_login_time
			, last_logout_time
			, del_time
			)
		VALUES (
			_acc_id
			, _ch_name
			, 0 		# guild_id
			, _job_class
			, _gender
			, _portrait_icon
			, _svr_id
			, NOW()
			, ''
			, _earth_mastery
			, _fire_mastery
			, _air_mastery
			, _water_mastery
			, _battle_mastery
			, _cunning_mastery
			, _morale_mastery
			, NOW()
			, NOW()
			, NOW()
		       );
	
	IF ROW_COUNT() > 0
	THEN
		SELECT LAST_INSERT_ID() INTO ch_id_created;

		#
		# Default runes
		#
		INSERT INTO pq_inventory_rune (
				ch_id
				, rune_id)
			VALUES ( ch_id_created
					, 101);
		INSERT INTO pq_inventory_rune (
				ch_id
				, rune_id)
			VALUES ( ch_id_created
					, 201);
		INSERT INTO pq_inventory_rune (
				ch_id
				, rune_id)
			VALUES ( ch_id_created
					, 301);

		#
		# Bartonia
		#
		SELECT pq_win_castle(ch_id_created, 1) INTO mycastle_ret;

		#
		# log
		#
		INSERT INTO pq_mng_creat_ch (
				svr_id
				, act_time
				, ch_id
				, job_class)
			VALUES (
					_svr_id
					, NOW()
					, ch_id_created
					, _job_class);

		RETURN ch_id_created;
	END IF;
	
	# FAILED
	RETURN -4;
END$$
DELIMITER ;

#-----------------------------------------------------------------------
# create pq_del_char(acc_id, ch_id);
#-----------------------------------------------------------------------
DELIMITER $$
DROP FUNCTION IF EXISTS pq_del_char$$

CREATE DEFINER=`pq_user`@`%` FUNCTION pq_del_char(
		_acc_id INT UNSIGNED
		, _ch_id INT UNSIGNED)
RETURNS INT
BEGIN
	#
	# If this char is a guild master and have members,
	# the char can not be deleted.
	# If have no members, possible.
	#
	
	DECLARE var_guild_id INT UNSIGNED;
	DECLARE var_guild_master_ch_id INT UNSIGNED;
	DECLARE var_guild_member_count INT UNSIGNED;
	DECLARE var_job_class TINYINT UNSIGNED;
	DECLARE var_svr_id INT UNSIGNED;
	DECLARE var_ch_level TINYINT UNSIGNED;

	SELECT guild_id
		, job_class
		, svr_id
		, ch_level
		INTO var_guild_id
			, var_job_class
			, var_svr_id
			, var_ch_level
		FROM pq_character 
		WHERE ch_id = _ch_id AND del_flag = 0;
	
	IF var_guild_id IS NULL
	THEN 
		# Character is not found.
		RETURN -1;
	END IF;

	IF var_guild_id <> 0
	THEN
		# Get guild master ch_id.
		SELECT guild_master_ch_id INTO var_guild_master_ch_id
			FROM pq_guild 
			WHERE guild_id = var_guild_id;

		# Get guild member count.
		SELECT COUNT(*) INTO var_guild_member_count
			FROM pq_guild_member
			WHERE guild_id = var_guild_id;

		# If being guild master and having any member,
		# can not be deleted.
		IF var_guild_master_ch_id = _ch_id
		THEN
			IF var_guild_member_count > 1
			THEN
				RETURN -2;
			END IF;

			# Deleting guild.
			DELETE FROM pq_guild 
				WHERE guild_id = var_guild_id;

		END IF;
	END IF;

	# Deleting friends.
	DELETE FROM pq_friend WHERE ch_id = _ch_id;
	DELETE FROM pq_friend WHERE friend_ch_id = _ch_id;

	# Deleting guild member info.
	DELETE FROM pq_guild_member WHERE ch_id = _ch_id;

	# Deleting store.
	DELETE FROM pq_store WHERE ch_id = _ch_id;

	UPDATE pq_character SET guild_id = 0
				, del_flag = 1
				, del_time = NOW()
		WHERE acc_id = _acc_id AND ch_id = _ch_id;

	#
	# log
	#
	INSERT INTO pq_mng_del_ch (
			svr_id
			, act_time
			, ch_id
			, job_class
			, ch_level)
		VALUES (
				var_svr_id
				, NOW()
				, _ch_id
				, var_job_class
				, var_ch_level);

	RETURN 0;
END$$
DELIMITER ;

