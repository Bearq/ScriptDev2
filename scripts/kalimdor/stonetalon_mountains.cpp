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
SDName: Stonetalon_Mountains
SD%Complete: 95
SDComment: Quest support: 6627 (Braug Dimspirits questions/'answers' might have more to it, need more info),6523
SDCategory: Stonetalon Mountains
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_braug_dimspirit
######*/

bool GossipHello_npc_braug_dimspirit(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(6627) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ysera", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Neltharion", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nozdormu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alexstrasza", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Malygos", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        pPlayer->SEND_GOSSIP_MENU(5820, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(5819, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_braug_dimspirit(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer,6766,false);

    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(6627);
    }
    return true;
}

/*######
## npc_kaya
######*/

enum
{
    NPC_GRIMTOTEM_RUFFIAN       = 11910,
    NPC_GRIMTOTEM_BRUTE         = 11912,
    NPC_GRIMTOTEM_SORCERER      = 11913,

    SAY_START                   = -1000357,
    SAY_AMBUSH                  = -1000358,
    SAY_END                     = -1000359,

    QUEST_PROTECT_KAYA          = 6523
};

struct MANGOS_DLL_DECL npc_kayaAI : public npc_escortAI
{
    npc_kayaAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            //Ambush
            case 16:
                //note about event here:
                //apparently NPC say _after_ the ambush is over, and is most likely a bug at you-know-where.
                //we simplify this, and make say when the ambush actually start.
                DoScriptText(SAY_AMBUSH, m_creature);
                m_creature->SummonCreature(NPC_GRIMTOTEM_RUFFIAN, -50.75f, -500.77f, -46.13f, 0.4f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                m_creature->SummonCreature(NPC_GRIMTOTEM_BRUTE, -40.05f, -510.89f, -46.05f, 1.7f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                m_creature->SummonCreature(NPC_GRIMTOTEM_SORCERER, -32.21f, -499.20f, -45.35f, 2.8f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            // Award quest credit
            case 18:
                DoScriptText(SAY_END, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_PROTECT_KAYA, m_creature);
                break;
        }
    }
};

CreatureAI* GetAI_npc_kaya(Creature* pCreature)
{
    return new npc_kayaAI(pCreature);
}

bool QuestAccept_npc_kaya(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    //Casting Spell and Starting the Escort quest is buggy, so this is a hack. Use the spell when it is possible.

    if (pQuest->GetQuestId() == QUEST_PROTECT_KAYA)
    {
        pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);
        DoScriptText(SAY_START,pCreature);

        if (npc_kayaAI* pEscortAI = dynamic_cast<npc_kayaAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*#####
## npc_piznik
######*/
enum
{
    NPC_VERMIT                  = 3998,
    NPC_DIGER                   = 3999,
    NPC_TUNEL                   = 4001,
    NPC_STONECLAW               = 4002,
    NPC_GEOMANCER               = 4003,
    NPC_OVERLORD                = 4004,

    QUEST_GARENZOS_ORDERS       = 1090,

    MAX_MOB_COUNT                = 16,
};

struct MANGOS_DLL_DECL npc_piznikAI : public npc_escortAI
{
    npc_piznikAI(Creature* pCreature) : npc_escortAI(pCreature) 
    { 
        Reset(); 
    }

    bool m_bIsEvent;

    uint32 m_uiAddsCount;
    uint32 m_uiStepTimer;

    void Reset()
    {
        m_uiAddsCount = 0;
        m_uiStepTimer = 5000;

        m_bIsEvent = false;
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->GetMotionMaster()->MovePoint(0, 961.4401f-+urand(1.0f, 3.0f), -262.5406f-+urand(1.0f, 3.0f), -5.8356f);
        m_uiAddsCount += 1;
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        pSummoned->ForcedDespawn();
    }

    void WaypointReached(uint32)
    {}

    void SummonMobs()
    {
        for (uint8 i = 0; i < 2; ++i)
        {
            switch(urand(0, 5))
            {
                case 0:
                    m_creature->SummonCreature(NPC_VERMIT, 937.3007f-+urand(1.0f, 3.0f), -254.3103f-+urand(1.0f, 3.0f), -2.2186f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                case 1:
                    m_creature->SummonCreature(NPC_DIGER, 937.3007f-+urand(1.0f, 3.0f), -254.3103f-+urand(1.0f, 3.0f), -2.2186f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                case 2:
                    m_creature->SummonCreature(NPC_TUNEL, 937.3007f-+urand(1.0f, 3.0f), -254.3103f-+urand(1.0f, 3.0f), -2.2186f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                case 3:
                    m_creature->SummonCreature(NPC_STONECLAW, 937.3007f-+urand(1.0f, 3.0f), -254.3103f-+urand(1.0f, 3.0f), -2.2186f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                case 4:
                    m_creature->SummonCreature(NPC_GEOMANCER, 937.3007f-+urand(1.0f, 3.0f), -254.3103f-+urand(1.0f, 3.0f), -2.2186f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                case 5:
                    m_creature->SummonCreature(NPC_OVERLORD, 937.3007f-+urand(1.0f, 3.0f), -254.3103f-+urand(1.0f, 3.0f), -2.2186f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if(m_bIsEvent)
        {
            if(m_uiStepTimer < uiDiff)
            {
                SummonMobs();
                m_uiStepTimer = 15000;
            }else m_uiStepTimer -= uiDiff;

            if(m_uiAddsCount >= MAX_MOB_COUNT)
            {
                m_bIsEvent = false;
            }
        }

        DoMeleeAttackIfReady();
    }
};
    
CreatureAI* GetAI_npc_piznik(Creature* pCreature)
{
    return new npc_piznikAI(pCreature);
}

bool QuestAccept_npc_piznik(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_GARENZOS_ORDERS)
    {
        if (npc_piznikAI* pEscortAI = dynamic_cast<npc_piznikAI*>(pCreature->AI()))
            pEscortAI->m_bIsEvent = true;
    }

    return true;
}

/*######
## AddSC
######*/

void AddSC_stonetalon_mountains()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_braug_dimspirit";
    newscript->pGossipHello = &GossipHello_npc_braug_dimspirit;
    newscript->pGossipSelect = &GossipSelect_npc_braug_dimspirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kaya";
    newscript->GetAI = &GetAI_npc_kaya;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_kaya;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_piznik";
    newscript->GetAI = &GetAI_npc_piznik;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_piznik;
    newscript->RegisterSelf();
}
