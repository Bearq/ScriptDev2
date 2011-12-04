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
SDName: boss_anubarak_trial
SD%Complete:
SDAuthor: Bear
SDComment: Thank for MichalPolko
SDCategory:
EndScriptData */
 
/*******************
I dont know if spell Submerge clear all debufs (Dot's Bleed etc). I use spell Clear to clear all debufs.
Problem with all slow spell AoE. Permafrost dont work. Im use trigger to disable spikes.

-------------------------------

Spawn adds in normal/hero is ok.
Respawn Frost Sphere is ok.
Submerge and Emerge work.
********************/
 
#include "precompiled.h"
#include "trial_of_the_crusader.h"
 
 
enum Summons
{
    NPC_FROST_SPHERE            = 34606,
    NPC_BURROWER                = 34607,
    NPC_SCARAB                  = 34605,
    NPC_SPIKE                   = 34660,
    NPC_ANUBARAK_PERMAFROST     = 198722,
};
 
enum Spells
{
    //Phaze 1 Land
    SPELL_SLASH             = 66012,    
    SPELL_COLD              = 67700,    
    SPELL_COLD_H            = 68509,
    //Phase 2 Underground
    SPELL_SPIKES_VISUAL     = 65921,    
    SPELL_PURSUED           = 67574,    //Attack Target 
    SPELL_IMPALE            = 67858,
    SPELL_IMPALE_DMG        = 53458,    // 3k dmg
    //SPELL_IMPALE_H          = 67859,
    SPELL_SPIKE_PERMA       = 66181,    //When Spike hit Permafrost
    SPELL_SUBMERGE          = 66936,    //Go Anubarak to underground
    SPELL_EMERGE            = 65982,    //Up Anubarak to ground
    //Phase 3 Land
    SPELL_SWARM             = 67630,    
    SPELL_SWARM_H           = 68647,
    SPELL_BERSERK           = 26662,
    SPELL_CLEAR             = 34098,    //Clear all debufs when Anub'arak go underground
 
    //Spells Frost Sphere
    SPELL_FROST_SPHERE      = 67539,
    SPELL_PERMAFROST        = 66193,    //Normal 10man      Reducing speed 30%
    SPELL_PERMAFROST_H      = 67855,    //Normal 25man      Reducing speed 80%
    SPELL_PERMAFROST_VISUAL = 65882,
 
    //Spells Nerubian Burrowers
    SPELL_FRENZY            = 66129,    //Incrased movement/cast/attack speed for 100%
    SPELL_EXPOSE            = 67847,    //Incrased psychical damage for 25% stack to 9times
    SPELL_MOB_SUBMERGE      = 67322,    //Wiping Agro and go to underground
    SPELL_STRIKE            = 66134,    //Only in hero mode
 
    //Spells Swarm Scarabs
    SPELL_ACID              = 67861,    //Infinite nature dmg
    SPELL_DETERMINATION     = 66092,    //Recoveres hp and incrase movement speed 100%
    
    //Summon mob's
    SPELL_SUMMON_SCARAB     = 66339,
    SPELL_SUMMON_BURROWER   = 66332,
 
    SUMMON_SCARAB           = NPC_SCARAB,
    SUMMON_BORROWER         = NPC_BURROWER,
    SUMMON_FROSTSPHERE      = NPC_FROST_SPHERE,
 
    
    //Spike
    SPELL_SUMMON_SPIKE      = 66169,
    SPELL_SPIKE_SPEED1      = 65920,    //speed only 12 sec
    SPELL_SPIKE_TRAIL       = 65921,
    SPELL_SPIKE_SPEED2      = 65922,    //speed x2 m_creature and spikes
    SPELL_SPIKE_SPEED3      = 65923,    //speed m_creature and spikes
    SPELL_SPIKE_TELE        = 66170,  
 
};
 
