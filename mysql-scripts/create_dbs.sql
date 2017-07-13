#
# jesssoft
#

#========================================================================
# create an account db.
#========================================================================

# account db creation.
CREATE DATABASE pq_account_db DEFAULT CHARACTER SET utf8
	COLLATE utf8_general_ci;

use pq_account_db;

#------------------------------------------------------------------------
# pq_account table  creation.
#------------------------------------------------------------------------
CREATE TABLE pq_account
(
	acc_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
	acc_name VARCHAR(12) BINARY NOT NULL, 
	acc_passwd VARCHAR(12) BINARY NOT NULL,
	cash INT UNSIGNED NOT NULL DEFAUlT 0,
	private_cash INT UNSIGNED NOT NULL DEFAULT 0,
	creat_time DATETIME NOT NULL,
	PRIMARY KEY(acc_id),
	UNIQUE INDEX pq_account_idx_acc_name(acc_name)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

#------------------------------------------------------------------------
# pq_account_chat_block_list table  creation.
#------------------------------------------------------------------------
CREATE TABLE pq_account_chat_block_list
(
	acc_id INT UNSIGNED NOT NULL,
	reg_time DATETIME NOT NULL,
	PRIMARY KEY(acc_id)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

#------------------------------------------------------------------------
# pq_account_black_list table  creation.
#------------------------------------------------------------------------
CREATE TABLE pq_account_black_list
(
	acc_id INT UNSIGNED NOT NULL,
	reg_time DATETIME NOT NULL,
	why_desc VARCHAR(140) BINARY NOT NULL DEFAULT '',
	PRIMARY KEY(acc_id),
	UNIQUE INDEX pq_account_black_list_idx_acc_id(acc_id)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

#------------------------------------------------------------------------
# pq_current_user table creation.
#------------------------------------------------------------------------
CREATE TABLE pq_current_user
(
	acc_id INT UNSIGNED NOT NULL,
	svr_id INT UNSIGNED NOT NULL,
	login_time DATETIME NOT NULL,
	PRIMARY KEY(acc_id)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;	

#------------------------------------------------------------------------
# pq_bad_word table creation.
#------------------------------------------------------------------------
CREATE TABLE pq_bad_word
(
	bad_word VARCHAR(40) BINARY NOT NULL,
	PRIMARY KEY(bad_word)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

#------------------------------------------------------------------------
# pq_reserved_name table creation.
#------------------------------------------------------------------------
CREATE TABLE pq_reserved_name
(
	reserved_name VARCHAR(12) BINARY NOT NULL,
	PRIMARY KEY(reserved_name)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

