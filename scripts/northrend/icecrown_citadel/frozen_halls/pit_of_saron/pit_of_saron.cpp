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
SDName: pit_of_saron
SD%Complete:
SDAuthor: Bearq
SDComment: Raise Dead dont have visual effect. We must talk with Jaina/Sylvanas to start event. 
Its good solution to test event. Later delete this and start event when players enter to instance.
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{   
    //Intro
    SAY_TYRANNUS_INTRO_1                      = -1658070,    
    SAY_TYRANNUS_INTRO_2                      = -1658071,   
    //attack guards
    SAY_JAINA_INTRO_1                         = -1658072,    
    SAY_TYRANNUS_INTRO_3                      = -1658073,    
    //Jak dostaja swiatlem
    SAY_JAINA_INTRO_2                         = -1658074,    
    //zmiana w szkielety
    SAY_TYRANNUS_INTRO_4                      = -1658075, 
    //podczas blizzarda
    SAY_JAINA_INTRO_3                         = -1658076, 
    //gdy szkielety nie zyja
    SAY_JAINA_INTRO_4                         = -1658077, 
    SAY_JAINA_INTRO_5                         = -1658078, 
    //SAY_SYLVANAS_INTRO_1                      = -1658079, You will have to battle your way throught this
    SAY_SYLVANAS_INTRO_2                      = -1658080, 
    SAY_SYLVANAS_INTRO_3                      = -1658081,

   /***************************/

    /** TUNEL RIMENFANG **/
    SAY_GAUNTLET                               = -1658052,

    /** INTRO TYRANNUS **/
    SAY_TYRANUS_TUNEL_1                        = -1658053,
    //SAY_MOBS                                 = brak
    SAY_TYRANNUS_TUNEL_2                       = -1658054,

    /** OUTRO TYRANNUS **/
    SAY_MOB_OUTRO_1                            = -1658063,
    SAY_MOB_OUTRO_2                            = -1658064,
    SAY_JAINA_OUTRO_1                          = -1658065, //when jaina
    SAY_SYLVANAS_OUTRO_1                       = -1658066, //when sylvanas
    SAY_JAINA_OUTRO_2                          = -1658067, //or down
    SAY_JAINA_OUTRO_3                          = -1658069, //or up
    SAY_SYLVANAS_OUTRO_2                       = -1658068, 

    
    /** SPELL AND EFFECTS **/
    SPELL_LANCE_VOLLEY                         = 70464,      
    SPELL_BLIZZARD_FREZE                       = 70130, //Spell freez target
    SPELL_BLIZZARD                             = 50715, //Efect and shards
    SPELL_TELEPORT                             = 70527, //When jaina teleport to tunel
    SPELL_TELEPORT_VISUAL                      = 70623, //Efekt ring in floor
    SPELL_VOLLEY                               = 59515, //Or 70513
    SPELL_SHRIEK                               = 59514, //Or 70512
    SPELL_RAISE_DEAD                           = 69350,
    SPELL_STRANGULATING                        = 69413, //Only visual effect. Must use movepoint
    SPELL_ICE_BLOCK                            = 68252,
    SPELL_NECROMANTIC                          = 69347, //Effect light
    SPELL_SHARDS                               = 69425, //dmg and sound
    SPELL_ICYCLE                               = 62234, //Effekt spadajacego sniegu + okrag na ziemi i spadajcy kawalek lodu
    NPC_ICYCLE                                 = 36847, //Ten npc ma kastowac spell icycle. Po X czasie ma wykastowac spell shards
    NPC_FROST_BOMB                             = 37186,
    NPC_BARRIER                                = 24110,
    SPELL_BARIER_VISUAL                        = 70573,

    DISPLAY_SKELETON                           = 9785,

};