enum Yell
{
    SAY_AGGRO               = -1649064,      // This place will serve as your tomb!
    //SAY_SUBMERGE          =                // Auum na-l ak-k-k-k, isshhh. Rise, minions. Devour...
    SAY_SWARM               = -1649070,      // The swarm shall overtake you!
    SAY_KILL                = -1713563,      // F-lakkh shir! 
    //SAY_KILL_2            =                // Another soul to sate the host.
    SAY_DEATH               = -1713564,      // I have failed you, master...
    //SAY_OUTRO             =                // Highlord Tirion Fordring yells: Champions, you're alive! 
                                             /* Not only have you defeated every challenge of the Trial of the Crusader, but thwarted Arthas directly! 
                                                Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of my mages to transport you back to the surface!*/
};
 
enum Phase
{
    PHASE_START             = 1,
    PHASE_UNDERGROUND       = 2,
    PHASE_WAIT              = 3,        //Anty stun phase. Anub'arak dont use any spell. Only autoatack for 4sec.
    PHASE_END               = 4,
};
 
static float afSpawnLocations[10][3]=
{
    //Spawn Frost Sphere
    {786.6439f, 108.2498f, 155.6701f},
    {806.8429f, 150.5902f, 155.6701f},
    {759.1386f, 163.9654f, 155.6701f},
    {744.3701f, 119.5211f, 155.6701f},
    {710.0211f, 120.8152f, 155.6701f},
    {706.6383f, 161.5266f, 155.6701f},
 
    //Spawn Nerubian Burrowers and Scrabs
    {740.2935f, 190.1425f, 145.1153f},
    {691.3117f, 182.9982f, 142.1179f},
    {687.7182f, 100.4481f, 142.1190f},
    {735.1433f, 74.5281f,  142.1190f},
};
 
