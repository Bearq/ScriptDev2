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
SDName: Boss_Victor_Nefarius
SD%Complete: 
SDComment: Missing some text, Vael beginning event, and spawns Nef in wrong place
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"

enum
{
    SAY_GAMESBEGIN_1                = -1469004,
    SAY_GAMESBEGIN_2                = -1469005,

    GOSSIP_ITEM_NEFARIUS_1          = -3469000,
    GOSSIP_ITEM_NEFARIUS_2          = -3469001,
    GOSSIP_ITEM_NEFARIUS_3          = -3469002,
    GOSSIP_TEXT_NEFARIUS_1          = 7134,
    GOSSIP_TEXT_NEFARIUS_2          = 7198,
    GOSSIP_TEXT_NEFARIUS_3          = 7199,

    NPC_RED_DRAKANOID               = 14264,
    NPC_BLUE_DRAKANOID              = 14261,
    NPC_GREEN_DRAKANOID             = 14262,
    NPC_BLACK_DRAKANOID             = 14265,
    NPC_BRONZE_DRAKANOID            = 14263,
    NPC_CHROMATIC_DRAKANOID         = 14302,

    //Spell for Drakonid
    SPELL_RED_POWER                 = 22558,
    SPELL_BLUE_POWER                = 22559,
    SPELL_GREEN_POWER               = 22561,
    SPELL_BLACK_POWER               = 22560,
    SPELL_BRONZE_POWER              = 22642,

    //Spell Victor Nefarius
    SPELL_NEFARIUS_BARRIER          = 22663,                
    SPELL_SHADOWBOLT                = 21077,
    SPELL_FEAR                      = 12096,                
    SPELL_SILENCE                   = 22666,
    SPELL_SHADOWBOLT_VOLLEY         = 22665,
    

    FACTION_BLACK_DRAGON            = 103,
    FACTION_FRIENDLY                = 35,
    MAX_DRAKE_COUNT                 = 45,
    MAX_DRAKES                      = 5,
};

enum Phase
{
    PHASE_HUMAN     = 1,
    PHASE_DRAGON    = 2,
};

struct SpawnLocation
{
    float m_fX, m_fY, m_fZ;
};

static const SpawnLocation aNefarianLocs[5] =
{
    {-7599.32f, -1191.72f, 475.545f},                       // opening where red/blue/black darknid spawner appear (ori 3.05433)
    {-7526.27f, -1135.04f, 473.445f},                       // same as above, closest to door (ori 5.75959)
    {-7498.177f, -1273.277f, 481.649f},                     // nefarian spawn location (ori 1.798)
    {-7592.0f, -1264.0f, 481.0f},                           // hide pos (useless; remove this)
    {-7502.002f, -1256.503f, 476.758f},                     // nefarian fly to this position
};

static const uint32 aPossibleDrake[MAX_DRAKES] = {NPC_RED_DRAKANOID, NPC_BLUE_DRAKANOID, NPC_GREEN_DRAKANOID, NPC_BLACK_DRAKANOID, NPC_BRONZE_DRAKANOID};

/**********
** Nefarius
***********/
struct MANGOS_DLL_DECL boss_victor_nefariusAI : public ScriptedAI
{
    boss_victor_nefariusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();

        uint8 uiPos1 = urand(0, MAX_DRAKES - 1);
        uint8 uiPos2 = (uiPos1 + urand(1, MAX_DRAKES - 1)) % MAX_DRAKES;

