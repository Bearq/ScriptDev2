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
SDName: boss_krick_and_ick
SDComplete: 
SDAuthor: Bearq
SDComment: Pursed add spell confusion for target. Must use hack to use confused for the Ick.
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{
    SAY_AGGRO                           = -1658024,
    SAY_SLAY_1                          = -1658025,
    SAY_SLAY_2                          = -1658026,
    SAY_ORDER_STOP                      = -1658027,
    SAY_ORDER_BLOW                      = -1658028,
    SAY_TARGET_1                        = -1658029,
    SAY_TARGET_2                        = -1658030,
    SAY_TARGET_3                        = -1658031,

    EMOTE_KRICK_MINES                   = -1658032,
    EMOTE_ICK_POISON                    = -1658033,
    EMOTE_ICK_CHASING                   = -1658034,

    /*SAY_SYLVANAS_KRICK_2                = -1658035, //Frostmourne? The lich king is never...
    SAY_JAINA_KRICK_1                   = -1658036, //I swear is tru dont kill me
    SAY_SYLVANAS_KRICK_1                = -1658037, //Worthless gnat! Death is all that awaits you!
    SAY_OUTRO_2                         = -1658038, //Urg... no!!
    SAY_JAINA_KRICK_2                   = -1658039, //Do not think that I shall permit you entry into my master's sanctum so easily. Pursue me if you dare.
    SAY_SYLVANAS_KRICK_2                = -1658040, //What a cruel end. Come, heroes. We must see if the gnome's story is true. If we can separate Arthas from Frostmourne, we might have a chance at stopping him.
    SAY_OUTRO_3                         = -1658041, //A fitting end for a traitor. Come, we must free the slaves and see what is within the Lich King's chamber for ourselves.
    SAY_TYRANNUS_KRICK_1                = -1658042, //
    SAY_OUTRO_4                         = -1658043, //
    SAY_TYRANNUS_KRICK_2                = -1658044, //
    SAY_JAINA_KRICK_3                   = -1658045, //
    SAY_SYLVANAS_KRICK_3                = -1658046, */

    /** OUTRO ALLIANCE **/
    //SAY_KRICK_1                         = brak
    //SAY_JAINA_1                         = brak
    //SAY_KRICK_2                         = brak
    //SAY_JAINA_2                         = brak
    SAY_KRICK_3                         = -1658036, //I swear it is true! Please, don't kill me!!
    SAY_TYRANNUS_1                      = -1658037, //Worthless gnat! Death is all that awaits you!
    SAY_KRICK_4                         = -1658038, //Urg... no!!
    SAY_TYRANNUS_2                      = -1658039, //Do not think that I shall permit you entry into my master's sanctum so easily. Pursue me if you dare.
    SAY_JAINA_3                         = -1658040, //What a cruel end. Come, heroes. We must see if the gnome's story is true. If we can separate Arthas from Frostmourne, we might have a chance at stopping him.
    //SAY_TYRANNUS_3
    //SAY_TYRANNUS_4

    /** HORDE ALLIANCE **/
    //SAY_KRICK_1                       = brak
    //SAY_SYLVANAS_1                    = brak
    //SAY_KRICK_2                       = brak
    SAY_SYLVANAS_2                      = -1658035,
    //SAY_KRICK_2
    SAY_SYLVANAS_3                      = -1658041, 
    //SAY_KRICK_3
    //SAY_TYRANUNUS_1
    //SAY_KRICK_4
    //SAY_TYRANNUS_2
    //SAY_SYLVANAS_3
    SAY_TYRANNUS_3                      = -1658050, 
    SAY_TYRANNUS_4                      = -1658051,



    //Ick
    SPELL_POISON_NOVA                   = 68989,
    SPELL_POISON_NOVA_H                 = 70434,
    SPELL_MIGHTY_KICK                   = 69021,
    SPELL_PURSUED                       = 68987,

    //Krick
    SPELL_TOXIC_WASTE                   = 69024,
    SPELL_TOXIC_WASTE_H                 = 70436,
    SPELL_STRANGULATING                 = 69413,        // spell used by tyrannus at the outro event
    SPELL_SHADOW_BOLT                   = 69028,
    SPELL_EXPLOSIVE_BARRAGE             = 69012,        // maybe 69263?
    SPELL_CONFUSION                     = 69029,
    SPELL_ROOT_SELF         = 33356,

    //Npc Explosive
    NPC_EXPLOSIVE_ORB                   = 36610,
    SPELL_EXPLOSIVE_BARRAGE_ORB         = 69019,
    SPELL_EXPLOSIVE_BARRAGE_ORB_H       = 70433,
    SPELL_EXPLOSIVE_VISUAL              = 69017,        //Effect
    SPELL_EXPLOSIVE_LIGHT               = 69015,

    PHASE_START                         = 1,
    PHASE_BARRAGE                       = 2,

    GOSSIP_MENU_ARTHAS_1                        = 100001,
};