/*************
** Anub'arak
**************/
struct MANGOS_DLL_DECL boss_anubarak_trialAI : public BSWScriptedAI
{
    boss_anubarak_trialAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHardMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL);
        m_bIsNormalMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL);
        m_bIs25Hero = m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC;
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
 
    bool m_bIsHardMode;
    bool m_bIsNormalMode;
    bool m_bIs25Hero;
 
    Difficulty m_uiMapDifficulty;
    uint8  m_uiPhase;
    uint32 m_uiSlashTimer;
    uint32 m_uiColdTimer;
    uint32 m_uiSubmergeTimer;
    uint32 m_uiEmergeTimer;
    uint32 m_uiWaitTimer;
    uint32 m_uiSpikesTimer;
    uint32 m_uiSwarmTimer;  
    uint32 m_uiBerserk;
 
    uint32 m_uiSummonBurrowerTimer;
    uint32 m_uiSummonFrostSphereTimer;
    uint32 m_uiSummonScarabTimer;
 
 
    void Reset()
    {
        m_uiSlashTimer              = 15000;
        m_uiColdTimer               = 17000;
        m_uiSubmergeTimer           = 30000;
        m_uiEmergeTimer             = 60000;
        m_uiWaitTimer               = 4000;
        m_uiSpikesTimer             = 6000;
        m_uiSwarmTimer              = 2000;
        m_uiBerserk                 = 900000;
 
        m_uiSummonBurrowerTimer     = m_bIsHardMode ? 10000 : 8000;            //Nerubian Burrower
        m_uiSummonScarabTimer       = 10000;                                   //Swarm Scrab
        m_uiSummonFrostSphereTimer  = 20000; 
 
        m_uiPhase = PHASE_START;
        m_creature->SetVisibility(VISIBILITY_ON);
 
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
       
    }
 

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        SummonFrostSphere();
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }
 
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, FAIL);
    }
 
    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, DONE);
 
        DoScriptText(SAY_DEATH, m_creature);
    }
 
    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_FROST_SPHERE)
        {
            if(m_bIsNormalMode)
            {
                m_creature->Respawn();
            }
        }
    }

    //Summon Burrowers
    void DoSummonBurrowers()
    {
        m_creature->SummonCreature(NPC_BURROWER, 740.2935f, 190.1425f, 145.1153f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);

        if(m_bIsHardMode)
        {
            m_creature->SummonCreature(NPC_BURROWER, 691.3117f, 182.9982f, 142.1179f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
        if(m_bIs25Hero)
        {
            m_creature->SummonCreature(NPC_BURROWER, 691.3117f, 182.9982f, 142.1179f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_creature->SummonCreature(NPC_BURROWER, 687.7182f, 100.4481f, 142.1190f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_creature->SummonCreature(NPC_BURROWER, 735.1433f, 74.5281f, 142.1190f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }

    //Summon Scarab
    void SummonScarab()
    {
        m_creature->SummonCreature(NPC_SCARAB, 740.2935f, 190.1425f, 145.1153f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_SCARAB, 691.3117f, 182.9982f, 142.1179f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_SCARAB, 687.7182f, 100.4481f, 142.1190f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_SCARAB, 735.1433f, 74.5281f, 142.1190f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    //Summon Frost Sphere
    void SummonFrostSphere()
    {
        m_creature->SummonCreature(NPC_FROST_SPHERE, 786.6439f, 108.2498f, 155.6701f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_FROST_SPHERE, 806.8429f, 150.5902f, 155.6701f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_FROST_SPHERE, 759.1386f, 163.9654f, 155.6701f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_FROST_SPHERE, 744.3701f, 119.5211f, 155.6701f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_FROST_SPHERE, 710.0211f, 120.8152f, 155.6701f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_FROST_SPHERE, 706.6383f, 161.5266f, 155.6701f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
 
        switch (m_uiPhase)
        {
            case PHASE_END:
            {
                if(m_uiSlashTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLASH);
                    m_uiSlashTimer = 15000;
                }else m_uiSlashTimer -= uiDiff;
 
                //5 Players
                if(m_uiColdTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_COLD) == CAST_OK)
                        m_uiColdTimer = 17000;
                }else m_uiColdTimer -= uiDiff;
 
                if(m_uiSwarmTimer < uiDiff)
                {   
                    DoCastSpellIfCan(m_creature, SPELL_SWARM);
                    m_uiSwarmTimer = 2500000;
                }else m_uiSwarmTimer -= uiDiff;
 
                // TODO: 1 on 10normal, 2 on 10hero, 2 on 25normal, 4 on 25hero
                if(m_uiSummonBurrowerTimer < uiDiff)
                {
                    DoSummonBurrowers();
                    m_uiSummonBurrowerTimer = 10000;
                }else m_uiSummonBurrowerTimer -= uiDiff;
 
                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_START:
            {
                if(m_uiSlashTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLASH) == CAST_OK)
                        m_uiSlashTimer = 15000;
                }else m_uiSlashTimer -= uiDiff;
 
                if(m_uiColdTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_COLD) == CAST_OK)
                        m_uiColdTimer = 17000;
                }else m_uiColdTimer -= uiDiff;
 
                //TODO: 1 on 10normal, 2 on 10hero, 2 on 25normal, 4 on 25hero
                if(m_uiSummonBurrowerTimer < uiDiff)
                {
                    DoSummonBurrowers();
                    m_uiSummonBurrowerTimer = m_bIsHardMode ? 10000 : 8000; 
                }else m_uiSummonBurrowerTimer -= uiDiff;
                    
                if(m_uiSubmergeTimer < uiDiff)
                {
                    m_uiSubmergeTimer = 90000;
                    m_uiPhase = PHASE_WAIT;
                }else m_uiSubmergeTimer -= uiDiff;
    
                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_WAIT:
            {
                if(m_uiWaitTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_SUBMERGE);
                    m_creature->CastSpell(m_creature, SPELL_CLEAR, true);
                    m_creature->RemoveAllAuras();
                    m_creature->MonsterTextEmote("Anub'arak burrows into the ground!", 0, true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_uiWaitTimer = 4000;
                    m_uiPhase = PHASE_UNDERGROUND;
                }else m_uiWaitTimer -= uiDiff;
 
                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_UNDERGROUND:
            {
                m_creature->AttackStop();
 
                if(m_uiSpikesTimer < uiDiff)
                {
                    m_creature->SummonCreature(NPC_SPIKE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 3000000);
                    m_uiSpikesTimer = 100000;
                }else m_uiSpikesTimer -= uiDiff;
 
                if(m_uiSummonScarabTimer < uiDiff)
                {
                    SummonScarab();
                    m_uiSummonScarabTimer = m_bIsHardMode ? 15000 : 10000;     
                }else m_uiSummonScarabTimer -= uiDiff;
 
                if(m_uiEmergeTimer < uiDiff)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                    DoCastSpellIfCan(m_creature, SPELL_EMERGE);
                    m_creature->MonsterTextEmote("Anub'arak emerges from the ground!", 0, true);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_uiEmergeTimer = 60000;
                    m_uiSpikesTimer = 6000;
                    m_uiPhase = PHASE_START;
                }else m_uiEmergeTimer -= uiDiff;
       
                break;
            }
 
            if (m_creature->GetHealthPercent() < 30.0f)
            {
                m_uiPhase = PHASE_END;
            }    
 
        }
    }
};
 
/*************
** Swarm Scarab
**************/
struct MANGOS_DLL_DECL mob_swarm_scarabAI : public BSWScriptedAI
{
    mob_swarm_scarabAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
 
    ScriptedInstance* m_pInstance;

    uint32 m_uiAcidTimer;
    uint32 m_uiDeterminationTimer;
 
    void Reset()
    {
        m_uiAcidTimer = 20000;
        m_uiDeterminationTimer = urand(10000, 60000);
 
        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
    }
 
    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_ANUBARAK) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
 
        if(m_uiAcidTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ACID);
            m_uiAcidTimer = 20000;
        }else m_uiAcidTimer -= uiDiff;
 
        if(m_uiDeterminationTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_DETERMINATION);
            m_uiDeterminationTimer = urand(10000, 60000);
        }else m_uiDeterminationTimer -= uiDiff;
 
        DoMeleeAttackIfReady();
    }
};
 
