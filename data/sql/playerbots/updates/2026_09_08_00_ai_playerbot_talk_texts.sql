-- First-class BotTalk ambient / callout lines (vanilla flavor).
-- Keys are used by src/Mgr/Text/BotTalk.cpp. Fallbacks exist in C++ if this
-- update has not been applied yet.

DELETE FROM ai_playerbot_texts WHERE name IN (
    'talk_group_ambient',
    'talk_world_ambient',
    'talk_pull',
    'talk_incoming'
);
DELETE FROM ai_playerbot_texts_chance WHERE name IN (
    'talk_group_ambient',
    'talk_world_ambient',
    'talk_pull',
    'talk_incoming'
);

INSERT INTO `ai_playerbot_texts`
    (`name`, `text`, `say_type`, `reply_type`,
     `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`,
     `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`)
VALUES
    ('talk_group_ambient', 'Ready.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'On you.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'Let''s go.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'I need a drink after this.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'Watch the patrol.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'Pull when you''re ready.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'I''ve got the next one.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_group_ambient', 'Buffs are up.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_world_ambient', 'Anyone for a run?', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_world_ambient', 'Looking for a group.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_world_ambient', 'Good hunting.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_world_ambient', 'Need a tank for Deadmines.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_world_ambient', 'WTS greens, cheap.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_world_ambient', 'Heading to the inn.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_pull', 'Pulling.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_pull', 'Incoming.', 0, 0, '', '', '', '', '', '', '', ''),
    ('talk_incoming', 'Incoming!', 1, 0, '', '', '', '', '', '', '', ''),
    ('talk_incoming', 'Adds on me!', 1, 0, '', '', '', '', '', '', '', '');

INSERT INTO ai_playerbot_texts_chance (name, probability) VALUES
    ('talk_group_ambient', 100),
    ('talk_world_ambient', 100),
    ('talk_pull', 100),
    ('talk_incoming', 100);