static float afSpawnLocations[6][4]=
{
    { 825.6246f, 116.474f, 509.4035f, 3.52370f }, //First move point jaina or sylvanas
    { 819.7660f, 113.200f, 509.2467f, 0.06377f }, //Second move point - II -
    { 750.6790f, 116.121f, 512.3890f, 0.06377f }, //Spawn point Jaina or sylvanas
    { 919.1620f, 264.228f, 554.8753f, 3.86377f }, //Spawn point Tyrannus
    { 840.8530f, 179.628f, 554.0090f, 4.76377f }, //Move point Tyrannus
};

struct MANGOS_DLL_DECL boss_ickAI : public ScriptedAI
{
    boss_ickAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsIntro;
    bool m_bConfusion;

    uint8 m_uiPhase;
    uint32 m_uiPoisonNovaTimer;
    uint32 m_uiPursueTimer;
    uint32 m_uiMightKickTimer;
    uint32 m_uiConfusionTimer;

    uint32 m_uiExplosivBarrageTimer;
    uint32 m_uiSummonOverTimer;

    void Reset()
    {
        m_uiPoisonNovaTimer     = 45000;
        m_uiPursueTimer         = 60000;
        m_uiMightKickTimer      = 20000;
        m_uiConfusionTimer      = 0;

        m_uiExplosivBarrageTimer = 25000;
        m_uiSummonOverTimer = 18000;
        m_uiPhase = PHASE_START;

        SetCombatMovement(true);
        m_bConfusion = false;
    }

    void KilledUnit(Unit* pVictim)
    {
        if(Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, pKrick);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRICK, IN_PROGRESS);
        
        if(Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
            pKrick->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRICK, DONE);