/*********************
** Nerubian Borrower
**********************/
struct MANGOS_DLL_DECL mob_nerubian_borrowerAI : public BSWScriptedAI
{
    mob_nerubian_borrowerAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHardMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL);
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
    bool m_bIsHardMode;
    bool m_bEmerge;

    Difficulty m_uiMapDifficulty;

    uint8 m_uiPhase;
    uint32 m_uiExposeTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiStrikeTimer;         //Only in Heroic Mode!
    uint32 m_uiEmergeTimer;
 
    void Reset()
    {
        m_uiExposeTimer      = 9000;
        m_uiEmergeTimer      = 5000;
        m_uiFrenzyTimer      = 1000;
        m_uiStrikeTimer      = 20000;
 
        m_creature->SetInCombatWithZone();
        m_uiPhase = PHASE_START;
    }
 
    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_ANUBARAK) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
 
       switch (m_uiPhase)
       {
            case PHASE_START:
            {
                if(m_uiFrenzyTimer < uiDiff)
                {
                    if(Creature* pTemp = GetClosestCreatureWithEntry(m_creature, NPC_BURROWER, 12.0f))
                    {
                        DoCastSpellIfCan(m_creature, SPELL_FRENZY);
                    }
                    m_uiFrenzyTimer = 1000;
                }else m_uiFrenzyTimer -= uiDiff;
 
                if(m_uiExposeTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_EXPOSE);
                    m_uiExposeTimer = 9000;
                }else m_uiExposeTimer -= uiDiff;
 
                //Hard Mode! 
                if(m_bIsHardMode)
                {
                    if(m_uiStrikeTimer <= uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            DoCastSpellIfCan(pTarget, SPELL_STRIKE);
                            m_uiStrikeTimer = 20000;
                        }
                    }else m_uiStrikeTimer -= uiDiff;
                }
 
                if(m_creature->GetHealthPercent() < 60.0f)
                {
                    if(Creature *pMob = GetClosestCreatureWithEntry(m_creature, NPC_ANUBARAK_PERMAFROST, 50.0f))
                    {
                        if(m_creature->IsInRange(pMob, 11.0f, 50.0f))
                        {
                            m_creature->CastSpell(m_creature, SPELL_SUBMERGE, true);
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_creature->SetVisibility(VISIBILITY_OFF);
                            m_creature->SetHealth(m_creature->GetMaxHealth());
                            m_creature->RemoveAllAuras();
                            m_creature->AttackStop();
                            DoResetThreat();
 
                            m_uiPhase = PHASE_UNDERGROUND;
                        }
                    }
                }
 
                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_UNDERGROUND:
            {
                if(m_uiEmergeTimer < uiDiff)
                {
                    m_creature->CastSpell(m_creature, SPELL_EMERGE, true);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->SetVisibility(VISIBILITY_ON);
                    m_uiEmergeTimer = 8000;
                    m_uiPhase = PHASE_START;
                }else m_uiEmergeTimer -= uiDiff;
 
                break;
            }
        }
    }
};
 
