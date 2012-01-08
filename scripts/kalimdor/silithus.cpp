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
SDName: Silithus
SD%Complete: 100
SDComment: Quest support: 7785, 8304.
SDCategory: Silithus
EndScriptData */

/* ContentData
npc_highlord_demitrian
npcs_rutgar_and_frankal
EndContentData */

#include "precompiled.h"

/*###
## npc_highlord_demitrian
###*/

#define GOSSIP_ITEM_DEMITRIAN1 "What do you know of it?"
#define GOSSIP_ITEM_DEMITRIAN2 "I am listening , Demitrian."
#define GOSSIP_ITEM_DEMITRIAN3 "Continue, please."
#define GOSSIP_ITEM_DEMITRIAN4 "A battle?"
#define GOSSIP_ITEM_DEMITRIAN5 "<Nod>"
#define GOSSIP_ITEM_DEMITRIAN6 "Caught unaware? How?"
#define GOSSIP_ITEM_DEMITRIAN7 "So what did Ragnaros do next?"

enum
{
    QUEST_EXAMINE_THE_VESSEL        =   7785,
    ITEM_BINDINGS_WINDSEEKER_LEFT   =   18563,
    ITEM_BINDINGS_WINDSEEKER_RIGHT  =   18564,
    ITEM_VESSEL_OF_REBIRTH          =   19016,
    GOSSIP_TEXTID_DEMITRIAN1        =   6842,
    GOSSIP_TEXTID_DEMITRIAN2        =   6843,
    GOSSIP_TEXTID_DEMITRIAN3        =   6844,
    GOSSIP_TEXTID_DEMITRIAN4        =   6867,
    GOSSIP_TEXTID_DEMITRIAN5        =   6868,
    GOSSIP_TEXTID_DEMITRIAN6        =   6869,
    GOSSIP_TEXTID_DEMITRIAN7        =   6870
};

bool GossipHello_npc_highlord_demitrian(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_EXAMINE_THE_VESSEL) == QUEST_STATUS_NONE &&
        (pPlayer->HasItemCount(ITEM_BINDINGS_WINDSEEKER_LEFT,1,false) || pPlayer->HasItemCount(ITEM_BINDINGS_WINDSEEKER_RIGHT,1,false)))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_highlord_demitrian(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN7, pCreature->GetObjectGuid());

            if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_VESSEL_OF_REBIRTH, 1))
                pPlayer->SendNewItem(pItem, 1, true, false);

            break;
    }
    return true;
}

/*###
## npcs_rutgar_and_frankal
###*/

//gossip item text best guess
#define GOSSIP_ITEM_SEEK1 "I seek information about Natalia"

#define GOSSIP_ITEM_RUTGAR2 "That sounds dangerous!"
#define GOSSIP_ITEM_RUTGAR3 "What did you do?"
#define GOSSIP_ITEM_RUTGAR4 "Who?"
#define GOSSIP_ITEM_RUTGAR5 "Women do that. What did she demand?"
#define GOSSIP_ITEM_RUTGAR6 "What do you mean?"
#define GOSSIP_ITEM_RUTGAR7 "What happened next?"

#define GOSSIP_ITEM_FRANKAL11 "Yes, please continue"
#define GOSSIP_ITEM_FRANKAL12 "What language?"
#define GOSSIP_ITEM_FRANKAL13 "The Priestess attacked you?!"
#define GOSSIP_ITEM_FRANKAL14 "I should ask the monkey about this"
#define GOSSIP_ITEM_FRANKAL15 "Then what..."

enum
{
    QUEST_DEAREST_NATALIA       =   8304,
    NPC_RUTGAR                  =   15170,
    NPC_FRANKAL                 =   15171,
    TRIGGER_RUTGAR              =   15222,
    TRIGGER_FRANKAL             =   15221,
    GOSSIP_TEXTID_RF            =   7754,
    GOSSIP_TEXTID_RUTGAR1       =   7755,
    GOSSIP_TEXTID_RUTGAR2       =   7756,
    GOSSIP_TEXTID_RUTGAR3       =   7757,
    GOSSIP_TEXTID_RUTGAR4       =   7758,
    GOSSIP_TEXTID_RUTGAR5       =   7759,
    GOSSIP_TEXTID_RUTGAR6       =   7760,
    GOSSIP_TEXTID_RUTGAR7       =   7761,
    GOSSIP_TEXTID_FRANKAL1      =   7762,
    GOSSIP_TEXTID_FRANKAL2      =   7763,
    GOSSIP_TEXTID_FRANKAL3      =   7764,
    GOSSIP_TEXTID_FRANKAL4      =   7765,
    GOSSIP_TEXTID_FRANKAL5      =   7766,
    GOSSIP_TEXTID_FRANKAL6      =   7767
};

