-- Persistent population-sim roster. Lives in the playerbots DB so a rndbot
-- character reset cannot wipe persona/schedule state.

CREATE TABLE IF NOT EXISTS `pb_sim_account` (
  `sim_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `persona` ENUM('morning','afternoon','evening','night','weekend','raid_logger','banker','pvper') NOT NULL,
  `status` ENUM('active','paused','quit') NOT NULL DEFAULT 'active',
  `quit_at` DATETIME NULL,
  `return_at` DATETIME NULL,
  `created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`sim_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `pb_sim_character` (
  `guid` INT UNSIGNED NOT NULL,
  `sim_id` INT UNSIGNED NOT NULL,
  `role` ENUM('main','alt','bank') NOT NULL DEFAULT 'main',
  `focus` ENUM('level','dungeon','pvp','raid','gold','explore','social') NOT NULL DEFAULT 'level',
  `focus_until` DATETIME NULL,
  `status` ENUM('active','paused','retired') NOT NULL DEFAULT 'active',
  `notes` VARCHAR(128) NULL,
  PRIMARY KEY (`guid`),
  KEY (`sim_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `pb_sim_schedule` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `sim_id` INT UNSIGNED NOT NULL,
  `dow_mask` TINYINT UNSIGNED NOT NULL,
  `login_tod_min` SMALLINT UNSIGNED NOT NULL,
  `duration_min` SMALLINT UNSIGNED NOT NULL,
  `jitter_min` SMALLINT UNSIGNED NOT NULL DEFAULT 25,
  `kind` ENUM('play','raid','ah','quick') NOT NULL DEFAULT 'play',
  PRIMARY KEY (`id`),
  KEY (`sim_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `pb_sim_session` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `guid` INT UNSIGNED NOT NULL,
  `planned_login` DATETIME NOT NULL,
  `planned_logout` DATETIME NOT NULL,
  `actual_login` DATETIME NULL,
  `actual_logout` DATETIME NULL,
  `denied_reason` ENUM('cap','restart_stagger','already_online') NULL,
  PRIMARY KEY (`id`),
  KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