/***************
** Frost Sphere
****************/
struct MANGOS_DLL_DECL mob_frost_sphereAI : public BSWScriptedAI
{
    mob_frost_sphereAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
    bool m_bPermafrostVisual;
 
    ObjectGuid m_uiTriggerGUID;
    uint32 m_uiSummonTimer;
    
    void Reset()
    {
        m_uiSummonTimer = 5000;
        m_creature->SetRespawnDelay(DAY);
        m_creature->CastSpell(m_creature, SPELL_FROST_SPHERE, true); 
    }
 
    void JustDied(Unit* pVictim)
    {
        FakeDeath();
        m_creature->Respawn();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        FakeDeath();
        uiDamage = 0;
        m_creature->Respawn();
    }
 
    void FakeDeath()
    {
        //m_creature->SetVisibility(VISIBILITY_OFF);
        SetCombatMovement(false);

        float x = m_creature->GetPositionX();
        float y = m_creature->GetPositionY();
        float z = m_creature->GetPositionZ()-13.0f;
        m_creature->SummonCreature(NPC_ANUBARAK_PERMAFROST, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000);
    }
 
    void UpdateAI(const uint32 uiDiff)
    {
       //if (m_pInstance && m_pInstance->GetData(TYPE_ANUBARAK) != IN_PROGRESS) 
            //m_creature->ForcedDespawn();
 
       /*if(m_uiSummonTimer <= uiDiff)
       {
            float x = m_creature->GetPositionX();
            float y = m_creature->GetPositionY();
            float z = m_creature->GetPositionZ()-13.0f;
 
            m_creature->SummonCreature(NPC_ANUBARAK_PERMAFROST, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000);
            m_uiSummonTimer = 30000;
       }else m_uiSummonTimer -= uiDiff;*/
 
    }
};
 
/*****************
** Anubarak Spike
******************/
struct MANGOS_DLL_DECL mob_anubarak_spikeAI : public BSWScriptedAI
{
    mob_anubarak_spikeAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
    bool m_bIncrasedSpeed;
 
    ObjectGuid m_uiPursuedTargetGUID;
    uint8 m_uiSpeed;
    uint32 m_uiDiedTimer;
    uint32 m_uiDmgTimer;
    uint32 m_uiIncreaseSpeedTimer;
    uint32 m_uiPermaTimer;
    uint32 m_uiPursuedTimer;
 
    void Reset()
    {
        m_uiDiedTimer   = 270000;
        m_uiDmgTimer    = 1000;
        m_uiPermaTimer  = 1000;
        m_uiPursuedTimer = 3000;
 
        m_bIncrasedSpeed = false;
        m_uiPursuedTargetGUID.Clear();
        m_creature->SetInCombatWithZone();
        m_creature->SetVisibility(VISIBILITY_ON);   //delete when spikes work
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
 
    void EnterCombat(Unit* who)
    {
        m_uiSpeed = 0;
        m_uiIncreaseSpeedTimer = 4000;
    }
 
    void Impale()
    {
        if (Player* pPursued = m_creature->GetMap()->GetPlayer(m_uiPursuedTargetGUID))
        {
            if(pPursued->HasAura(SPELL_PURSUED, EFFECT_INDEX_0))
            {
                m_creature->CastSpell(m_creature, SPELL_SPIKES_VISUAL, true);
                m_bIncrasedSpeed = true; 
            }
        }
    }
 
    void HitImpale()
    {
        Map* pMap = m_creature->GetMap();
        if (pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();
 
            if (PlayerList.isEmpty())
                return;
 
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && i->getSource()->IsWithinDistInMap(m_creature, 4.0f))
                    i->getSource()->CastSpell(i->getSource(), SPELL_IMPALE_DMG, true);
            }
        }
 
