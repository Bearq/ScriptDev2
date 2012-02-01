/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: trial_of_the_crusader
SD%Complete: 
SDAuthor: Bearq
SDComment: 
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Equipment
{
    EQUIP_MAIN_1         = 49303,
    EQUIP_OFFHAND_1      = 47146,
    EQUIP_RANGED_1       = 47267,
    EQUIP_MAIN_2         = 45990,
    EQUIP_OFFHAND_2      = 47470,
    EQUIP_RANGED_2       = 47267,
    EQUIP_DONE           = EQUIP_NO_CHANGE,
};

enum Summons
{
    NPC_DARK_ESSENCE     = 34567,
    NPC_LIGHT_ESSENCE    = 34568,

    NPC_UNLEASHED_DARK   = 34628,   //Concentrated Darkness
    NPC_UNLEASHED_LIGHT  = 34630,   //Concentrated Light
};

enum BossSpells
{
    SPELL_TWIN_SPIKE_L     = 66075,
    SPELL_LIGHT_SURGE      = 65766,
    SPELL_SHIELD_LIGHT     = 65858,
    SPELL_TWIN_PACT_L      = 67303,
    SPELL_LIGHT_VORTEX     = 66046,
    SPELL_LIGHT_TOUCH      = 67297,
    SPELL_TWIN_SPIKE_H     = 66069,
    SPELL_DARK_SURGE       = 65768,
    SPELL_SHIELD_DARK      = 65874,
    SPELL_TWIN_PACT_H      = 67306,
    SPELL_DARK_VORTEX      = 66058,
    SPELL_DARK_TOUCH       = 67282,
    SPELL_TWIN_POWER       = 65916,
    SPELL_LIGHT_ESSENCE    = 65686,
    SPELL_DARK_ESSENCE     = 65684,
    SPELL_BERSERK          = 64238,

    SPELL_UNLEASHED_DARK   = 65808,
    SPELL_UNLEASHED_LIGHT  = 65795,

    SPELL_EMPOVERED_DARK   = 67215,
    SPELL_EMPOVERED_LIGHT  = 67218,
    SPELL_POWERING_UP      = 67604,

    PHASE_NORMAL           = 1,
    PHASE_HEAL             = 2,
};

struct MANGOS_DLL_DECL boss_fjolaAI : public ScriptedAI
{
    boss_fjolaAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    Difficulty m_uiMapDifficulty;
    uint8 m_uiPhase;
    uint32 m_uiTwinSpikeTimer;
    uint32 m_uiSpecialTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiTouchOfLightTimer;
    uint32 m_uiChangePhaseTimer;
    uint32 m_uiSwitchPhaseTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiSummonOrbsTimer;
    uint32 m_uiTwinPowerTimer;

    void Reset()
    {
        m_uiTwinSpikeTimer = 10000;
        m_uiSpecialTimer = 0;
        m_uiVortexTimer = 3*MINUTE*IN_MILLISECONDS;
        m_uiTouchOfLightTimer = urand(35000, 50000);
        m_uiChangePhaseTimer = 0;
        m_uiSwitchPhaseTimer = 17000;
        m_uiHealTimer = 1000;
        m_uiSummonOrbsTimer = 25000;
        m_uiTwinPowerTimer = 2000;
        m_uiPhase = PHASE_NORMAL;

        m_creature->SetHealth(m_creature->GetMaxHealth());
        SetEquipmentSlots(false, EQUIP_MAIN_1, EQUIP_OFFHAND_1, EQUIP_RANGED_1);

        m_creature->SummonCreature(NPC_LIGHT_ESSENCE, 548.0815f, 168.2063f, 394.6575f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 5000);
        m_creature->SummonCreature(NPC_LIGHT_ESSENCE, 580.3616f, 125.1220f, 394.3193f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 5000);

        m_creature->GetMotionMaster()->MovePoint(548.0815f, 168.2063f, 399.6575f, 4.790659f);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALKIRIES, IN_PROGRESS);
        DoScriptText(-1713541,m_creature);
        DoCastSpellIfCan(m_creature, SPELL_LIGHT_SURGE);