        m_uiDrakeTypeOne = aPossibleDrake[uiPos1];
        m_uiDrakeTypeTwo = aPossibleDrake[uiPos2];
    }

    ScriptedInstance* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiAddsCount;
    uint32 m_uiSummonTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiFunctionTimer;
    uint32 m_uiDrakeTypeOne;
    uint32 m_uiDrakeTypeTwo;

    void Reset()
    {
        m_uiAddsCount               = 0;
        m_uiSummonTimer             = 10*IN_MILLISECONDS;
        m_uiShadowBoltTimer         = 5*IN_MILLISECONDS;
        m_uiShadowBoltVolleyTimer   = 10*IN_MILLISECONDS;
        m_uiFearTimer               = 8*IN_MILLISECONDS;
        m_uiSilenceTimer            = 25*IN_MILLISECONDS;
        m_uiFunctionTimer           = 0;

        m_uiPhase = PHASE_HUMAN;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        // Make visible if needed
        if (m_creature->GetVisibility() != VISIBILITY_ON)
            m_creature->SetVisibility(VISIBILITY_ON);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NEFARIAN, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_NEFARIUS_BARRIER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustReachedHome()
    {
        m_creature->setFaction(FACTION_FRIENDLY);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NEFARIAN, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_NEFARIAN)
        {
            pSummoned->SetWalk(false);

            // see boss_onyxia (also note the removal of this in boss_nefarian)
            pSummoned->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
            pSummoned->SetLevitate(true);

            // Let Nefarian fly towards combat area
            pSummoned->GetMotionMaster()->MovePoint(1, aNefarianLocs[4].m_fX, aNefarianLocs[4].m_fY, aNefarianLocs[4].m_fZ);
        }
        else
        {
            m_uiAddsCount += 1;

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }

        pSummoned->SetRespawnDelay(7*DAY);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        // If Nefarian has reached combat area, let him attack
        if (pSummoned->GetEntry() == NPC_NEFARIAN && uiMotionType == POINT_MOTION_TYPE && uiPointId == 1)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }


    Unit* SelectEnemyTargetWithinMana()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        for(iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if(target = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
                if(target->getPowerType() == POWER_MANA)
                    return target;
        }
        return NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_uiPhase)
        {
            case PHASE_HUMAN:
            {
                if(m_uiShadowBoltTimer < uiDiff)
                {
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if(DoCastSpellIfCan(pTarget, SPELL_SHADOWBOLT) == CAST_OK)
                            m_uiShadowBoltTimer = urand(4*IN_MILLISECONDS, 10*IN_MILLISECONDS);
                    }
                }else m_uiShadowBoltTimer -= uiDiff;

                if(m_uiShadowBoltVolleyTimer < uiDiff)
                {
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if(DoCastSpellIfCan(pTarget, SPELL_SHADOWBOLT_VOLLEY) == CAST_OK)
                            m_uiShadowBoltVolleyTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                    }
                }else m_uiShadowBoltVolleyTimer -= uiDiff;
            
                if(m_uiFearTimer < uiDiff)
                {
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if(DoCastSpellIfCan(pTarget, SPELL_FEAR) == CAST_OK)
                            m_uiFearTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);
                    }
                }else m_uiFearTimer -= uiDiff; 

                if(m_uiSilenceTimer < uiDiff)
                {
                    if(Unit* pTarget = SelectEnemyTargetWithinMana())
                    {
                        if(DoCastSpellIfCan(pTarget, SPELL_SILENCE) == CAST_OK)
                            m_uiSilenceTimer = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS);
                    }
                }else m_uiSilenceTimer -= uiDiff;

                if(m_uiSummonTimer < uiDiff)
                {
                    //Spawn 2 random types of creatures at the 2 locations
                    uint32 uiCreatureId = 0;

                    // 1 in 3 chance it will be a chromatic
                    uiCreatureId = urand(0, 2) ? m_uiDrakeTypeOne : NPC_CHROMATIC_DRAKANOID;
                    m_creature->SummonCreature(uiCreatureId, aNefarianLocs[0].m_fX, aNefarianLocs[0].m_fY, aNefarianLocs[0].m_fZ, 5.000f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*MINUTE*IN_MILLISECONDS);

                    // 1 in 3 chance it will be a chromatic
                    uiCreatureId = urand(0, 2) ? m_uiDrakeTypeTwo : NPC_CHROMATIC_DRAKANOID;
                    m_creature->SummonCreature(uiCreatureId, aNefarianLocs[1].m_fX, aNefarianLocs[1].m_fY, aNefarianLocs[1].m_fZ, 5.000, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*MINUTE*IN_MILLISECONDS);

                    m_uiSummonTimer = 5*IN_MILLISECONDS;
                }else m_uiSummonTimer -= uiDiff;

                if(m_uiAddsCount >= MAX_DRAKE_COUNT)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCastSpellIfCan(m_creature, 33356, CAST_TRIGGERED);
                    m_uiPhase = PHASE_DRAGON;
                }

                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_DRAGON:
            {
                if(m_uiFunctionTimer < uiDiff)
                {
                    if (m_creature->GetVisibility() != VISIBILITY_OFF)
                        m_creature->SetVisibility(VISIBILITY_OFF);

                    m_creature->SummonCreature(NPC_NEFARIAN, aNefarianLocs[2].m_fX, aNefarianLocs[2].m_fY, aNefarianLocs[2].m_fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0, true);
                    m_uiFunctionTimer = 1*DAY;
                }else m_uiFunctionTimer -= uiDiff;

                break;
            }
        }
    }
};
      

/****************
** Red Drakonid
*****************/
struct MANGOS_DLL_DECL mob_red_drakonidAI : public ScriptedAI
{
    mob_red_drakonidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiRedPowerTimer;

    void Reset()
    {
        m_uiRedPowerTimer = 0;
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_NEFARIAN) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiRedPowerTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_RED_POWER);
            m_uiRedPowerTimer = 15*IN_MILLISECONDS;
        }else m_uiRedPowerTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

/****************
** Blue Drakonid
*****************/
struct MANGOS_DLL_DECL mob_blue_drakonidAI : public ScriptedAI
{
    mob_blue_drakonidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBluePowerTimer;