        m_creature->SummonCreature(NPC_KRICK, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRICK, FAIL);
    }

    //Hack!!! Delete when spell work. 
    void Confusion()
    {
        Map* pMap = m_creature->GetMap();
        Map::PlayerList const &lPlayers = pMap->GetPlayers();
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            Unit* pPlayer = itr->getSource();
            if (pPlayer->isAlive() && pPlayer->HasAura(SPELL_CONFUSION))
            {
                 pPlayer->RemoveAurasDueToSpell(SPELL_CONFUSION);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_uiPhase)
        {
            case PHASE_START:
            {
                if(m_uiPoisonNovaTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_POISON_NOVA : SPELL_POISON_NOVA_H))
                    {
                        DoScriptText(SAY_ORDER_BLOW, m_creature);
                        m_uiPoisonNovaTimer = 35000;
                    }
                }else m_uiPoisonNovaTimer -= uiDiff;
                
                if(m_uiPursueTimer < uiDiff)
                {
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_PURSUED) == CAST_OK)
                        {
                            if(pTarget->HasAura(SPELL_CONFUSION))
                            {
                                pTarget->RemoveAurasDueToSpell(SPELL_CONFUSION);
                                m_creature->CastSpell(m_creature, SPELL_CONFUSION, true);
                                DoScriptText(EMOTE_ICK_CHASING, m_creature, pTarget);
                                switch (urand(0, 2))
                                {
                                    case 0: DoScriptText(SAY_TARGET_1, m_creature); break;
                                    case 1: DoScriptText(SAY_TARGET_2, m_creature); break;
                                    case 2: DoScriptText(SAY_TARGET_3, m_creature); break;
                                }
                                //m_bConfusion = true;
                            }
                        }
                    }
                    m_uiPursueTimer = 45000;
                }else m_uiPursueTimer -= uiDiff;

                //Hack!!! Delete when spell work. 
                /*if(m_bConfusion)
                {
                    if(m_uiConfusionTimer < uiDiff)
                    {
                        m_creature->CastSpell(m_creature, SPELL_CONFUSION, true);
                        Confusion();
                        m_bConfusion = false;
                        m_uiConfusionTimer = 5000;
                    }else m_uiConfusionTimer -= uiDiff;
                }*/
                    
                if(m_uiMightKickTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_KICK) == CAST_OK)
                        m_uiMightKickTimer = 25000;
                }else m_uiMightKickTimer -= uiDiff;

                if(m_uiExplosivBarrageTimer < uiDiff)
                {
                    m_creature->CastSpell(m_creature, SPELL_ROOT_SELF, true);
                    m_uiPhase = PHASE_BARRAGE;
                    m_uiExplosivBarrageTimer = 45000; //45sec
                }else m_uiExplosivBarrageTimer -= uiDiff;
                
                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_BARRAGE:
            {
                if(m_uiSummonOverTimer < uiDiff)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_ROOT_SELF);
                    m_uiPhase = PHASE_START;
                    m_uiSummonOverTimer = 18000;
                }else m_uiSummonOverTimer -= uiDiff;

                break;
            }
        }
    }
};

struct MANGOS_DLL_DECL boss_krickAI : public ScriptedAI
{
    boss_krickAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsOutro;
    bool m_bIsSummoning;

    uint8 m_uiPhase;
    uint32 m_uiToxicWasteTimer;
    uint32 m_uiShadowboltTimer;
    uint32 m_uiExplosivBarrageTimer;
    // workaround
    uint32 m_uiSummonOrbsTimer;
    uint32 m_uiSummonOverTimer;


    uint32 m_uiEventStepTimer;
    uint8 m_uiEventStep;

    void Reset()
    {
        m_uiToxicWasteTimer         = 3000;
        m_uiShadowboltTimer         = 15000;
        m_uiExplosivBarrageTimer    = 25000; //35
        m_uiSummonOrbsTimer         = 600000;
        m_uiSummonOverTimer         = 600000;

        m_bIsOutro = false;
        m_uiEventStepTimer = 1000;
        m_uiEventStep = 1;

        m_uiPhase = PHASE_START;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        SetCombatMovement(true);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_KRICK, DONE);