        m_creature->SetInCombatWithZone();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALKIRIES, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALKIRIES, DONE);

        DoScriptText(-1713547,m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(-1713544,m_creature,pVictim);
    }

    //Rigt side 
    void SummonOrbs()
    {
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 615.982788f, 120.614517f, 395.132782f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 617.468018f, 126.999695f, 395.110321f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 618.649414f, 133.869843f, 395.091217f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 617.905823f, 142.445221f, 395.112091f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 617.253662f, 150.486526f, 395.107086f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 615.754272f, 157.200745f, 395.128967f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 613.518921f, 163.942703f, 395.196045f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 610.334595f, 169.419250f, 395.197296f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 606.106445f, 175.826523f, 395.141174f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 601.035461f, 181.328415f, 395.136841f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 594.395386f, 184.864258f, 395.128693f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 587.224426f, 185.012360f, 395.132782f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 578.729492f, 184.538696f, 395.139069f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }


    Unit* PlayerWithDarkEssence()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();

        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (pPlayer && pPlayer->HasAura(SPELL_DARK_ESSENCE))
                    return pPlayer;
        }
        return NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_uiPhase)
        {
            case PHASE_NORMAL:
            {
                if(m_uiTwinSpikeTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWIN_SPIKE_L);
                    m_uiTwinSpikeTimer = urand(25000, 35000);
                }else m_uiTwinSpikeTimer -= uiDiff;

                if(m_uiVortexTimer < uiDiff)
                {
                    if(Unit* pPlayer = PlayerWithDarkEssence())
                        m_creature->CastSpell(pPlayer, SPELL_DARK_VORTEX, false);
                    DoScriptText(-1713540,m_creature);
                    m_uiVortexTimer = 3*MINUTE*IN_MILLISECONDS;
                }else m_uiVortexTimer -= uiDiff;

                if(m_uiSummonOrbsTimer < uiDiff)
                {
                    SummonOrbs();
                    m_uiSummonOrbsTimer = 25000;
                }else m_uiSummonOrbsTimer -= uiDiff;

                if(m_bIsHeroicMode)
                {
                    if(m_uiTouchOfLightTimer < uiDiff)
                    {
                        if(Unit* pPlayer = PlayerWithDarkEssence())
                            DoCastSpellIfCan(pPlayer, SPELL_LIGHT_TOUCH);
                            
                        m_uiTouchOfLightTimer = urand(35000, 50000);
                    }else m_uiTouchOfLightTimer -= uiDiff;
                }

                if(m_uiChangePhaseTimer < uiDiff)
                {
                    if(m_creature->GetHealthPercent() <= 50.0f)
                    {
                        m_uiPhase = PHASE_HEAL;
                    }
                    m_uiChangePhaseTimer = 0;
                }else m_uiChangePhaseTimer -= uiDiff;

                if(m_uiTwinPowerTimer < uiDiff)
                {
                    if(Creature* pFjola = m_pInstance->GetSingleCreatureFromStorage(NPC_DARKBANE))
                    {
                        if(pFjola->GetHealthPercent() <= 50.0f && pFjola->HasAura(SPELL_SHIELD_DARK))
                            m_creature->CastSpell(m_creature, SPELL_TWIN_POWER, true);
                    }
                    m_uiTwinPowerTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiTwinPowerTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_HEAL:
            {
                //Cast Shield and channel Twin's Pact
                if(m_uiSpecialTimer < uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    DoCastSpellIfCan(m_creature, SPELL_SHIELD_LIGHT);
                    DoScriptText(-1713539,m_creature);

                    m_uiSpecialTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiSpecialTimer -= uiDiff;

                if(m_uiHealTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_TWIN_PACT_H);
                    m_uiHealTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiHealTimer -= uiDiff;

                if(m_uiSwitchPhaseTimer < uiDiff)
                {
                    m_uiPhase = PHASE_NORMAL;
                    m_uiChangePhaseTimer = 1*MINUTE*IN_MILLISECONDS;
                    m_uiSwitchPhaseTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiSwitchPhaseTimer -= uiDiff;

                break;
            }
        }
    }
};
         
/*******
** boss_eydis
*******/
struct MANGOS_DLL_DECL boss_eydisAI : public ScriptedAI
{
    boss_eydisAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    Difficulty m_uiMapDifficulty;

    uint8 m_uiPhase;
    uint32 m_uiTwinSpikeTimer;
    uint32 m_uiSpecialTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiTouchOfDarkTimer;
    uint32 m_uiChangePhaseTimer;
    uint32 m_uiSwitchPhaseTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiSummonOrbsTimer;
    uint32 m_uiTwinPowerTimer;

    void Reset()
    {
        m_uiTwinSpikeTimer = 10000;
        m_uiSpecialTimer = 0;
        m_uiVortexTimer = 3*MINUTE*IN_MILLISECONDS;
        m_uiTouchOfDarkTimer = urand(35000, 50000);
        m_uiChangePhaseTimer = 0;
        m_uiSwitchPhaseTimer = 17000;
        m_uiHealTimer = 1000;
        m_uiSummonOrbsTimer = 25000;
        m_uiTwinPowerTimer = 2000;
        m_uiPhase = PHASE_NORMAL;

        m_creature->SetHealth(m_creature->GetMaxHealth());
        SetEquipmentSlots(false, EQUIP_MAIN_2, EQUIP_OFFHAND_2, EQUIP_RANGED_2);

        m_creature->SummonCreature(NPC_DARK_ESSENCE, 548.7794f, 132.4107f, 394.3178f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 5000);
        m_creature->SummonCreature(NPC_DARK_ESSENCE, 578.3840f, 166.6422f, 394.6210f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 5000);

        m_creature->GetMotionMaster()->MovePoint(578.3840f, 166.6422f, 399.6210f, 4.5762f);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALKIRIES, IN_PROGRESS);
        DoScriptText(-1713741, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_DARK_SURGE);

        m_creature->SetInCombatWithZone();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALKIRIES, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALKIRIES, DONE);

        DoScriptText(-1713547,m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(-1713543, m_creature, pVictim);
    }

    //Left side 
    void SummonOrbs()
    {
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 546.585266f, 185.174210f, 395.137482f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 539.782043f, 184.579437f, 395.132782f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 32.364929f, 185.067596f, 395.128204f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 526.880676f, 181.854706f, 395.134857f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 522.371277f, 176.282516f, 395.139038f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 518.560669f, 170.454071f, 395.139038f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 514.452087f, 164.506165f, 395.139282f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 510.968750f, 155.672806f, 395.124084f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 509.529144f, 148.740860f, 395.100952f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 509.772919f, 141.779373f, 395.119049f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 508.725464f, 133.896362f, 395.091156f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, 510.899933f, 126.650635f, 395.113190f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_UNLEASHED_DARK, 511.481506f, 120.842834f, 395.131409f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    Unit* PlayerWithLightEssence()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();

        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (pPlayer && pPlayer->HasAura(SPELL_LIGHT_ESSENCE))
                    return pPlayer;
        }
        return NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_uiPhase)
        {
            case PHASE_NORMAL:
            {
                if(m_uiTwinSpikeTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWIN_SPIKE_H);
                    m_uiTwinSpikeTimer = urand(25000, 35000);
                }else m_uiTwinSpikeTimer -= uiDiff;

                if(m_uiVortexTimer < uiDiff)
                {
                    if(Unit* pPlayer = PlayerWithLightEssence())
                        m_creature->CastSpell(pPlayer, SPELL_LIGHT_VORTEX, false);
                    DoScriptText(-1713538, m_creature);
                    m_uiVortexTimer = 3*MINUTE*IN_MILLISECONDS;
                }else m_uiVortexTimer -= uiDiff;

                if(m_uiSummonOrbsTimer < uiDiff)
                {
                    SummonOrbs();
                    m_uiSummonOrbsTimer = 25000;
                }else m_uiSummonOrbsTimer -= uiDiff;

                if(m_bIsHeroicMode)
                {
                    if(m_uiTouchOfDarkTimer < uiDiff)
                    {
                        if(Unit* pPlayer = PlayerWithLightEssence())
                            DoCastSpellIfCan(pPlayer, SPELL_DARK_TOUCH);
                            
                        m_uiTouchOfDarkTimer = urand(35000, 50000);
                    }else m_uiTouchOfDarkTimer -= uiDiff;
                }

                if(m_uiChangePhaseTimer < uiDiff)
                {
                    if(m_creature->GetHealthPercent() <= 50.0f)
                        m_uiPhase = PHASE_HEAL;

                    m_uiChangePhaseTimer = 0;
                }else m_uiChangePhaseTimer -= uiDiff;

                if(m_uiTwinPowerTimer < uiDiff)
                {
                    if(Creature* pFjola = m_pInstance->GetSingleCreatureFromStorage(NPC_LIGHTBANE))
                    {
                        if(pFjola->GetHealthPercent() <= 50.0f && pFjola->HasAura(SPELL_SHIELD_LIGHT))
                            m_creature->CastSpell(m_creature, SPELL_TWIN_POWER, true);
                    }
                    m_uiTwinPowerTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiTwinPowerTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_HEAL:
            {
                //Cast Shield and channel Twin's Pact
                if(m_uiSpecialTimer < uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    DoCastSpellIfCan(m_creature, SPELL_SHIELD_DARK);
                    DoScriptText(-1713539,m_creature);

                    m_uiSpecialTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiSpecialTimer -= uiDiff;

                if(m_uiHealTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_TWIN_PACT_H);
                    m_uiHealTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiHealTimer -= uiDiff;

                if(m_uiSwitchPhaseTimer < uiDiff)
                {
                    m_uiPhase = PHASE_NORMAL;
                    m_uiChangePhaseTimer = 1*MINUTE*IN_MILLISECONDS;
                    m_uiSwitchPhaseTimer = 1*MINUTE*IN_MILLISECONDS;
                }else m_uiSwitchPhaseTimer -= uiDiff;

                break;
            }
        }
    }
};