    void Reset()
    {
        m_uiBluePowerTimer = 0;
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_NEFARIAN) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiBluePowerTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLUE_POWER);
            m_uiBluePowerTimer = 6*IN_MILLISECONDS;
        }else m_uiBluePowerTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

/****************
** Green Drakonid
*****************/
struct MANGOS_DLL_DECL mob_green_drakonidAI : public ScriptedAI
{
    mob_green_drakonidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiGreenPowerTimer;

    void Reset()
    {
        m_uiGreenPowerTimer = 0;
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_NEFARIAN) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiGreenPowerTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GREEN_POWER);
            m_uiGreenPowerTimer = 6*IN_MILLISECONDS;
        }else m_uiGreenPowerTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

/****************
** Black Drakonid
*****************/
struct MANGOS_DLL_DECL mob_black_drakonidAI : public ScriptedAI
{
    mob_black_drakonidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBlackPowerTimer;

    void Reset()
    {
        m_uiBlackPowerTimer = 0;
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_NEFARIAN) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiBlackPowerTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLACK_POWER);
            m_uiBlackPowerTimer = 6*IN_MILLISECONDS;
        }else m_uiBlackPowerTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

/****************
** Bronze Drakonid
*****************/
struct MANGOS_DLL_DECL mob_bronze_drakonidAI : public ScriptedAI
{
    mob_bronze_drakonidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBronzePowerTimer;

    void Reset()
    {
        m_uiBronzePowerTimer = 0;
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_NEFARIAN) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiBronzePowerTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BRONZE_POWER);
            m_uiBronzePowerTimer = 6*IN_MILLISECONDS;
        }else m_uiBronzePowerTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

/*********************
** Chromatic Drakonid
**********************/
struct MANGOS_DLL_DECL mob_chromatic_drakonidAI : public ScriptedAI
{
    mob_chromatic_drakonidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBlackPowerTimer;

    void Reset()
    {
        m_uiBlackPowerTimer = 0;
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_NEFARIAN) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiBlackPowerTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLACK_POWER);
            m_uiBlackPowerTimer = 6*IN_MILLISECONDS;
        }else m_uiBlackPowerTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};


bool GossipHello_boss_victor_nefarius(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEFARIUS_1 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_NEFARIUS_1, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_victor_nefarius(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEFARIUS_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_NEFARIUS_2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEFARIUS_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_NEFARIUS_3, pCreature->GetObjectGuid());
            DoScriptText(SAY_GAMESBEGIN_1, pCreature);
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            DoScriptText(SAY_GAMESBEGIN_2, pCreature);
            // remove gossip, set hostile and attack
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pCreature->setFaction(FACTION_BLACK_DRAGON);
            pCreature->CastSpell(pCreature, SPELL_NEFARIUS_BARRIER, false);
            pCreature->AI()->AttackStart(pPlayer);
            break;
    }
    return true;
}


CreatureAI* GetAI_boss_victor_nefarius(Creature* pCreature)
{
    return new boss_victor_nefariusAI(pCreature);
}
CreatureAI* GetAI_mob_red_drakonid(Creature* pCreature)
{
    return new mob_red_drakonidAI(pCreature);
}
CreatureAI* GetAI_mob_blue_drakonid(Creature* pCreature)
{
    return new mob_blue_drakonidAI(pCreature);
}
CreatureAI* GetAI_mob_green_drakonid(Creature* pCreature)
{
    return new mob_green_drakonidAI(pCreature);
}
CreatureAI* GetAI_mob_black_drakonid(Creature* pCreature)
{
    return new mob_black_drakonidAI(pCreature);
}
CreatureAI* GetAI_mob_bronze_drakonid(Creature* pCreature)
{
    return new mob_bronze_drakonidAI(pCreature);
}
CreatureAI* GetAI_mob_chromatic_drakonid(Creature* pCreature)
{
    return new mob_chromatic_drakonidAI(pCreature);
}

void AddSC_boss_victor_nefarius()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_victor_nefarius";
    pNewScript->GetAI = &GetAI_boss_victor_nefarius;
    pNewScript->pGossipHello = &GossipHello_boss_victor_nefarius;
    pNewScript->pGossipSelect = &GossipSelect_boss_victor_nefarius;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="mob_red_drakonid";
    pNewScript->GetAI = &GetAI_mob_red_drakonid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="mob_blue_drakonid";
    pNewScript->GetAI = &GetAI_mob_blue_drakonid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="mob_green_drakonid";
    pNewScript->GetAI = &GetAI_mob_green_drakonid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="mob_black_drakonid";
    pNewScript->GetAI = &GetAI_mob_black_drakonid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="mob_bronze_drakonid";
    pNewScript->GetAI = &GetAI_mob_bronze_drakonid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="mob_chromatic_drakonid";
    pNewScript->GetAI = &GetAI_mob_chromatic_drakonid;
    pNewScript->RegisterSelf();
}