static float afSpawnLocations[20][3]=
{
    //Move point guards
    {498.3471f, 230.7642f, 526.2070f},   //Or 0.4099
    {509.6598f, 200.2581f, 528.7103f},   //Or 6.0137
    {1011.6488f, 171.2529f, 628.1567f},  //Movepoint main npc Victis or. 5.3210
    {441.2067f, 213.4815f, 528.7100f},   //Move Point skeleton
    {1007.4609f, 166.0320f, 628.1561f},  //Spawn bomb
    {888.8536f, 122.6058f, 554.7172f},   //Spawn sindragosy 
    {888.8536f, 122.6058f, 651.7172f},   //Point sindragosa or. 0.3308
    {719.6231f, -52.3803f, 801.14758f},  //First Movepoint Sindragosa
    {1065.0568f, 96.9975f, 630.9969f},   //Teleport jainy in tunel or 2.0475f
    {1058.1867f, 115.1969f, 628.2355f},  //First movepoint Jaina/Sylvana when go to platform
    {1048.0514f, 149.6024f, 628.1561f},  //Second movepoint Jaina/Sylvana or 2.8077
    {1045.9538f, 185.8750f, 628.1561f},  //Three movepoint Jaina/Sylvana
    {1101.2410f, 238.2556f, 629.4762f},  //Next to door Jaina/Sylvana or. 0.8913
};

struct MANGOS_DLL_DECL npc_jaina_or_sylvanas_POSintroAI : public ScriptedAI
{
    npc_jaina_or_sylvanas_POSintroAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsAlliance;
    bool m_bIsIntro;

    GUIDList m_lGuards;
    ObjectGuid m_uiTyranusGuid;
    ObjectGuid m_uiSkeletonGuid;

    uint8 m_uiEventStep;
    uint32 m_uiEventStepTimer;

    void Reset()
    {
        m_uiEventStep = 1;
        m_uiEventStepTimer = 5000;

        m_bIsIntro = false;
        SetCombatMovement(false);

        m_uiTyranusGuid.Clear();
        m_uiSkeletonGuid.Clear();
        m_lGuards.clear();

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        pSummoned->ForcedDespawn();
    }