            m_bIsOutro = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_bIsOutro)
        {
            if(m_uiEventStepTimer < uiDiff)
            {
                switch(m_uiEventStep)
                {
                    case 1:
                        if(Creature *pJaina = m_creature->SummonCreature(NPC_JAINA_PART_1, 750.6790f, 116.121f, 512.3890f, 0.06377f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        { 
                            pJaina->GetMotionMaster()->MovePoint(0, 825.6246f, 116.474f, 509.4035f);
                            pJaina->GetCharmInfo()->SetReactState(REACT_PASSIVE);
                            //m_creature->GetMotionMaster()->MovePoint(0, 439.0362f, 204.2187f, 528.7357f);
                        }
                        else if(Creature *pSylvanas = m_creature->SummonCreature(NPC_SYLVANAS_PART_1, 750.6790f, 116.121f, 512.3890f, 0.06377f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            pSylvanas->GetMotionMaster()->MovePoint(0, 825.6246f, 116.474f, 509.4035f);
                            pSylvanas->GetCharmInfo()->SetReactState(REACT_PASSIVE);
                            //m_creature->GetMotionMaster()->MovePoint(0, 439.0362f, 204.2187f, 528.7357f);
                        }
                        m_creature->MonsterYell("1", 0);
                        m_uiEventStep = 2;
                        m_uiEventStepTimer = 8000;
                        break;
                    case 2:
                        //DoScriptText(SAY_KRICK_1, m_creature);
                        m_creature->MonsterYell("2", 0);
                        m_uiEventStep = 3;
                        m_uiEventStepTimer = 5000;
                        break;
                    case 3:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {
                            //DoScriptText(SAY_JAINA_1, pJaina);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_1)
                        {
                            //DoScriptText(SAY_SYLVANAS_1, pSylvanas);
                        }
                        m_creature->MonsterYell("3", 0);
                        m_uiEventStep = 4;
                        m_uiEventStepTimer = 5000;
                        break;
                    case 4:
                        //DoScriptText(SAY_KRICK_2, m_creature);
                        m_creature->MonsterYell("4", 0);
                        m_uiEventStep = 5;
                        m_uiEventStepTimer = 5000;
                        break;
                    case 5:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {
                            //DoScriptText(SAY_JAINA_2, pJaina);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_1)
                        {
                            DoScriptText(SAY_SYLVANAS_2, m_creature);
                        }
                        m_creature->MonsterYell("5", 0);
                        m_uiEventStep = 6;
                        m_uiEventStepTimer = 5000;
                        break;
                    case 6:
                        DoScriptText(SAY_KRICK_3, m_creature);
                        if(Creature *pTyranus = m_creature->SummonCreature(NPC_TYRANNUS_INTRO, 841.0100f, 196.2450f, 573.9640f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            pTyranus->GetMotionMaster()->MovePoint(0, 840.8530f, 179.628f, 554.0090f);
                        }
                        m_uiEventStep = 7;
                        m_uiEventStepTimer = 10000;
                        break;
                    case 7:
                        if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            DoScriptText(SAY_TYRANNUS_1, pTyrannus);
                        }
                        m_uiEventStep = 8;
                        m_uiEventStepTimer = 6000;
                        break;
                    case 8:
                        DoScriptText(SAY_KRICK_4, m_creature);
                        m_uiEventStep = 9;
                        m_uiEventStepTimer = 2000;
                        break;
                    case 9:
                        //kil krick
                        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                        m_creature->SetHealth(0);
                        if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            DoScriptText(SAY_TYRANNUS_2, pTyrannus);
                        }
                        m_uiEventStep = 10;
                        m_uiEventStepTimer = 10000;
                        break;
                    case 10:
                        //when krick died
                        if (m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {
                            DoScriptText(SAY_JAINA_3, m_creature);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_1)
                        {
                            DoScriptText(SAY_SYLVANAS_3, m_creature);
                        }
                        m_uiEventStep = 11;
                        m_uiEventStepTimer = 6000;
                        break;
                    case 11:
                        if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            DoScriptText(SAY_TYRANNUS_3, pTyrannus);
                        }
                        m_uiEventStep = 12;
                        m_uiEventStepTimer = 6000;
                        break;
                    case 12:
                        if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            DoScriptText(SAY_TYRANNUS_4, pTyrannus);
                        }
                        m_bIsOutro = false;
                        break;
                    default:
                        break;
                }
            }else m_uiEventStepTimer -= uiDiff;
        }

        switch(m_uiPhase)
        {
            case PHASE_START:
            {
                if(m_uiToxicWasteTimer < uiDiff)
                {
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        m_creature->CastSpell(pTarget, m_bIsRegularMode ? SPELL_TOXIC_WASTE : SPELL_TOXIC_WASTE_H, false);
                        m_uiToxicWasteTimer = 10000;
                    }
                }else m_uiToxicWasteTimer -= uiDiff;
                 
                if(m_uiShadowboltTimer < uiDiff)
                {
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        m_creature->CastSpell(pTarget, SPELL_SHADOW_BOLT, false);
                        m_uiShadowboltTimer = 5000;
                    }
                }else m_uiShadowboltTimer -= uiDiff;

                if(m_uiExplosivBarrageTimer < uiDiff)
                {
                    DoScriptText(SAY_ORDER_STOP, m_creature);
                    DoScriptText(EMOTE_KRICK_MINES, m_creature);

                    m_bIsSummoning = true;
                    m_uiSummonOrbsTimer = 1000;
                    m_uiSummonOverTimer = 18000;
                    m_uiPhase = PHASE_BARRAGE;
                    m_uiExplosivBarrageTimer = 45000; //45sec
                }else m_uiExplosivBarrageTimer -= uiDiff;

                break;
            }
            case PHASE_BARRAGE:
            {
                if(m_bIsSummoning)
                {
                    if(m_uiSummonOrbsTimer < uiDiff) 
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            m_creature->CastSpell(pTarget, SPELL_EXPLOSIVE_LIGHT, true);
                            m_uiSummonOrbsTimer = 600;
                        }
                    }else m_uiSummonOrbsTimer -= uiDiff;
                }
                
                if(m_uiSummonOverTimer < uiDiff)
                {
                    m_bIsSummoning = false;
                    m_uiPhase = PHASE_START;
                    m_uiSummonOverTimer = 60000;
                }else m_uiSummonOverTimer -= uiDiff;

                break;
            }
        }
    }
};