/********
** mob_light_essence
*********/
struct MANGOS_DLL_DECL mob_light_essenceAI : public ScriptedAI
{
    mob_light_essenceAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
    ScriptedInstance* m_pInstance;

    void Reset() 
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetWalk(true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) m_creature->ForcedDespawn();
        if (m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
        {
            Map* pMap = m_creature->GetMap();
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Unit* pPlayer = itr->getSource();
                if (!pPlayer) continue;
                if (pPlayer->isAlive())
                     pPlayer->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
            }

            m_creature->ForcedDespawn();
        }
        return;
    }
};


bool GossipHello_mob_light_essence(Player *player, Creature* pCreature)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
    if(!pInstance) return true;
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
        player->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
//        player->CastSpell(player,SPELL_REMOVE_TOUCH,false); // Not worked now
        player->CastSpell(player,SPELL_LIGHT_ESSENCE,false);
            player->RemoveAurasDueToSpell(SPELL_LIGHT_TOUCH); // Override for REMOVE_TOUCH
        player->CLOSE_GOSSIP_MENU();
    return true;
};

/********
** mob_dark_essence
*********/
struct MANGOS_DLL_DECL mob_dark_essenceAI : public ScriptedAI
{
    mob_dark_essenceAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
    ScriptedInstance* m_pInstance;