    void DoSummonAllianceEventNpcs()
    {
        //Left Group
        for (uint32 i = 0; i < 3; ++i)
        {
            if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_1_ALLIANCE, 444.2836f-+urand(3.0f, 8.0f), 234.7212f-+urand(3.0f, 8.0f), 528.7091f, 0.0146f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                m_lGuards.push_back(pTemp->GetObjectGuid());

            if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_2_ALLIANCE, 444.2836f-+urand(3.0f, 8.0f), 234.7212f-+urand(3.0f, 8.0f), 528.7091f, 0.0146f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                m_lGuards.push_back(pTemp->GetObjectGuid());

            if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_3_ALLIANCE, 444.2836f-+urand(3.0f, 8.0f), 234.7212f-+urand(3.0f, 8.0f), 528.7091f, 0.0146f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                m_lGuards.push_back(pTemp->GetObjectGuid());

        }

        //Right Group
        for (uint32 i = 0; i < 3; ++i)
        {
            if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_1_ALLIANCE, 443.9927f-+urand(3.0f, 8.0f), 201.5524f-+urand(3.0f, 8.0f), 530.7065f, 0.0146f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                m_lGuards.push_back(pTemp->GetObjectGuid());

            if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_2_ALLIANCE, 443.9927f-+urand(3.0f, 8.0f), 201.5524f-+urand(3.0f, 8.0f), 530.7065f, 0.0146f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                m_lGuards.push_back(pTemp->GetObjectGuid());

            if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_3_ALLIANCE, 443.9927f-+urand(3.0f, 8.0f), 201.5524f+urand(3.0f, 8.0f), 530.7065f, 0.0146f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                m_lGuards.push_back(pTemp->GetObjectGuid());
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bIsIntro)
        {
            if(m_uiEventStepTimer < uiDiff)
            {
                switch(m_uiEventStep)
                {
                    case 1:
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        { 
                            DoScriptText(SAY_TYRANNUS_INTRO_1, pTyranus);
                        }
                        DoSummonAllianceEventNpcs();
                        m_uiEventStepTimer = 10000;
                        m_uiEventStep = 2;
                        break;
                    case 2:
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            DoScriptText(SAY_TYRANNUS_INTRO_2, pTyranus);
                        }
                        m_uiEventStepTimer = 5000;
                        m_uiEventStep = 3;
                        break;
                    case 3:
                        m_uiEventStepTimer = 2000;
                        m_uiEventStep = 4;
                        break;
                    case 4:
                        //Guards go to Tyranus
                        for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                        {
                            if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                            {
                                pTmp->SetWalk(false);
                                switch(urand(0,1))
                                {
                                    case 0:
                                        pTmp->GetMotionMaster()->MovePoint(0, 498.3471f-+urand(5.0f, 8.0f), 230.7642f-+urand(5.0f, 8.0f), 526.2070f);
                                        break;
                                    case 1:
                                        pTmp->GetMotionMaster()->MovePoint(0, 510.5871f-+urand(5.0f, 8.0f), 196.7350f-+urand(5.0f, 8.0f), 528.7084f);
                                        break;
                                }
                            }
                        }
                        if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {
                            DoScriptText(SAY_JAINA_INTRO_1, m_creature);
                        }
                        m_uiEventStepTimer = 5000;
                        m_uiEventStep = 5;
                        break;
                    case 5:
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            DoScriptText(SAY_TYRANNUS_INTRO_3, pTyranus);
                        }
                        m_uiEventStepTimer = 5000;
                        m_uiEventStep = 6;
                        break;
                    case 6:
                        //Guards up to air
                        for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                        {
                            if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                            {
                                pTmp->SetLevitate(true);
                                pTmp->SetSpeedRate(MOVE_FLIGHT, 1.0f);
                                pTmp->SetWalk(true);
                                pTmp->GetMotionMaster()->MovePoint(0, pTmp->GetPositionX(), pTmp->GetPositionY(), 536.2070f);
                                pTmp->CastSpell(m_creature, SPELL_STRANGULATING, true);
                            }
                        }
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 7; 
                        break;
                    case 7:
                        m_uiEventStepTimer = 5000;
                        m_uiEventStep = 8;
                        break;
                    case 8:
                        //Tyranus use lights in guards.
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        { 
                            for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                            {
                                if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                                {  
                                    pTyranus->CastSpell(pTmp, SPELL_NECROMANTIC, false);
                                    pTmp->RemoveAurasDueToSpell(SPELL_STRANGULATING);
                                    pTmp->SetLevitate(false); 
                                    pTmp->SetWalk(false);
                                    float z = m_creature->GetTerrain()->GetHeight(pTmp->GetPositionX(), pTmp->GetPositionY(), MAX_HEIGHT);
                                    pTmp->GetMotionMaster()->MovePoint(0, pTmp->GetPositionX(), pTmp->GetPositionY(), z);
                                    pTmp->SetStandState(UNIT_STAND_STATE_SLEEP);
                                    pTmp->SetStandState(UNIT_STAND_STATE_DEAD);
                                    pTmp->SetDisplayId(DISPLAY_SKELETON);
                                    pTmp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    pTmp->setFaction(14);
                                }
                            }
                            if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                            {  
                                DoScriptText(SAY_JAINA_INTRO_2, m_creature);
                            }
                        }
                        m_uiEventStepTimer = 8000;
                        m_uiEventStep = 9; 
                        break;
                    case 9:
                        m_uiEventStepTimer = 5000;
                        m_uiEventStep = 10; 
                        break;
                    case 10:
                        //Change guards to skeleton
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                            {
                                if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                                {
                                    pTmp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    pTmp->CastSpell(pTmp, SPELL_RAISE_DEAD, true);
                                    pTmp->SetStandState(UNIT_STAND_STATE_STAND);
                                }
                            }
                            DoScriptText(SAY_TYRANNUS_INTRO_4, pTyranus);
                        }
                        m_uiEventStepTimer = 10000;
                        m_uiEventStep = 11; 
                        break;
                   case 11:
                        m_uiEventStepTimer = 2000;
                        m_uiEventStep = 12;  
                        break;
                    case 12:
                        //Skeleton attack Jaina or Sylvanas
                        for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                        {
                            if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                            {
                                pTmp->SetWalk(false);
                                pTmp->GetMotionMaster()->MovePoint(0, 441.5120f, 213.0944f, 528.7105f);
                            }
                        }
                        if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {  
                            DoScriptText(SAY_JAINA_INTRO_3, m_creature);
                        }  
                        m_uiEventStepTimer = 2000;
                        m_uiEventStep = 13;  
                        break;
                   case 13:
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                            {
                                if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                                { 
                                    if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                                    {
                                        m_creature->CastSpell(pTmp, SPELL_BLIZZARD, false);
                                        m_creature->CastSpell(pTmp, SPELL_BLIZZARD_FREZE, true);
                                    }
                                    else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_1)
                                    {
                                        m_creature->CastSpell(m_creature, SPELL_SHRIEK, true);
                                        m_creature->CastSpell(pTmp, SPELL_VOLLEY, true);
                                    }
                                }
                            }
                            pTyranus->SetSpeedRate(MOVE_FLIGHT, 2.5f);
                            pTyranus->GetMotionMaster()->MovePoint(0, 738.1108f, 258.7723f, 575.3183f);
                        }
                        m_uiEventStepTimer = 7000;
                        m_uiEventStep = 14;
                        break;
                    case 14:
                        for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                        {
                            if (Creature *pTmp = m_creature->GetMap()->GetCreature(*i))
                            { 
                                if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                                {
                                    m_creature->CastSpell(pTmp, SPELL_LANCE_VOLLEY, true); 
                                }
                            }
                        }
                        m_uiEventStepTimer = 4000;
                        m_uiEventStep = 15;
                        break;
                    case 15:
                        if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {
                            DoScriptText(SAY_JAINA_INTRO_4, m_creature);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_1)
                        {
                            //DoScriptText(SAY_SYLVANAS_INTRO_1, m_creature);
                        }
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 16;
                        break;
                    case 16:
                        if(m_creature->GetEntry() == NPC_JAINA_PART_1)
                        {   
                            DoScriptText(SAY_JAINA_INTRO_5, m_creature);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_1)
                        {
                            DoScriptText(SAY_SYLVANAS_INTRO_2, m_creature);
                        }
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 17;
                        break;
                    case 17:
                        if (Creature* pTyranus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                        {
                            pTyranus->ForcedDespawn();
                        }
                        m_bIsIntro = false;
                        break;
                    default:
                        break;
                } 
            }else m_uiEventStepTimer -= uiDiff;
        }
    }   
};