        //If Spikes in 1y of target they cast Imape (10-Players: 14,138 to 15,862) (25-Players: 17,672 to 19,828)
        /*if (Player* pPursued = m_creature->GetMap()->GetPlayer(m_uiPursuedTargetGUID))
        {
            if(m_creature->IsWithinDistInMap(pPursued, 1.0f))
            {
                //Cast spell. Impale doesn't work
            }
        }*/
 
    }
 
    void ChangeTarget()
    {
        if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
        {
            m_uiPursuedTargetGUID = pTarget->GetObjectGuid();
            m_creature->CastSpell(pTarget, SPELL_PURSUED, true);
            m_creature->MonsterTextEmote("Anub'arak's spikes pursue!", 0, true);
            m_creature->NearTeleportTo(pTarget->GetPositionX(), pTarget->GetPositionY()-10.0f, pTarget->GetPositionZ(), 0.0f);
            m_creature->TauntApply(pTarget);
            m_uiIncreaseSpeedTimer = 4000;
            m_uiSpeed = 0;
            Impale();
        }
    }
 
    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_ANUBARAK) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
 
        if(m_bIncrasedSpeed)
        {
            if (m_uiIncreaseSpeedTimer)
            {
                if (m_uiIncreaseSpeedTimer <= uiDiff)
                {
                    switch (m_uiSpeed)
                    {
                        case 0:
                            m_creature->CastSpell(m_creature, SPELL_SPIKE_SPEED1, false);
                            m_uiSpeed = 1;
                            m_uiIncreaseSpeedTimer = 6000;
                            break;
                        case 1:
                            m_creature->CastSpell(m_creature, SPELL_SPIKE_SPEED2, false);
                            m_uiSpeed = 2;
                            m_uiIncreaseSpeedTimer = 6000;
                            break;
                        case 2:
                            m_creature->CastSpell(m_creature, SPELL_SPIKE_SPEED3, false);
                            m_uiIncreaseSpeedTimer = 9000;
                            m_bIncrasedSpeed = false;
                            break;
                    }
                }else m_uiIncreaseSpeedTimer -= uiDiff;
            }
        }
 
        if(m_uiPermaTimer < uiDiff)
        {
            if(Creature *pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_ANUBARAK_PERMAFROST, 5.0f))
            {
                pTrigger->CastSpell(pTrigger, SPELL_SPIKE_PERMA, true);
                pTrigger->ForcedDespawn();
                m_creature->RemoveAurasDueToSpell(SPELL_IMPALE);
                ChangeTarget();
 
                if (Player* pPursued = m_creature->GetMap()->GetPlayer(m_uiPursuedTargetGUID))
                {
                    if(pPursued->isDead())
                    {
                        ChangeTarget();
                    }
                }
            }
            HitImpale();
            m_uiPermaTimer = 1000;
        }else m_uiPermaTimer -= uiDiff;
 
        if(m_uiPursuedTimer < uiDiff)
        {
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->CastSpell(pTarget, SPELL_PURSUED, true);
                m_uiPursuedTargetGUID = pTarget->GetObjectGuid();
                m_creature->TauntApply(pTarget);
                Impale();
            }
            m_uiPursuedTimer = 3000000;
        }else m_uiPursuedTimer -= uiDiff;
 
        /*if(m_uiDmgTimer < uiDiff)
        {
            HitImpale();
            m_uiDmgTimer = 100;
        }else m_uiDmgTimer -= uiDiff;*/
           
        if(m_uiDiedTimer < uiDiff)
        {
            m_creature->SetDeathState(JUST_DIED);
            m_creature->ForcedDespawn();
            m_uiDiedTimer = 580000;
        }else m_uiDiedTimer -= uiDiff;
    }
};
 
/**********************
** Anubarak Permafrost
***********************/
struct MANGOS_DLL_DECL mob_anubarak_permafrostAI : public BSWScriptedAI
{
    mob_anubarak_permafrostAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
 
    uint32 m_uiPermafrostTimer;
    uint32 m_uiDiedTimer;
 
    void Reset()
    {
        m_uiPermafrostTimer = 4000;
        m_uiDiedTimer = 28000;
 
        m_creature->SetDisplayId(11686);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //m_creature->SetVisibility(VISIBILITY_OFF);
    }
 