    void Reset() 
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetWalk(true);
        m_creature->GetMotionMaster()->MoveRandom();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) m_creature->ForcedDespawn();
        if (m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) {
                    Map* pMap = m_creature->GetMap();
                    Map::PlayerList const &lPlayers = pMap->GetPlayers();
                    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    {
                        Unit* pPlayer = itr->getSource();
                        if (!pPlayer) continue;
                        if (pPlayer->isAlive())
                             pPlayer->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
                    }
            m_creature->ForcedDespawn();
            }
        return;
    }
};

bool GossipHello_mob_dark_essence(Player *player, Creature* pCreature)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
    if(!pInstance) return true;
    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
        player->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
//        player->CastSpell(player,SPELL_REMOVE_TOUCH,false); // Not worked now
        player->CastSpell(player,SPELL_DARK_ESSENCE,false);
            player->RemoveAurasDueToSpell(SPELL_DARK_TOUCH); // Override for REMOVE_TOUCH
        player->CLOSE_GOSSIP_MENU();
    return true;
}

/********
** mob_unleashed_darkA
*********/
struct MANGOS_DLL_DECL mob_unleashed_darkAI : public ScriptedAI
{
    mob_unleashed_darkAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiRangeCheck_Timer;
    uint32 m_uiDieTimer;
    uint32 m_uiMoveRandomTimer;
    Creature* pDarkbane;
    Creature* pLightbane;