bool GossipHello_npc_jaina_or_sylvanas_POSintro(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());

    if(pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu( pCreature->GetObjectGuid());

     pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, "Intro!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
     pPlayer->SEND_GOSSIP_MENU(100001, pCreature->GetObjectGuid());


     return true;
}

bool GossipSelect_npc_jaina_or_sylvanas_POSintro(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (!pInstance) return false;

    pPlayer->CLOSE_GOSSIP_MENU();
    ((npc_jaina_or_sylvanas_POSintroAI*)pCreature->AI())->m_bIsIntro = true;

    //pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    //pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    return true;
}

struct MANGOS_DLL_DECL npc_jaina_or_sylvanas_POSoutroAI : public ScriptedAI
{
    npc_jaina_or_sylvanas_POSoutroAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
         m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
         Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHorde;
    bool m_bIsOutro;

    ObjectGuid m_uiSindragosaGuid;
    ObjectGuid m_uiBombGuid;
    ObjectGuid m_uiMageGuid;
    ObjectGuid m_uiBarierGuid;
    ObjectGuid m_guidNpc;

    GUIDList m_lGuards;
    uint8 m_uiEventStep;
    uint32 m_uiEventStepTimer;

    void Reset()
    {
        m_uiEventStepTimer = 5000;
        m_uiEventStep = 1;

        m_uiSindragosaGuid.Clear();
        m_uiBombGuid.Clear();
        m_uiMageGuid.Clear();
        m_uiBarierGuid.Clear();
        m_guidNpc.Clear();
        m_lGuards.clear();

        m_bIsOutro = true;

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    }