struct MANGOS_DLL_DECL mob_exploding_orbAI : public ScriptedAI
{
   mob_exploding_orbAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
   }

   ScriptedInstance* pInstance;
   uint32 m_uiExplodeTimer;
   uint32 m_uiScaleTimer;

    void Reset()
    {
        m_uiExplodeTimer = 3000;
        m_uiScaleTimer = 2000;
        DoCastSpellIfCan(m_creature, SPELL_EXPLOSIVE_VISUAL);
        SetCombatMovement(false);
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->SetObjectScale(1.5f);
    }

    void AttackStart(Unit* who)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!pInstance) return;

        if(m_uiScaleTimer < uiDiff)
        {
            m_creature->SetObjectScale(2.5f);
            m_uiScaleTimer = 2000;
        }else m_uiScaleTimer -= uiDiff;

        if(m_uiExplodeTimer < uiDiff)
        {
           m_creature->CastSpell(m_creature, SPELL_EXPLOSIVE_BARRAGE_ORB, true);
           m_creature->ForcedDespawn();
           m_uiExplodeTimer = 3000;
        }else m_uiExplodeTimer -= uiDiff;
    }

};

bool GossipHello_boss_krick(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());

    if(pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu( pCreature->GetObjectGuid());

    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, "We are ready!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ARTHAS_1, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_boss_krick(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (!pInstance) return false;

    pPlayer->CLOSE_GOSSIP_MENU();
    ((boss_krickAI*)pCreature->AI())->m_bIsOutro = true;
    //pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    //pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    return true;
}

CreatureAI* GetAI_boss_ick(Creature* pCreature)
{
    return new boss_ickAI (pCreature);
}

CreatureAI* GetAI_boss_krick(Creature* pCreature)
{
    return new boss_krickAI (pCreature);
}

CreatureAI* GetAI_mob_exploding_orb(Creature* pCreature)
{
    return new mob_exploding_orbAI(pCreature);
}

void AddSC_boss_krick_and_ick()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_krick";
    newscript->GetAI = &GetAI_boss_krick;
    newscript->pGossipHello = &GossipHello_boss_krick;
    newscript->pGossipSelect = &GossipSelect_boss_krick;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_ick";
    newscript->GetAI = &GetAI_boss_ick;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_exploding_orb";
    newscript->GetAI = &GetAI_mob_exploding_orb;
    newscript->RegisterSelf();

}