    void Reset()
    {
        m_uiRangeCheck_Timer = 1000;
        m_uiDieTimer = 10000;
        m_uiMoveRandomTimer = 1000;

        m_creature->GetMotionMaster()->MoveRandom();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        pDarkbane = m_pInstance->GetSingleCreatureFromStorage(NPC_DARKBANE);
        pLightbane = m_pInstance->GetSingleCreatureFromStorage(NPC_LIGHTBANE);
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void CheckDist()
    {
        Map* pMap = m_creature->GetMap();
        Map::PlayerList const &lPlayers = pMap->GetPlayers();
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
           Player* pPlayer = itr->getSource();

           if(pPlayer->IsWithinDistInMap(m_creature, 2.0f) && pPlayer->HasAura(SPELL_DARK_ESSENCE))
           {
               m_creature->CastSpell(pPlayer, SPELL_POWERING_UP, true);
               m_creature->ForcedDespawn();
           }
           if(pPlayer->IsWithinDistInMap(m_creature, 2.0f) && pPlayer->HasAura(SPELL_LIGHT_ESSENCE))
           {
               m_creature->CastSpell(pPlayer, SPELL_UNLEASHED_DARK, true);
               m_creature->ForcedDespawn();
           }
        }
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
              m_creature->ForcedDespawn();
 
        if (m_uiRangeCheck_Timer < uiDiff)
        {
            CheckDist();
            m_uiRangeCheck_Timer = 1000;
        }else m_uiRangeCheck_Timer -= uiDiff;

        //Move Random
        if(m_uiMoveRandomTimer < uiDiff)
        {
            //Center of floor 
            float fX = 562.998657 -+ float(urand(5.0f, 20.0f));
            float fY = 139.631195 -+ float(urand(5.0f, 20.0f));
            float fZ = m_creature->GetTerrain()->GetHeight(fX, fY, MAX_HEIGHT); 

            m_creature->GetMotionMaster()->MovePoint(fX, fY, fZ, 0.0f);
            m_uiMoveRandomTimer = 3000;
        }else m_uiMoveRandomTimer -= uiDiff;

        if(m_uiDieTimer < uiDiff)
        {
            m_creature->ForcedDespawn();
            m_uiDieTimer = 11000;
        }else m_uiDieTimer -= uiDiff;
    }
};

/********
** mob_unleashed_light
*********/
struct MANGOS_DLL_DECL mob_unleashed_lightAI : public ScriptedAI
{
    mob_unleashed_lightAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiRangeCheck_Timer;
    uint32 m_uiDieTimer;
    uint32 m_uiMoveRandomTimer;
    Creature* pDarkbane;
    Creature* pLightbane;