    //Teleport Players when Sindragosa drop Frost Bomb
    void TeleportPlayers()
    {
        Map* pMap = m_creature->GetMap();
        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();
            if (!PlayerList.isEmpty())

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive())
                {
                    float x = m_creature->GetPositionX()-+urand(1.0f, 5.0f);
                    float y = m_creature->GetPositionY()-+urand(3.0f, 6.0f);

                    DoTeleportPlayer(i->getSource(), x, y, m_creature->GetPositionZ(), m_creature->GetOrientation());
                }
            }
        }
    }
    
    void DoSummonEventNpcs()
    {
        // main npc
        if (Creature *pTmp = m_creature->SummonCreature(NPC_MARTIN_VICTUS_END, 1065.0568f, 96.9975f, 630.9969f, 2.0475f, TEMPSUMMON_MANUAL_DESPAWN, 0))
        {
            pTmp->SetWalk(false);
            pTmp->GetMotionMaster()->MovePoint(1011.6488f, 171.2529f, 628.1567f, 5.3210f);     
            pTmp->setFaction(m_creature->getFaction());
            m_guidNpc = pTmp->GetObjectGuid();
        }

        // 2 adds First group
        if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_1_ALLIANCE, 1051.1462f, 152.1018f, 628.0000f, 2.5910f, TEMPSUMMON_MANUAL_DESPAWN, 0))
        {
            pTemp->SetWalk(false);
            pTemp->GetMotionMaster()->MovePoint(1051.1462f, 152.1018f, 628.0000f, 2.5910f);     
            pTemp->setFaction(m_creature->getFaction());
            m_lGuards.push_back(pTemp->GetObjectGuid());
        }
        if (Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_3_ALLIANCE, 1044.0268f, 140.5048f, 628.0000f, 2.5910f, TEMPSUMMON_MANUAL_DESPAWN, 0))
        {
            pTemp->SetWalk(false);
            pTemp->GetMotionMaster()->MovePoint(1044.0268f, 140.5048f, 628.0000f, 2.5910f);  
            pTemp->setFaction(m_creature->getFaction());
            m_lGuards.push_back(pTemp->GetObjectGuid());
        }
        //Mage
        if (Creature *pMage = m_creature->SummonCreature(NPC_CHAMPION_2_ALLIANCE, 1038.0280f, 130.7332f, 628.0000f, 2.5910f, TEMPSUMMON_MANUAL_DESPAWN, 0))
        {
            pMage->SetWalk(false);
            pMage->GetMotionMaster()->MovePoint(1038.0280f, 130.7332f, 628.0000f, 2.5910f);       
            m_uiMageGuid = pMage->GetObjectGuid();
        }
    }

    Creature* GetMainNpc()
    {
        return m_creature->GetMap()->GetCreature(m_guidNpc);
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
                        /*if(Creature *pJaina = m_creature->SummonCreature(NPC_JAINA_PART_2, 1065.0568f, 96.9975f, 630.9969f, 2.0475f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            pJaina->SetWalk(false);
                            pJaina->GetMotionMaster()->MovePoint(0, 1007.4609f, 166.0320f, 628.1561f);
                            pJaina->SetOrientation(5.2621f);
                        }*/
                        m_creature->MonsterYell("1", 0);
                        m_creature->SetWalk(false);
                        m_creature->GetMotionMaster()->MovePoint(1007.4609f, 166.0320f, 628.1561f, 5.2621f);
                        DoSummonEventNpcs();
                        m_uiEventStepTimer = 8000;
                        m_uiEventStep = 2;
                        break;
                    case 2:
                        if (Creature *pTmp = GetMainNpc())
                        {
                            pTmp->SetOrientation(5.3210f);
                            DoScriptText(SAY_MOB_OUTRO_1, pTmp);
                        }
                        m_uiEventStepTimer = 20000;
                        m_uiEventStep = 3;
                        break;
                    case 3:
                        if (Creature *pTmp = GetMainNpc())
                        {
                            DoScriptText(SAY_MOB_OUTRO_2, pTmp);
                        }
                        if(Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, 898.8536f, 122.6058f, 554.7172f, 0.3308f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            m_uiSindragosaGuid = pSindragosa->GetObjectGuid();;
                            pSindragosa->SetLevitate(true);
                            pSindragosa->GetMotionMaster()->MovePoint(0, 898.8536f, 122.6058f, 651.1871f);
                        }
                        m_uiEventStepTimer = 10000;
                        m_uiEventStep = 4;
                        break;
                    case 4:
                        if(Creature* pBomb = m_creature->SummonCreature(NPC_FROST_BOMB, 1007.4609f, 166.0320f, 628.1561f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            m_uiBombGuid = pBomb->GetObjectGuid();
                        }
                        m_uiEventStepTimer = 2000;
                        m_uiEventStep = 5;
                        break;
                    case 5:
                        if(m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            DoScriptText(SAY_JAINA_OUTRO_1, m_creature);
                        }
                        else if(m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            DoScriptText(SAY_SYLVANAS_OUTRO_1, m_creature);
                        }
                        m_creature->SetWalk(false);
                        m_uiEventStepTimer = 1000;
                        m_uiEventStep = 6;
                        break;
                    case 6:
                        if(m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            if(Creature* pBarier = m_creature->SummonCreature(NPC_BARRIER, 1065.0568f, 86.9975f, 630.9969f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                            {
                                m_creature->CastSpell(m_creature, SPELL_TELEPORT_VISUAL, true);
                                m_uiBarierGuid = pBarier->GetObjectGuid();
                                pBarier->CastSpell(pBarier, SPELL_BARIER_VISUAL, true);
                            }
                        }
                        else if(m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            if(Creature* pBarier = m_creature->SummonCreature(NPC_BARRIER, 1065.0568f, 86.9975f, 630.9969f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                            {
                                m_creature->CastSpell(m_creature, SPELL_TELEPORT_VISUAL, true);
                                m_uiBarierGuid = pBarier->GetObjectGuid();
                                pBarier->CastSpell(pBarier, SPELL_BARIER_VISUAL, true);
                            }
                        }                       
                        m_uiEventStepTimer = 3000;
                        m_uiEventStep = 7; 
                        break;
                    case 7:
                        if (Creature* pBomb = m_creature->GetMap()->GetCreature(m_uiBombGuid))
                        {
                            if(m_creature->GetEntry() == NPC_JAINA_PART_2)
                            {
                                m_creature->NearTeleportTo(1065.0568f, 96.9975f, 630.9969f, 2.0475f);
                                TeleportPlayers();
                                m_creature->CastSpell(m_creature, SPELL_TELEPORT, true);
                            }
                            else if(m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                            {
                                m_creature->NearTeleportTo(1065.0568f, 96.9975f, 630.9969f, 0.0f);
                                TeleportPlayers();
                                m_creature->CastSpell(m_creature, SPELL_TELEPORT, true);
                            }  
                            pBomb->ForcedDespawn();
                        }
                        m_uiEventStepTimer = 1000;
                        m_uiEventStep = 8; 
                        break;
                    case 8:
                        if (Creature* pSindragosa = m_creature->GetMap()->GetCreature(m_uiSindragosaGuid))
                        {
                            pSindragosa->GetMotionMaster()->MovePoint(0, 719.6231f, -52.3803f, 801.14758f);
                        }
                        m_uiEventStepTimer = 4000;
                        m_uiEventStep = 9;
                        break;
                    case 9:
                        if (Creature *pTmp = GetMainNpc())
                        {
                            pTmp->SetStandState(UNIT_STAND_STATE_DEAD);
                        }
                        if (Creature* pMage = m_creature->GetMap()->GetCreature(m_uiMageGuid))
                        {
                            pMage->CastSpell(pMage, SPELL_ICE_BLOCK, true);
                        }
                        m_uiEventStepTimer = 4000;
                        m_uiEventStep = 10; 
                        break;
                    case 10:
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 11; 
                        break;
                    case 11:
                        if (Creature* pSindragosa = m_creature->GetMap()->GetCreature(m_uiSindragosaGuid))
                        {
                            if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                            {
                                m_creature->GetMotionMaster()->MovePoint(0, 1058.1867f, 115.1969f, 628.2355f);
                            }
                            else if(m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                            {
                                m_creature->GetMotionMaster()->MovePoint(0, 1058.1867f, 115.1969f, 628.2355f);
                            }
                            pSindragosa->ForcedDespawn();
                        }
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 12; 
                        break;
                   case 12:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            DoScriptText(SAY_JAINA_OUTRO_2, m_creature);
                        }
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 13; 
                        break;
                    case 13:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            m_creature->GetMotionMaster()->MovePoint(1048.0514f, 149.6024f, 628.1561f, 2.8077f);
                        }
                        else if(m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            m_creature->GetMotionMaster()->MovePoint(1048.0514f, 149.6024f, 628.1561f, 2.8077f);
                        }
                        m_uiEventStepTimer = 8000;
                        m_uiEventStep = 14;  
                        break;
                    case 14:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            DoScriptText(SAY_JAINA_OUTRO_3, m_creature);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            DoScriptText(SAY_SYLVANAS_OUTRO_2, m_creature);
                        }
                        m_uiEventStepTimer = 6000;
                        m_uiEventStep = 15;  
                        break;
                    case 15:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            m_creature->GetMotionMaster()->MovePoint(0, 1045.9538f, 185.8750f, 628.1561f, true);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            m_creature->GetMotionMaster()->MovePoint(0, 1045.9538f, 185.8750f, 628.1561f, true);
                        }
                        m_uiEventStepTimer = 4000;
                        m_uiEventStep = 16;  
                        break;
                    case 16:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            m_creature->GetMotionMaster()->MovePoint(0, 1101.2410f, 238.2556f, 629.4762f);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            m_creature->GetMotionMaster()->MovePoint(0, 1101.2410f, 238.2556f, 629.4762f);
                        }
                        m_uiEventStepTimer = 8000;
                        m_uiEventStep = 17;  
                        break;
                    case 17:
                        if (m_creature->GetEntry() == NPC_JAINA_PART_2)
                        {
                            if (m_pInstance)
                                m_pInstance->DoUseDoorOrButton(GO_HALLS_OF_REFLECT_PORT);
                        }
                        else if (m_creature->GetEntry() == NPC_SYLVANAS_PART_2)
                        {
                            if (m_pInstance)
                                m_pInstance->DoUseDoorOrButton(GO_HALLS_OF_REFLECT_PORT);
                        }
                        m_bIsOutro = false;
                        break;
                    default:
                        break;
                }
             }else m_uiEventStepTimer -= uiDiff;
         }   
     }    
};    