bool GossipHello_npcs_rutgar_and_frankal(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_DEAREST_NATALIA) == QUEST_STATUS_INCOMPLETE &&
        pCreature->GetEntry() == NPC_RUTGAR &&
        !pPlayer->GetReqKillOrCastCurrentCount(QUEST_DEAREST_NATALIA, TRIGGER_RUTGAR))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SEEK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    if (pPlayer->GetQuestStatus(QUEST_DEAREST_NATALIA) == QUEST_STATUS_INCOMPLETE &&
        pCreature->GetEntry() == NPC_FRANKAL &&
        pPlayer->GetReqKillOrCastCurrentCount(QUEST_DEAREST_NATALIA, TRIGGER_RUTGAR))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SEEK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RF, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npcs_rutgar_and_frankal(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR7, pCreature->GetObjectGuid());
            //'kill' our trigger to update quest status
            pPlayer->KilledMonsterCredit(TRIGGER_RUTGAR, pCreature->GetObjectGuid());
            break;

        case GOSSIP_ACTION_INFO_DEF + 9:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL12, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 11:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL13, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL14, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL15, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL6, pCreature->GetObjectGuid());
            //'kill' our trigger to update quest status
            pPlayer->KilledMonsterCredit(TRIGGER_FRANKAL, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*******************
**Lesser Wind Stone
********************/
#define GOSSIP_ITEM_1 "I am not cultist, you monster! Come to me and face yout destruction!"
#define GOSSIP_ITEM_2 "You will listen to this, wille duke! I am not your Twillight's Hammer lapdog! I am here to challenge you! Come! Come, and meet your death..."
#define GOSSIP_ITEM_3 "The day of judgement has come, friend! I challenge you, no battle!"

#define GOSSIP_TEXT_1 "A faint whisper is heard form within the stone... Greetings, little one. Do you have news for the Templar?"
#define GOSSIP_TEXT_2 "A clear voice rises form within the stone...You come to us with litle and so we will listen, but do not try our patience. The council has no time for idle gossip from one such as you."
#define GOSSIP_TEXT_3 "A thunderous voice bellows form the stone... Greetings, commander. What news of Sillithus do you bring the Lords of the Council?"

enum
{
    //Lesser Wind Stone
    NPC_HOARY_TEMPLAR       = 15212,
    NPC_CRIMSON_TEMPLAR     = 15209,
    NPC_EARTHEN_TEMPLAR     = 15307,
    NPC_AZURE_TEMPLAR       = 15211,

    //Wind Stone
    NPC_DUKE_CYNDERS        = 15206,
    NPC_DUKE_FATHOMS        = 15207,
    NPC_DUKE_SHARDS         = 15208,
    NPC_DUKE_ZEPHYRS        = 15220,

    //Great Wind Stone
    NPC_LORD                = 15305,
    NPC_PRINCE              = 15203,
    NPC_BARON               = 15205,
    NPC_MARSHAL             = 15204,

    SPELL_TEMPLAR_RANDOM    = 24745,
    SPELL_DUKE_RANDOM       = 24763,    
    SPELL_ROYAL_RANDOM      = 24784, 
    SPELL_ARCANE_LIGHT      = 52921,    //When player dont have all items

    GO_LESSER_WIND_STONE    = 180456,
    GO_WIND_STONE           = 180461,
    GO_GREATH_WIND_STONE    = 180466,

    ITEM_MANTLE             = 20406,
    ITEM_COWL               = 20408,
    ITEM_ROBE               = 20407,
    ITEM_MEDALION           = 20422,
    ITEM_RING               = 20451,
};

bool GOGossipHello_go_lesser_wind_stone(Player* pPlayer,  GameObject* pGo)
{
    //pPlayer->CastSpell(pPlayer, SPELL_ARCANE_LIGHT, true);

    if(pPlayer->HasItemOrGemWithIdEquipped(ITEM_MANTLE, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_COWL, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_ROBE, 1))
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(100001, pGo->GetObjectGuid());
    }

    return true;
}

