-- Staff for Varidus (it's not blizzlike model :/ )
UPDATE creature_template SET equipment_id = '93' WHERE entry = '25618';

-- Registering scripts in DB
UPDATE creature_template SET ScriptName = 'npc_varidus_the_flenser' WHERE entry = '25618';
UPDATE creature_template SET ScriptName = 'npc_shadowstalker_getry' WHERE entry = '25729';
UPDATE creature_template SET ScriptName = 'npc_enkilah_necrolord' WHERE entry = '25730';

-- Delete unwanted saurfang from map
DELETE FROM creature WHERE guid = 78915;

-- Change damage of overlord saurfang to 1500 for 2000
UPDATE creature_template SET mindmg = '468' WHERE entry = '25730';
UPDATE creature_template SET maxdmg = '702' WHERE entry = '25730';
UPDATE creature_template SET attackpower = '175' WHERE entry = '25730';
UPDATE creature_template SET dmg_multiplier = '3,4' WHERE entry = '25730';

-- Change infested prisoners respawn time to 20 sec 
UPDATE creature SET spawntimesecs = '20' WHERE guid IN (103197, 103198, 103199, 103200, 103201, 103202, 103203, 103204, 103205, 103206, 103207, 103208, 103209, 103210,
103211, 103212, 103213, 103214, 103215, 103216, 103217, 103218, 103219, 103220, 103221, 103222, 103223, 103224, 103225, 103226, 103227, 103228, 103229, 103230, 103231,
103232, 103233, 103234, 103235, 103236, 103237, 103238); 
