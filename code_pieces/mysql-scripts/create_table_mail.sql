#
# jesssoft.
#

use pq_account_db;

#-----------------------------------------------------------------------
# pq_mail table creation.
#-----------------------------------------------------------------------
DROP TABLE IF EXISTS pq_mail;

CREATE TABLE pq_mail
(
 	svr_id		INT UNSIGNED NOT NULL,
	mail_id		INT UNSIGNED NOT NULL AUTO_INCREMENT,
	from_ch_id	INT UNSIGNED NOT NULL,
	from_ch_name	VARCHAR(12) BINARY NOT NULL,
	to_ch_id	INT UNSIGNED NOT NULL,
	title		VARCHAR(16) BINARY NOT NULL,
	text		VARCHAR(300) BINARY NOT NULL,
	date_sent	DATETIME NOT NULL,
	read_flag	TINYINT NOT NULL,
	receipt_flag	TINYINT NOT NULL,
	to_guild_id	INT UNSIGNED NOT NULL,
	from_guild_id	INT UNSIGNED NOT NULL,
	gold		INT UNSIGNED NOT NULL,
	item_inst_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(mail_id, to_ch_id)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;