bool GOGossipSelect_go_lesser_wind_stone(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    switch(urand(0, 3))
    {
        case 0:
            pPlayer->CastSpell(pPlayer, SPELL_TEMPLAR_RANDOM, false);
            pPlayer->SummonCreature(NPC_HOARY_TEMPLAR, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 1:
            pPlayer->CastSpell(pPlayer, SPELL_TEMPLAR_RANDOM, false);
            pPlayer->SummonCreature(NPC_CRIMSON_TEMPLAR, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 2:
            pPlayer->CastSpell(pPlayer, SPELL_TEMPLAR_RANDOM, false);
            pPlayer->SummonCreature(NPC_EARTHEN_TEMPLAR, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 3:
            pPlayer->CastSpell(pPlayer, SPELL_TEMPLAR_RANDOM, false);
            pPlayer->SummonCreature(NPC_AZURE_TEMPLAR, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
    }
    return true;
}

/************
**Wind Stone
*************/
bool GOGossipHello_go_wind_stone(Player* pPlayer, GameObject* pGo)
{
    //pPlayer->CastSpell(pPlayer, SPELL_ARCANE_LIGHT, true);

    if(pPlayer->HasItemOrGemWithIdEquipped(ITEM_MANTLE, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_COWL, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_ROBE, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_MEDALION, 1))
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(100001, pGo->GetObjectGuid());
    }

    return true;
}

bool GOGossipSelect_go_wind_stone(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();
    switch(urand(0, 3))
    {
        case 0:
            pPlayer->CastSpell(pPlayer, SPELL_DUKE_RANDOM, false);
            pPlayer->SummonCreature(NPC_DUKE_CYNDERS, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 1:
            pPlayer->CastSpell(pPlayer, SPELL_DUKE_RANDOM, false);
            pPlayer->SummonCreature(NPC_DUKE_FATHOMS, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 2:
            pPlayer->CastSpell(pPlayer, SPELL_DUKE_RANDOM, false);
            pPlayer->SummonCreature(NPC_DUKE_SHARDS, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 3:
            pPlayer->CastSpell(pPlayer, SPELL_DUKE_RANDOM, false);
            pPlayer->SummonCreature(NPC_DUKE_ZEPHYRS, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
    }
    return true;
}

/******************
**Great Wind Stone
*******************/
bool GOGossipHello_go_great_wind_stone(Player* pPlayer, GameObject* pGo)
{
    //pPlayer->CastSpell(pPlayer, SPELL_ARCANE_LIGHT, true);

    if(pPlayer->HasItemOrGemWithIdEquipped(ITEM_MANTLE, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_COWL, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_ROBE, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_MEDALION, 1) && pPlayer->HasItemOrGemWithIdEquipped(ITEM_RING, 1))
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(100001, pGo->GetObjectGuid());
    }

    return true;
}

bool GOGossipSelect_go_great_wind_stone(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    switch(urand(0, 3))
    {
        case 0:
            pPlayer->CastSpell(pPlayer, SPELL_ROYAL_RANDOM, false);
            pGo->SummonCreature(NPC_LORD, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 1:
            pPlayer->CastSpell(pPlayer, SPELL_ROYAL_RANDOM, false);
            pGo->SummonCreature(NPC_PRINCE, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 2:
            pPlayer->CastSpell(pPlayer, SPELL_ROYAL_RANDOM, false);
            pGo->SummonCreature(NPC_BARON, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
        case 3:
            pPlayer->CastSpell(pPlayer, SPELL_ROYAL_RANDOM, false);
            pGo->SummonCreature(NPC_MARSHAL, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            break;
    }

    return true;
}

void AddSC_silithus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_highlord_demitrian";
    newscript->pGossipHello =  &GossipHello_npc_highlord_demitrian;
    newscript->pGossipSelect = &GossipSelect_npc_highlord_demitrian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npcs_rutgar_and_frankal";
    newscript->pGossipHello =  &GossipHello_npcs_rutgar_and_frankal;
    newscript->pGossipSelect = &GossipSelect_npcs_rutgar_and_frankal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_lesser_wind_stone";
    newscript->pGossipHelloGO  = &GOGossipHello_go_lesser_wind_stone;
    newscript->pGossipSelectGO = &GOGossipSelect_go_lesser_wind_stone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_wind_stone";
    newscript->pGossipHelloGO  = &GOGossipHello_go_wind_stone;
    newscript->pGossipSelectGO = &GOGossipSelect_go_wind_stone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_great_wind_stone";
    newscript->pGossipHelloGO  = &GOGossipHello_go_great_wind_stone;
    newscript->pGossipSelectGO = &GOGossipSelect_go_great_wind_stone;
    newscript->RegisterSelf();
}
