-- Persistent time-zone offsets for population-sim personas.
-- The offset is relative to the worldserver's local clock.
CREATE TABLE IF NOT EXISTS `pb_sim_account_timezone` (
  `sim_id` INT UNSIGNED NOT NULL,
  `timezone_offset_min` SMALLINT NOT NULL DEFAULT 0,
  PRIMARY KEY (`sim_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT IGNORE INTO `pb_sim_account_timezone` (`sim_id`, `timezone_offset_min`)
SELECT `sim_id`, (MOD(`sim_id`, 10) * 60) - 360
FROM `pb_sim_account`;
