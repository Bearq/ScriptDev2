-- speech for rescued tadpoles, this goes to SD2 db
DELETE FROM script_texts WHERE entry IN (-1999950, -1999951, -1999952, -1999953, -1999954, -1999955, -1999956, -1999957, -1999958, 
-1999959);
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999950', 'Da-da?','0','0','0','0','on_quest_tadpol_say1');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999951', 'Ma-ma?','0','0','0','0','on_quest_tadpol_say2');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999952', 'Me go home?','0','0','0','0','on_quest_tadpol_say3');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999953', 'Play!','0','0','0','0','on_quest_tadpol_say4');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999954', 'Alurglgl.','0','0','0','0','on_quest_tadpol_say5');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999955', 'Close!','0','0','0','0','not_on_quest_tadpol_say1');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999956', 'No go with stranger!','0','0','0','0','not_on_quest_tadpol_say1');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999957', 'You not da-da!','0','0','0','0','not_on_quest_tadpol_say1');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999958', 'You not ma-ma!','0','0','0','0','not_on_quest_tadpol_say1');
INSERT INTO script_texts (entry, content_default, sound, type, language, emote, comment)
VALUES ('-1999959', 'Eww... $R!','0','0','0','0','not_on_quest_tadpol_say1');
