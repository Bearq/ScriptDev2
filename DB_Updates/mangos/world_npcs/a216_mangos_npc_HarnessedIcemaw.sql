-- npc_harnessed_icemaw
UPDATE creature_template SET flags_extra = 4096, ScriptName = "npc_harnessed_icemaw"  WHERE entry = 30468;
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `minhealth` = 12175, `maxhealth` = 12175, `mechanic_immune_mask` = 75511891 WHERE `entry` = 30468;