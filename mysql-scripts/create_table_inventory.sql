#
# jesssoft.
#

use pq_account_db;

#-----------------------------------------------------------------------
# pq_item_inst table creation.
#-----------------------------------------------------------------------
DROP TABLE IF EXISTS pq_item_inst;

CREATE TABLE pq_item_inst
(
 	item_inst_id	INT UNSIGNED NOT NULL AUTO_INCREMENT,
	item_id		INT UNSIGNED NOT NULL,
	item_count	SMALLINT UNSIGNED NOT NULL,
	strength	TINYINT UNSIGNED NOT NULL,
	PRIMARY KEY(item_inst_id, item_id)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

#-----------------------------------------------------------------------
# pq_inventory table creation.
#-----------------------------------------------------------------------
DROP TABLE IF EXISTS pq_inventory;

CREATE TABLE pq_inventory
(
 	ch_id		INT UNSIGNED NOT NULL,
	page_no		TINYINT UNSIGNED NOT NULL,
	slot_no		TINYINT	UNSIGNED NOT NULL,
	item_inst_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(ch_id, page_no, slot_no)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

#-----------------------------------------------------------------------
# pq_inventory table creation.
#-----------------------------------------------------------------------
DROP TABLE IF EXISTS pq_inventory_rune;

CREATE TABLE pq_inventory_rune
(
 	ch_id		INT UNSIGNED NOT NULL,
	rune_id		INT UNSIGNED NOT NULL,
	PRIMARY KEY(ch_id, rune_id)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;