    void UpdateAI(const uint32 uiDiff)
    {
       if (m_pInstance && m_pInstance->GetData(TYPE_ANUBARAK) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
 
       if (Creature* pCreature = m_creature->GetMap()->GetCreature(NPC_FROST_SPHERE))
       {
           if(pCreature->isDead())
           {
               m_creature->CastSpell(m_creature, SPELL_PERMAFROST, true);
           }
       }
 
       if(m_uiPermafrostTimer < uiDiff)
       {
           m_creature->CastSpell(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+1.0f, SPELL_PERMAFROST, true);
           m_uiPermafrostTimer = 20000;
       }else m_uiPermafrostTimer -= uiDiff;
 
        if(m_uiDiedTimer < uiDiff)
        {
            m_creature->SetDeathState(JUST_DIED);
            m_creature->ForcedDespawn();
            m_uiDiedTimer = 28000;
        }else m_uiDiedTimer -= uiDiff;
    }
};
 
 
CreatureAI* GetAI_mob_anubarak_spike(Creature* pCreature)
{
    return new mob_anubarak_spikeAI(pCreature);
};
 
CreatureAI* GetAI_boss_anubarak_trial(Creature* pCreature)
{
    return new boss_anubarak_trialAI(pCreature);
}
 
CreatureAI* GetAI_mob_nerubian_borrower(Creature* pCreature)
{
    return new mob_nerubian_borrowerAI(pCreature);
}
 
CreatureAI* GetAI_mob_frost_sphere(Creature* pCreature)
{
    return new mob_frost_sphereAI(pCreature);
}
 
CreatureAI* GetAI_mob_swarm_scarab(Creature* pCreature)
{
    return new mob_swarm_scarabAI(pCreature);
}
 
CreatureAI* GetAI_mob_anubarak_permafrost(Creature* pCreature)
{
    return new mob_anubarak_permafrostAI(pCreature);
};
 
 
void AddSC_boss_anubarak_trial()
{
    Script* newscript;
 
    newscript = new Script;
    newscript->Name = "boss_anubarak_trial";
    newscript->GetAI = &GetAI_boss_anubarak_trial;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "mob_swarm_scarab";
    newscript->GetAI = &GetAI_mob_swarm_scarab;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "mob_nerubian_borrower";
    newscript->GetAI = &GetAI_mob_nerubian_borrower;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "mob_frost_sphere";
    newscript->GetAI = &GetAI_mob_frost_sphere;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "mob_anubarak_spike";
    newscript->GetAI = &GetAI_mob_anubarak_spike;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "mob_anubarak_permafrost";
    newscript->GetAI = &GetAI_mob_anubarak_permafrost;
    newscript->RegisterSelf();
 
}
 

/*

UPDATE `creature_template` SET `mechanic_immune_mask` = 650854363 WHERE `entry` = 34564;

UPDATE `creature_template` SET `spell1` = 0, `spell2` = 0, `spell3` = 0 WHERE `entry` = 34564;
+-- FIXME: additional creature for Permafrost handling
DELETE FROM creature_template WHERE entry = 198722;
INSERT INTO creature_template(entry, KillCredit1, KillCredit2, modelid_1, modelid_2, modelid_3, modelid_4, name, subname, 
IconName, minlevel, maxlevel, minhealth, maxhealth, minmana, maxmana, armor, faction_A, faction_H, npcflag, scale, rank, 
mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, dynamicflags,
family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, 
type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, 
spell1, spell2, spell3, spell4, PetSpellDataId, mingold, maxgold, AIName, MovementType, InhabitType, unk16, unk17, 
RacialLeader, questItem1, questItem2, questItem3, questItem4, movementId, RegenHealth, equipment_id, mechanic_immune_mask, 
flags_extra, ScriptName) VALUES ('198722', '0', '0', '0', '0', '17612', '1126', '0', 'Anub\'arak Permafrost', '0','1', '1', 
'1', '1', '1', '1', '114', '114', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '','0', '3', 
'1.0', '1.0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', 'mob_anubarak_permafrost'); 

UPDATE `creature_template` SET `modelid_1` = 17612, `modelid_2` = 1126, `name` = 'Anub''arak Permafrost', `subname` = 0, 
`unit_flags` = 516 WHERE `entry` = 198722;
*/

