/*
                if (phase == PHASE_OUTRO)
+                {
+                    events.Update(diff);
+                    switch(events.ExecuteEvent())
+                    {
+                        case EVENT_OUTRO_1:
+                        {
+                            DoScriptText(SAY_KRICK_OUTRO_1, me);
+                            events.ScheduleEvent(EVENT_OUTRO_2, 14000);
+                            break;
+                        }
+                        case EVENT_OUTRO_2:
+                        {
+                            if (Creature* pNpcDialog = Unit::GetCreature(*me, uiNpcOutroDialog))
+                            {
+                                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == TEAM_ALLIANCE)
+                                    DoScriptText(SAY_JAYNA_OUTRO_2, pNpcDialog);
+                                else
+                                    DoScriptText(SAY_SYLVANAS_OUTRO_2, pNpcDialog);
+                            }
+                            events.ScheduleEvent(EVENT_OUTRO_3, 8500);
+                            break;
+                        }
+                        case EVENT_OUTRO_3:
+                            DoScriptText(SAY_KRICK_OUTRO_3, me);
+                            events.ScheduleEvent(EVENT_OUTRO_4, 12000);
+                            break;
+                        case EVENT_OUTRO_4:
+                        {
+                            if (Creature* pNpcDialog = Unit::GetCreature(*me, uiNpcOutroDialog))
+                            {
+                                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == TEAM_ALLIANCE)
+                                    DoScriptText(SAY_JAYNA_OUTRO_4, pNpcDialog);
+                                else
+                                    DoScriptText(SAY_SYLVANAS_OUTRO_4, pNpcDialog);
+                            }
+                            events.ScheduleEvent(EVENT_OUTRO_5, 8000);
+                            break;
+                        }
+                        case EVENT_OUTRO_5:
+                            DoScriptText(SAY_KRICK_OUTRO_5, me);
+                            events.ScheduleEvent(EVENT_OUTRO_6, 4000);
+                            break;
+                        case EVENT_OUTRO_6:
+                            // store uiTyrannus
+                            if (Creature *tyrannus = me->SummonCreature(CREATURE_TYRANNUS, spawnPointTyrannus, TEMPSUMMON_MANUAL_DESPAWN))
+                            {
+                                uiTyrannus = tyrannus->GetGUID();
+                                tyrannus->GetMotionMaster()->MovePoint(0, movePointTyrannus);
+                            }
+                            events.ScheduleEvent(EVENT_OUTRO_7, 1);
+                            break;
+                        case EVENT_OUTRO_7:
+                            if (Creature *pTyrannus = Unit::GetCreature(*me, uiTyrannus))
+                                DoScriptText(SAY_TYRANNUS_OUTRO_7, pTyrannus);
+                            events.ScheduleEvent(EVENT_OUTRO_8, 7000);
+                            break;
+                        case EVENT_OUTRO_8:
+                            DoScriptText(SAY_KRICK_OUTRO_8, me);
+                            DoCast(SPELL_STRANGULATE);
+                            me->AddUnitMovementFlag(MOVEMENTFLAG_LEVITATING);
+                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5);
+                            events.ScheduleEvent(EVENT_OUTRO_9, 4000);
+                            break;
+                        case EVENT_OUTRO_9:
+                            // tyrannus kills krick
+                            me->Kill(me);
+                            if (Creature *pTyrannus = Unit::GetCreature(*me, uiTyrannus))
+                                DoScriptText(SAY_TYRANNUS_OUTRO_9, pTyrannus);
+                            events.ScheduleEvent(EVENT_OUTRO_10, 12000);
+                            break;
+                        case EVENT_OUTRO_10:
+                        {
+                            if (Creature* pNpcDialog = Unit::GetCreature(*me, uiNpcOutroDialog))
+                            {
+                                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == TEAM_ALLIANCE)
+                                    DoScriptText(SAY_JAYNA_OUTRO_10, pNpcDialog);
+                                else
+                                    DoScriptText(SAY_SYLVANAS_OUTRO_10, pNpcDialog);
+                            }
+
+                            // End of OUTRO. for now...
+                            events.ScheduleEvent(EVENT_OUTRO_END, 8000);
+                            break;
+                        }
+                        case EVENT_OUTRO_END:
+                        {
+                            // SHOULD NOT DISSAPEAR!
+                            // This should trigger a follow up event in the instance script
+                            if (Creature* pNpcDialog = Unit::GetCreature(*me, uiNpcOutroDialog))
+                                pNpcDialog->DisappearAndDie();
+                            break;
+                        }
+                    }
+                    return;
*/