bool GossipHello_npc_jaina_or_sylvanas_POSoutro(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());

    //Game Master can Start, Stop, Reset Event. Must Spawn Jaina ID 38188
    if (pPlayer->isGameMaster())
    {
        if(pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu( pCreature->GetObjectGuid());

        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, "Start Outro!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(100001, pCreature->GetObjectGuid());

        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, "Stop Outro!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(100001, pCreature->GetObjectGuid());

        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, "Reset Outro!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
        pPlayer->SEND_GOSSIP_MENU(100001, pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_jaina_or_sylvanas_POSoutro(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_bIsOutro = true;
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_uiEventStepTimer = 2000;
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_uiEventStep = 1;
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_bIsOutro = false;
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_uiEventStepTimer = 200000;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_bIsOutro = true;
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_uiEventStepTimer = 2000;
            ((npc_jaina_or_sylvanas_POSoutroAI*)pCreature->AI())->m_uiEventStep = 1;
            break;
    }

    return true;
}


struct MANGOS_DLL_DECL mob_icycleAI : public ScriptedAI
{
    mob_icycleAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
         pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
         Reset();
    }

    ScriptedInstance* pInstance;
    uint32 m_uiIcycleTimer;
    uint32 m_uiIcycleDmgTimer;

    void Reset()
    {
        m_uiIcycleTimer = 1000;
        m_uiIcycleDmgTimer = 7000;
        SetCombatMovement(false);
        //m_creature->SetVisibility(VISIBILITY_OFF);

        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiIcycleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ICYCLE);
            m_uiIcycleTimer = 6000000;
        }else m_uiIcycleTimer -= uiDiff;

        if(m_uiIcycleDmgTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SHARDS);
            m_creature->ForcedDespawn();
            m_uiIcycleDmgTimer = 6000000;
        }else m_uiIcycleDmgTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_jaina_or_sylvanas_POSintro(Creature* pCreature)
{
    return new npc_jaina_or_sylvanas_POSintroAI(pCreature);
}

CreatureAI* GetAI_npc_jaina_or_sylvanas_POSoutro(Creature* pCreature)
{
    return new npc_jaina_or_sylvanas_POSoutroAI(pCreature);
}

CreatureAI* GetAI_mob_icycle(Creature* pCreature)
{
    return new mob_icycleAI(pCreature);
}

void AddSC_pit_of_saron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_jaina_or_sylvanas_POSintro";
    newscript->GetAI = &GetAI_npc_jaina_or_sylvanas_POSintro;
    newscript->pGossipHello = &GossipHello_npc_jaina_or_sylvanas_POSintro;
    newscript->pGossipSelect = &GossipSelect_npc_jaina_or_sylvanas_POSintro;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_jaina_or_sylvanas_POSoutro";
    newscript->GetAI = &GetAI_npc_jaina_or_sylvanas_POSoutro;
    newscript->pGossipHello = &GossipHello_npc_jaina_or_sylvanas_POSoutro;
    newscript->pGossipSelect = &GossipSelect_npc_jaina_or_sylvanas_POSoutro;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_icycle";
    newscript->GetAI = &GetAI_mob_icycle;
    newscript->RegisterSelf();
}