    void Reset()
    {
        m_uiRangeCheck_Timer = 1000;
        m_uiDieTimer = 10000;
        m_uiMoveRandomTimer = 1000;

        m_creature->GetMotionMaster()->MoveRandom();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        pDarkbane = m_pInstance->GetSingleCreatureFromStorage(NPC_DARKBANE);
        pLightbane = m_pInstance->GetSingleCreatureFromStorage(NPC_LIGHTBANE);
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void CheckDist()
    {
        Map* pMap = m_creature->GetMap();
        Map::PlayerList const &lPlayers = pMap->GetPlayers();
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
           Player* pPlayer = itr->getSource();

           if(pPlayer->IsWithinDistInMap(m_creature, 2.0f) && pPlayer->HasAura(SPELL_LIGHT_ESSENCE))
           {
               m_creature->CastSpell(pPlayer, SPELL_POWERING_UP, true);
               m_creature->ForcedDespawn();
           }
           if(pPlayer->IsWithinDistInMap(m_creature, 2.0f) && pPlayer->HasAura(SPELL_DARK_ESSENCE))
           {
               m_creature->CastSpell(pPlayer, SPELL_UNLEASHED_LIGHT, true);
               m_creature->ForcedDespawn();
           }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
              m_creature->ForcedDespawn();
 
        if (m_uiRangeCheck_Timer < uiDiff)
        {
            CheckDist();
            m_uiRangeCheck_Timer = 1000;
        }else m_uiRangeCheck_Timer -= uiDiff;

        //Move Random
        if(m_uiMoveRandomTimer < uiDiff)
        {
            //Center of floor 
            float fX = 562.998657 -+ float(urand(5.0f, 20.0f));
            float fY = 139.631195 -+ float(urand(5.0f, 20.0f));
            float fZ = m_creature->GetTerrain()->GetHeight(fX, fY, MAX_HEIGHT); 

            m_creature->GetMotionMaster()->MovePoint(fX, fY, fZ, 0.0f);
            m_uiMoveRandomTimer = 3000;
        }else m_uiMoveRandomTimer -= uiDiff;


        if(m_uiDieTimer < uiDiff)
        {
            m_creature->ForcedDespawn();
            m_uiDieTimer = 11000;
        }else m_uiDieTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_fjola(Creature* pCreature)
{
    return new boss_fjolaAI(pCreature);
}

CreatureAI* GetAI_boss_eydis(Creature* pCreature)
{
    return new boss_eydisAI(pCreature);
}

CreatureAI* GetAI_mob_unleashed_light(Creature *pCreature)
{
    return new mob_unleashed_lightAI(pCreature);
}

CreatureAI* GetAI_mob_light_essence(Creature* pCreature)
{
    return new mob_light_essenceAI(pCreature);
}

CreatureAI* GetAI_mob_dark_essence(Creature* pCreature)
{
    return new mob_dark_essenceAI(pCreature);
}

CreatureAI* GetAI_mob_unleashed_dark(Creature *pCreature)
{
    return new mob_unleashed_darkAI(pCreature);
}

void AddSC_twin_valkyr()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_fjola";
    newscript->GetAI = &GetAI_boss_fjola;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_eydis";
    newscript->GetAI = &GetAI_boss_eydis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_unleashed_light";
    newscript->GetAI = &GetAI_mob_unleashed_light;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_unleashed_dark";
    newscript->GetAI = &GetAI_mob_unleashed_dark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_light_essence";
    newscript->GetAI = &GetAI_mob_light_essence;
    newscript->pGossipHello = &GossipHello_mob_light_essence;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_essence";
    newscript->GetAI = &GetAI_mob_dark_essence;
    newscript->pGossipHello = &GossipHello_mob_dark_essence;
    newscript->RegisterSelf();

}
