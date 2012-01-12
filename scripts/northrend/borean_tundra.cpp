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
SDName: Borean_Tundra
SD%Complete: 100
SDComment: Quest support: 11570, 11590, 11608, 11676, 11692, 11708, 11881, 11919, 11940, 11961. Taxi vendors.
SDCategory: Borean Tundra
EndScriptData */

/* ContentData
npc_fizzcrank_fullthrottle
npc_iruk
npc_kara_thricestar
npc_nesingwary_trapper
go_caribou_trap
npc_sinkhole_kill_credit
npc_surristrasz
npc_tiare
npc_lurgglbr
npc_nexus_drake
go_scourge_cage
npc_beryl_sorcerer
npc_seaforium_depth_charge
npc_jenny
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "follower_ai.h"
#include "pet_ai.h"

/*######
## npc_taunkale_evacuee
######*/

enum
{
    SAY_EMOTE_CRY           = -1999839,
    SAY_STOP                = -1999838,
    SAY_EMOTE_HAND          = -1999837,
    SAY_FINAL_POINT_1       = -1999836,
    SAY_EMOTE_POINT         = -1999835,
    SAY_FINAL_POINT_2       = -1999834,
    SAY_FINAL_POINT_3       = -1999833,
    SAY_FINAL_POINT_4       = -1999832,
    SAY_FINAL_POINT_5       = -1999831,

    QUEST_ACROSS_TRANSBOREA = 11930,
    SPELL_REFUGEE_AURA      = 46657,
    MAX_ACTION              = 9,
    MAX_NODE                = 29
};

float fBetweenBridges[2][3] =
{
    {3562.54f, 3052.27f, 24.41f},
    {3646.83f, 2903.63f, 89.84f}
};

struct EvacueeBehaviour
{
    uint32 uiText;
    uint32 uiEmote;
};

EvacueeBehaviour EmoteInfo[MAX_ACTION] =
{
    {SAY_EMOTE_CRY, EMOTE_ONESHOT_CRY},
    {SAY_STOP, EMOTE_ONESHOT_NONE},
    {SAY_EMOTE_HAND, EMOTE_STATE_KNEEL},
    {SAY_FINAL_POINT_1, EMOTE_STATE_NONE},
    {SAY_EMOTE_POINT, EMOTE_ONESHOT_POINT},
    {SAY_FINAL_POINT_2, EMOTE_ONESHOT_NONE},
    {SAY_FINAL_POINT_3, EMOTE_ONESHOT_NONE},
    {SAY_FINAL_POINT_4, EMOTE_ONESHOT_SHOUT},
    {SAY_FINAL_POINT_5, EMOTE_ONESHOT_NONE}
};

float fNodePosition[MAX_NODE][3] =
{
    {3565.72f, 3033.53f, 24.9965f},
    {3561.08f, 3018.83f, 25.5423f},
    {3553.36f, 3008.6f, 24.0242f},
    {3542.04f, 2991.62f, 20.9338f},
    {3531.22f, 2976.43f, 20.932f},
    {3515.59f, 2953.94f, 24.7565f},
    {3507.22f, 2940.59f, 25.0046f},
    {3506.25f, 2919.64f, 25.8977f},
    {3520.38f, 2907.51f, 30.3007f},
    {3532.14f, 2903.95f, 32.5451f},
    {3551.17f, 2898.59f, 33.8845f},
    {3566.89f, 2898.84f, 37.1695f},
    {3583.11f, 2900.46f, 40.7102f},
    {3597.23f, 2896.9f, 46.0036f},
    {3607.1f, 2888.81f, 52.842f},
    {3615.79f, 2879.3f, 59.2817f},
    {3623.99f, 2870.92f, 64.8177f},
    {3616.09f, 2864.06f, 68.4165f},
    {3613.75f, 2855.05f, 70.3628f},
    {3619.8f, 2841.17f, 72.0173f},
    {3625.71f, 2831.19f, 73.3184f},
    {3637.65f, 2828.49f, 76.5685f},
    {3650.15f, 2825.67f, 81.2263f},
    {3661.73f, 2827.0f, 83.4205f},
    {3667.76f, 2836.97f, 84.9583f},
    {3672.93f, 2849.29f, 87.4974f},
    {3674.77f, 2860.17f, 90.4819f},
    {3670.58f, 2869.79f, 91.3695f},
    {3664.37f, 2878.25f, 91.2818f},
};

struct MANGOS_DLL_DECL npc_taunkale_evacueeAI : public ScriptedPetAI
{
    npc_taunkale_evacueeAI(Creature* pCreature) : ScriptedPetAI(pCreature) 
    {
        Reset();
        uiAction = 0;
        m_bIsLeader = false;
    }

    bool m_bIsLeader;
    uint8 uiAction;
    uint8 uiNode;
    uint32 m_uiActionTimer;

    void Reset()
    {
        m_uiActionTimer = 3000;
    }

    void JustDied(Unit* pKiller)
    {
        if (Player* pOwner = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid()))
        {
            pOwner->RemoveAurasDueToSpell(SPELL_REFUGEE_AURA);
            pOwner->FailQuest(QUEST_ACROSS_TRANSBOREA);
        }
    }

    void UpdatePetOOCAI(const uint32 uiDiff)
    {
        if (m_uiActionTimer < uiDiff)
        {
            m_uiActionTimer = 2000;
            if (m_creature->IsInRange2d(fBetweenBridges[0][0], fBetweenBridges[0][1], 0.0f, 10.0f) && !m_bIsLeader)
            {
                if (!uiAction)
                {
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->SetFacingTo(5.32f);
                }

                DoScriptText(EmoteInfo[uiAction].uiText, m_creature);
                m_creature->HandleEmoteCommand(EmoteInfo[uiAction].uiEmote);
                ++uiAction;

                if (uiAction > 8)
                {
                    if (Player* pOwner = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid()))
                    {
                        pOwner->RemoveAurasDueToSpell(SPELL_REFUGEE_AURA);
                        pOwner->GroupEventHappens(QUEST_ACROSS_TRANSBOREA, m_creature);
                    }

                    m_bIsLeader = true;
                    uiNode = 0;
                    GoToTheNextNode();
                }
                m_uiActionTimer = 5000;
            }
            else if (!uiAction && !urand(0, 20) && !m_bIsLeader)
            {
                DoScriptText(EmoteInfo[uiAction].uiText, m_creature);
                m_creature->HandleEmoteCommand(EmoteInfo[uiAction].uiEmote);
            }
        }
        else 
            m_uiActionTimer -= uiDiff;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId >= MAX_NODE)
            m_creature->ForcedDespawn();
        else if (uiPointId == uiNode)
        {
            ++uiNode;
            GoToTheNextNode();
        }
    }

    void GoToTheNextNode()
    {
        m_creature->GetMotionMaster()->MovePoint(uiNode, fNodePosition[uiNode][0], fNodePosition[uiNode][1], fNodePosition[uiNode][2]);
    }
};

CreatureAI* GetAI_npc_taunkale_evacuee(Creature* pCreature)
{
    return new npc_taunkale_evacueeAI(pCreature);
}

/*######
## npc_fizzcrank_fullthrottle
######*/

#define GOSSIP_ITEM_GO_ON   "Go on."
#define GOSSIP_ITEM_TELL_ME "Tell me what's going on out here, Fizzcrank."

enum
{
    GOSSIP_TEXTID_FIZZCRANK1    = 12456,
    GOSSIP_TEXTID_FIZZCRANK2    = 12457,
    GOSSIP_TEXTID_FIZZCRANK3    = 12458,
    GOSSIP_TEXTID_FIZZCRANK4    = 12459,
    GOSSIP_TEXTID_FIZZCRANK5    = 12460,
    GOSSIP_TEXTID_FIZZCRANK6    = 12461,
    GOSSIP_TEXTID_FIZZCRANK7    = 12462,
    GOSSIP_TEXTID_FIZZCRANK8    = 12463,
    GOSSIP_TEXTID_FIZZCRANK9    = 12464,

    QUEST_THE_MECHAGNOMES       = 11708
};

bool GossipHello_npc_fizzcrank_fullthrottle(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_THE_MECHAGNOMES) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELL_ME, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_fizzcrank_fullthrottle(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK7, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+8:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK8, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+9:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK9, pCreature->GetObjectGuid());
            pPlayer->AreaExploredOrEventHappens(QUEST_THE_MECHAGNOMES);
            break;
    }
    return true;
}

/*######
## npc_iruk
######*/

#define GOSSIP_ITEM_IRUK        "<Search corpse for Issliruk's Totem.>"

enum
{
    QUEST_SPIRITS_WATCH_OVER_US     = 11961,
    SPELL_CREATE_TOTEM              = 46816
};

bool GossipHello_npc_iruk(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_SPIRITS_WATCH_OVER_US) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_IRUK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_iruk(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,SPELL_CREATE_TOTEM,true);
    }

    return true;
}

/*######
## npc_kara_thricestar
######*/

#define GOSSIP_ITEM_THRICESTAR1      "Do you think I could take a ride on one of those flying machines?"
#define GOSSIP_ITEM_THRICESTAR2      "Kara, I need to be flown out the Dens of Dying to find Bixie."

enum
{
    QUEST_CHECK_IN_WITH_BIXIE       = 11692,
    SPELL_FIZZCRANK_AIRSTRIP        = 51446
};

bool GossipHello_npc_kara_thricestar(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isTaxi())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_ITEM_THRICESTAR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetQuestStatus(QUEST_CHECK_IN_WITH_BIXIE) == QUEST_STATUS_COMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THRICESTAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_kara_thricestar(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->GetSession()->SendTaxiMenu(pCreature);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_FIZZCRANK_AIRSTRIP, false);
            break;
    }

    return true;
}

/*######
## npc_nesingwary_trapper
######*/

enum
{
    NPC_NESINGWARY_TRAPPER  = 25835,
    GO_QUALITY_FUR          = 187983,

    SAY_PHRASE_1            = -1000599,
    SAY_PHRASE_2            = -1000600,
    SAY_PHRASE_3            = -1000601,
    SAY_PHRASE_4            = -1000602
};

struct MANGOS_DLL_DECL npc_nesingwary_trapperAI : public ScriptedAI
{
    npc_nesingwary_trapperAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiPhase;
    uint32 m_uiPhaseTimer;
    ObjectGuid m_playerGuid;
    ObjectGuid m_trapGuid;

    void Reset()
    {
        m_uiPhase = 0;
        m_uiPhaseTimer = 0;
        m_playerGuid.Clear();
        m_trapGuid.Clear();
    }

    void StartAction(Player* pPlayer, GameObject* pTrap)
    {
        m_uiPhase = 1;
        m_uiPhaseTimer = 3000;
        m_playerGuid = pPlayer->GetObjectGuid();
        m_trapGuid = pTrap->GetObjectGuid();

        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_PHRASE_1, m_creature); break;
            case 1: DoScriptText(SAY_PHRASE_2, m_creature); break;
            case 2: DoScriptText(SAY_PHRASE_3, m_creature); break;
            case 3: DoScriptText(SAY_PHRASE_4, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->getVictim() && m_uiPhase)
        {
            if (m_uiPhaseTimer <= uiDiff)
            {
                switch(m_uiPhase)
                {
                    case 1:
                        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_trapGuid))
                        {
                            if (pTrap->isSpawned())
                                m_creature->GetMotionMaster()->MovePoint(0, pTrap->GetPositionX(), pTrap->GetPositionY(), pTrap->GetPositionZ());
                        }
                        break;
                    case 2:
                        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_trapGuid))
                        {
                            if (pTrap->isSpawned())
                            {
                                pTrap->Use(m_creature);

                                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                                {
                                    if (pPlayer->isAlive())
                                        pPlayer->KilledMonsterCredit(m_creature->GetEntry());
                                }
                            }
                        }
                        break;
                }

                m_uiPhase = 0;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        m_creature->HandleEmote(EMOTE_ONESHOT_LOOT);
        m_uiPhaseTimer = 2000;
        m_uiPhase = 2;
    }
};

CreatureAI* GetAI_npc_nesingwary_trapper(Creature* pCreature)
{
    return new npc_nesingwary_trapperAI(pCreature);
}

/*#####
# npc_oil_stained_wolf
#####*/

enum
{
    SPELL_THROW_WOLF_BAIT           = 53326,
    SPELL_PLACE_WOLF_BAIT           = 46072,                // doesn't appear to be used for anything
    SPELL_HAS_EATEN                 = 46073,
    SPELL_SUMMON_DROPPINGS          = 46075,

    FACTION_MONSTER                 = 634,

    POINT_DEST                      = 1
};

struct MANGOS_DLL_DECL npc_oil_stained_wolfAI : public ScriptedAI
{
    npc_oil_stained_wolfAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bCanCrapInPublic;
    uint32 m_uiPooTimer;

    void Reset()
    {
        m_bCanCrapInPublic = false;
        m_uiPooTimer = 0;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_DEST)
        {
            DoCastSpellIfCan(m_creature, SPELL_HAS_EATEN);
            m_uiPooTimer = 4000;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiPooTimer)
            {
                if (m_uiPooTimer <= uiDiff)
                {
                    if (m_bCanCrapInPublic)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DROPPINGS);
                        m_creature->GetMotionMaster()->Clear();
                        Reset();
                    }
                    else
                    {
                        m_creature->HandleEmote(EMOTE_ONESHOT_BATTLEROAR);
                        m_bCanCrapInPublic = true;
                        m_uiPooTimer = 3000;
                    }
                }
                else
                    m_uiPooTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oil_stained_wolf(Creature* pCreature)
{
    return new npc_oil_stained_wolfAI(pCreature);
}

bool EffectDummyCreature_npc_oil_stained_wolf(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)
{
    if (uiSpellId == SPELL_THROW_WOLF_BAIT)
    {
        if (uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->getFaction() != FACTION_MONSTER && !pCreatureTarget->HasAura(SPELL_HAS_EATEN))
        {
            pCreatureTarget->SetFactionTemporary(FACTION_MONSTER);
            pCreatureTarget->SetWalk(false);

            pCreatureTarget->GetMotionMaster()->MoveIdle();

            float fX, fY, fZ;
            pCaster->GetContactPoint(pCreatureTarget, fX, fY, fZ, CONTACT_DISTANCE);
            pCreatureTarget->GetMotionMaster()->MovePoint(POINT_DEST, fX, fY, fZ);
            return true;
        }
    }

    return false;
}

bool EffectAuraDummy_npc_oil_stained_wolf(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_HAS_EATEN)
    {
        if (pAura->GetEffIndex() != EFFECT_INDEX_0)
            return false;

        if (bApply)
        {
            pAura->GetTarget()->HandleEmote(EMOTE_ONESHOT_CUSTOMSPELL01);
        }
        else
        {
            Creature* pCreature = (Creature*)pAura->GetTarget();
            pCreature->setFaction(pCreature->GetCreatureInfo()->faction_A);
        }

        return true;
    }

    return false;
}

/*######
## go_caribou_trap
######*/

bool GOUse_go_caribou_trap(Player* pPlayer, GameObject* pGo)
{
    float fX, fY, fZ;
    pGo->GetClosePoint(fX, fY, fZ, pGo->GetObjectBoundingRadius(), 2*INTERACTION_DISTANCE, frand(0, M_PI_F*2));

    if (Creature* pCreature = pGo->SummonCreature(NPC_NESINGWARY_TRAPPER, fX, fY, fZ, pGo->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
    {
        if (npc_nesingwary_trapperAI* pTrapperAI = dynamic_cast<npc_nesingwary_trapperAI*>(pCreature->AI()))
            pTrapperAI->StartAction(pPlayer, pGo);

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

        if (GameObject* pGoFur = GetClosestGameObjectWithEntry(pGo, GO_QUALITY_FUR, INTERACTION_DISTANCE))
        {
            if (!pGoFur->isSpawned())
            {
                pGoFur->SetRespawnTime(10);
                pGoFur->Refresh();
            }
        }
    }

    return true;
}
/*#####
# npc_sinkhole_kill_credit
#####*/

enum
{
    SPELL_SUMMON_EXPLOSIVES_CART_FIRE   = 46799,
    SPELL_SUMMON_SCOURGE_BURROWER       = 46800,
    SPELL_COSMETIC_HUGE_EXPLOSION       = 46225,
    SPELL_CANNON_FIRE                   = 42445,
};

struct MANGOS_DLL_DECL npc_sinkhole_kill_creditAI : public ScriptedAI
{
    npc_sinkhole_kill_creditAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_cartGuid;
    ObjectGuid m_wormGuid;
    uint32 m_uiCartTimer;
    uint32 m_uiCartPhase;

    void Reset()
    {
        m_cartGuid.Clear();
        m_wormGuid.Clear();
        m_uiCartTimer = 2000;
        m_uiCartPhase = 0;
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_wormGuid = pSummoned->GetObjectGuid();
    }

    void JustSummoned(GameObject* pGo)
    {
        // Go is not really needed, but ok to use as a check point so only one "event" can be processed at a time
        if (m_cartGuid)
            return;

        // Expecting summoned from mangos dummy effect 46797
        m_cartGuid = pGo->GetObjectGuid();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_cartGuid)
        {
            if (m_uiCartTimer <= uiDiff)
            {
                switch(m_uiCartPhase)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_EXPLOSIVES_CART_FIRE);
                        m_uiCartTimer = 4000;
                        break;
                    case 1:
                        // Unclear if these should be in a dummy effect or not.
                        // The order of spells are correct though.
                        DoCastSpellIfCan(m_creature, SPELL_COSMETIC_HUGE_EXPLOSION, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_CANNON_FIRE, CAST_TRIGGERED);
                        break;
                    case 2:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCOURGE_BURROWER);
                        m_uiCartTimer = 2000;
                        break;
                    case 3:
                        if (Creature* pWorm = m_creature->GetMap()->GetCreature(m_wormGuid))
                        {
                            pWorm->SetDeathState(JUST_DIED);
                            pWorm->SetHealth(0);
                        }
                        m_uiCartTimer = 10000;
                        break;
                    case 4:
                        if (Creature* pWorm = m_creature->GetMap()->GetCreature(m_wormGuid))
                            pWorm->RemoveCorpse();

                        Reset();
                        return;
                }

                ++m_uiCartPhase;
            }
            else
                m_uiCartTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_sinkhole_kill_credit(Creature* pCreature)
{
    return new npc_sinkhole_kill_creditAI(pCreature);
}

/*######
## npc_surristrasz
######*/

#define GOSSIP_ITEM_FREE_FLIGHT "I'd like passage to the Transitus Shield."
#define GOSSIP_ITEM_FLIGHT      "May I use a drake to fly elsewhere?"

enum
{
    SPELL_ABMER_TO_COLDARRA     = 46064
};

bool GossipHello_npc_surristrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isTaxi())
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FREE_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_GOSSIP);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_ITEM_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_TAXIVENDOR);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_surristrasz(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_OPTION_GOSSIP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        //TaxiPath 795 (amber to coldarra)
        pPlayer->CastSpell(pPlayer, SPELL_ABMER_TO_COLDARRA, true);
    }

    if (uiAction == GOSSIP_OPTION_TAXIVENDOR)
        pPlayer->GetSession()->SendTaxiMenu(pCreature);

    return true;
}

/*######
## npc_tiare
######*/

#define GOSSIP_ITEM_TELEPORT    "Teleport me to Amber Ledge, please."

enum
{
    SPELL_TELEPORT_COLDARRA     = 50135
};

bool GossipHello_npc_tiare(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_GOSSIP);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_tiare(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_OPTION_GOSSIP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_TELEPORT_COLDARRA, true);
    }
    return true;
}

/*######
## npc_lurgglbr
######*/

enum
{
    QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS = 11570,
    GO_CAGE                             = 187369,

    SAY_START_1                         = -1000575,
    SAY_START_2                         = -1000576,
    SAY_END_1                           = -1000577,
    SAY_END_2                           = -1000578
};

struct MANGOS_DLL_DECL npc_lurgglbrAI : public npc_escortAI
{
    npc_lurgglbrAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiSayTimer = 0;
        m_uiSpeech = 0;
        Reset();
    }

    uint32 m_uiSayTimer;
    uint8 m_uiSpeech;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSayTimer = 0;
            m_uiSpeech = 0;
        }
    }

    void JustStartedEscort()
    {
        if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_CAGE, INTERACTION_DISTANCE))
        {
            if (pCage->GetGoState() == GO_STATE_READY)
                pCage->Use(m_creature);
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_START_2, m_creature, pPlayer);

                // Cage actually closes here, however it's normally determined by GO template and auto close time

                break;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    m_creature->SetFacingToObject(pPlayer);
                    DoScriptText(SAY_START_1, m_creature, pPlayer);
                }
                break;
            case 25:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_END_1, m_creature, pPlayer);
                    m_uiSayTimer = 3000;
                }
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiSayTimer)
            {
                if (m_uiSayTimer <= uiDiff)
                {
                    Player* pPlayer = GetPlayerForEscort();

                    if (!pPlayer)
                    {
                        m_uiSayTimer = 0;
                        return;
                    }

                    m_creature->SetFacingToObject(pPlayer);

                    switch(m_uiSpeech)
                    {
                        case 0:
                            DoScriptText(SAY_END_2, m_creature, pPlayer);
                            m_uiSayTimer = 3000;
                            break;
                        case 1:
                            pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS, m_creature);
                            m_uiSayTimer = 0;
                            break;
                    }

                    ++m_uiSpeech;
                }
                else
                    m_uiSayTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_lurgglbr(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS)
    {
        if (npc_lurgglbrAI* pEscortAI = dynamic_cast<npc_lurgglbrAI*>(pCreature->AI()))
        {
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_lurgglbr(Creature* pCreature)
{
    return new npc_lurgglbrAI(pCreature);
}

/*######
## npc_nexus_drake_hatchling
######*/

enum
{
    SPELL_DRAKE_HARPOON             = 46607,
    SPELL_RED_DRAGONBLOOD           = 46620,
    SPELL_DRAKE_HATCHLING_SUBDUED   = 46691,
    SPELL_SUBDUED                   = 46675,

    NPC_RAELORASZ                   = 26117,
    DRAKE_HUNT_KILL_CREDIT          = 26175,

    SPELL_INTANGIBLE_PRESENCE     = 36513,
    SPELL_NETHERBREATH            = 36631,

    QUEST_DRAKE_HUNT                = 11919,
    QUEST_DRAKE_HUNT_D              = 11940

};

struct MANGOS_DLL_DECL npc_nexus_drakeAI : public FollowerAI
{
    npc_nexus_drakeAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }
    
     ObjectGuid uiHarpoonerGuid;
     bool bWithRedDragonBlood;
     bool bIsFollowing;
     uint32 SPELL_INTANGIBLE_PRESENCE_Timer;
     uint32 SPELL_NETHERBREATH_Timer;

     void Reset()
     {
         bWithRedDragonBlood = false;
         bIsFollowing = false;
         SPELL_INTANGIBLE_PRESENCE_Timer = 16600;
         SPELL_NETHERBREATH_Timer = 4600;
     }

     void EnterCombat(Unit* pWho)
     {
         AttackStart(pWho);
     }

     void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
     {
            if (pSpell->Id == SPELL_DRAKE_HARPOON && pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                uiHarpoonerGuid = pCaster->GetObjectGuid();
                DoCast(m_creature, SPELL_RED_DRAGONBLOOD, true);
            }
            m_creature->Attack(pCaster,true);
            bWithRedDragonBlood = true;
     }

     void MoveInLineOfSight(Unit *pWho)
     {
         FollowerAI::MoveInLineOfSight(pWho);


         if (pWho->GetEntry() == NPC_RAELORASZ && m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
         {
           if (Player *pHarpooner = m_creature->GetMap()->GetPlayer(uiHarpoonerGuid))
                 {
                    
                     pHarpooner->KilledMonsterCredit(DRAKE_HUNT_KILL_CREDIT, m_creature->GetObjectGuid());
                     pHarpooner->RemoveAurasByCasterSpell(SPELL_DRAKE_HATCHLING_SUBDUED, uiHarpoonerGuid);
                     SetFollowComplete();
                     uiHarpoonerGuid = 0;
                     m_creature->ForcedDespawn(1000);
                 }

          }
      }

     void UpdateAI(const uint32 uidiff)
        {
            if (bWithRedDragonBlood && uiHarpoonerGuid && !m_creature->HasAura(SPELL_RED_DRAGONBLOOD))
            {
                if (Player *pHarpooner = m_creature->GetMap()->GetPlayer(uiHarpoonerGuid))
                {
                    EnterEvadeMode();
                    StartFollow(pHarpooner, 35, NULL);

                    DoCast(m_creature, SPELL_SUBDUED, true);
                    pHarpooner->CastSpell(pHarpooner, SPELL_DRAKE_HATCHLING_SUBDUED, true);

                    m_creature->AttackStop();
                    bIsFollowing = true;
                    bWithRedDragonBlood = false;
                }
            }
            if(bIsFollowing && !m_creature->HasAura(SPELL_SUBDUED))
            {
                m_creature->ForcedDespawn(1000);
            }

            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (SPELL_NETHERBREATH_Timer < uidiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_NETHERBREATH);
                SPELL_NETHERBREATH_Timer = 4600;
            }
            else
                SPELL_NETHERBREATH_Timer -= uidiff;

            if (SPELL_INTANGIBLE_PRESENCE_Timer < uidiff)
            {
                DoCastSpellIfCan(m_creature,SPELL_INTANGIBLE_PRESENCE);
                SPELL_INTANGIBLE_PRESENCE_Timer = 16600;
             }
             else
                 SPELL_INTANGIBLE_PRESENCE_Timer -= uidiff;

            DoMeleeAttackIfReady();
        }
};

CreatureAI* GetAI_npc_nexus_drake(Creature* pCreature)
{
    return new npc_nexus_drakeAI(pCreature);
}

/*#####
## go_scourge_cage
#####*/

enum
{
    QUEST_MERCIFUL_FREEDOM      =  11676,
    NPC_SCOURGE_PRISONER        =  25610,
};

bool GOHello_go_scourge_cage(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_MERCIFUL_FREEDOM) == QUEST_STATUS_INCOMPLETE)
    {
        Creature *pCreature = GetClosestCreatureWithEntry(pGo, NPC_SCOURGE_PRISONER, INTERACTION_DISTANCE);
        if(pCreature)
        {
            pPlayer->KilledMonsterCredit(NPC_SCOURGE_PRISONER, pCreature->GetObjectGuid());
            pCreature->CastSpell(pCreature, 43014, false);
        }
    }
    return false;
};

/*######
## npc_beryl_sorcerer
######*/

enum eBerylSorcerer
{
    NPC_CAPTURED_BERLY_SORCERER         = 25474,
    NPC_LIBRARIAN_DONATHAN              = 25262,

    SPELL_FROST_BOLT                     = 9672,
    SPELL_BLINK                          = 50648,

    SPELL_ARCANE_CHAINS                 = 45611,
    SPELL_COSMETIC_CHAINS               = 54324,
    SPELL_COSMETIC_ENSLAVE_CHAINS_SELF  = 45631
};

struct MANGOS_DLL_DECL npc_beryl_sorcererAI : public FollowerAI
{
    npc_beryl_sorcererAI(Creature* pCreature) : FollowerAI(pCreature) {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    bool bEnslaved;
    ObjectGuid uiChainerGuid;
    uint32 m_uiNormalFaction;

    uint32 SPELL_FROST_BOLT_Timer;
    uint32 SPELL_BLINK_Timer;

    void Reset()
    {
         m_creature->setFaction(14);
         bEnslaved = false;
         SPELL_FROST_BOLT_Timer = 5400;
         SPELL_BLINK_Timer = 15000;
    }
    void EnterCombat(Unit* pWho)
    {
            AttackStart(pWho);
    }

    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_ARCANE_CHAINS && pCaster->GetTypeId() == TYPEID_PLAYER && !bEnslaved)
            {
                EnterEvadeMode(); //We make sure that the npc is not attacking the player!
                m_creature->setFaction(35);
                uiChainerGuid = pCaster->GetObjectGuid();
                if(Player *pChainer = m_creature->GetMap()->GetPlayer(uiChainerGuid))
                {
                StartFollow(pChainer, 35, NULL);
                m_creature->UpdateEntry(NPC_CAPTURED_BERLY_SORCERER);
                DoCast(m_creature, SPELL_COSMETIC_ENSLAVE_CHAINS_SELF, true);
                bEnslaved = true;
                }
            }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
            FollowerAI::MoveInLineOfSight(pWho);

            if (pWho->GetEntry() == NPC_LIBRARIAN_DONATHAN && m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if(Player *pChainer = m_creature->GetMap()->GetPlayer(uiChainerGuid))
                {
                    pChainer->KilledMonsterCredit(NPC_CAPTURED_BERLY_SORCERER,m_creature->GetObjectGuid());
                    SetFollowComplete();
                    m_creature->ForcedDespawn(1000);
                }
            }
     }
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        /// Frost_bolt needs to be casted more after blink
        if (SPELL_FROST_BOLT_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_BOLT);
            SPELL_FROST_BOLT_Timer = 5400 + rand()%1400;
        }else SPELL_FROST_BOLT_Timer -= uiDiff;

        if (SPELL_BLINK_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BLINK);
            SPELL_BLINK_Timer = 15000 + rand()%3000;
        }else SPELL_BLINK_Timer -= uiDiff;

            DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_npc_beryl_sorcerer(Creature* pCreature)
{
    return new npc_beryl_sorcererAI(pCreature);
}

/**** Quest: Last Rites ****/
enum
{
    /************* Speech and Sounds ***********************/
    SAY_DIA01 = -1534071,
    SAY_DIA02 = -1534072,
    SAY_DIA03 = -1534073,
    SAY_DIA04 = -1534074,
    SAY_DIA05 = -1534075,
    SAY_DIA06 = -1534076,
    SAY_DIA07 = -1534077,
    SAY_DIA08 = -1534078,
    SAY_DIA09 = -1534079,
    SAY_DIA10 = -1534080,
    SAY_DIA11 = -1534081,
    SAY_DIA12 = -1534082,
    SAY_DIA13 = -1534083,
    SAY_DIA14 = -1534084,
    SAY_DIA15 = -1534085,
    SAY_DIA16 = -1534086,
    SAY_DIA17 = -1534087,
    SAY_DIA18 = -1534088,
    SAY_DIA19 = -1534089,
    SAY_DIA20 = -1534090,

    /************* Npc's ***********************/
    NPC_THASSARIAN = 26170,
    NPC_LICH_KING = 26203,
    NPC_LERYSSA = 25251,
    NPC_ARLOS = 25250,
    NPC_VALANAR = 28189,
    NPC_VISUAL_FORM = 25301,

    /************* Other's ***********************/
    QUEST_LAST_RITES = 12019,
    STAFF_ID = 485,
    AREA_ID = 4128,
};

#define GOSSIP_ITEM_T "Let's do this, Thassarian. It's now or never."

struct MANGOS_DLL_DECL npc_thassarianAI : public ScriptedAI
{
    npc_thassarianAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiStepTimer;
    uint32 m_uiStep;

    ObjectGuid m_uiValanarGuid;
    ObjectGuid m_uiLichKingGuid;
    ObjectGuid m_uiLeryssaGuid;
    ObjectGuid m_uiArlosGuid;

    void Reset()
    {
        m_uiValanarGuid = 0;
        m_uiLichKingGuid = 0;
        m_uiLeryssaGuid = 0;
        m_uiArlosGuid = 0;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        if (m_creature->GetAreaId() == AREA_ID)
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_uiStep = 0;
        m_uiStepTimer = 100;
    }

    void JustDied(Unit* pKiller)
    {
        if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
Kill(Leryssa);
        if (Unit* Arlos = m_creature->GetMap()->GetCreature(m_uiArlosGuid))
            Kill(Arlos);
        if (Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
            Valanar->RemoveFromWorld();
    }

    void EnterEvadeMode()
    {
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        m_creature->SetLootRecipient(NULL);

        if (m_creature->isAlive() && m_uiStep > 20)
        {
            m_creature->GetMotionMaster()->MoveTargetedHome();
            Reset();
        }
    }

    void StartEvent(uint8 EventId)
    {
        m_uiStep = EventId;
        m_uiStepTimer = 100;
    }

    void JumpNextStep(uint32 Time)
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

    void Kill(Unit* Oo)
    {
        Oo->DealDamage(Oo, Oo->GetMaxHealth() ,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 1:
               m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
               m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
               m_creature->SetWalk(true);
               m_creature->GetMotionMaster()->MovePoint(0, 3712.466f, 3570.2911f, 477.443f);
               JumpNextStep(4000);
               break;
            case 2:
               m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
               if (Creature* LichKing = m_creature->SummonCreature(NPC_LICH_KING,3730.15f,3519.43f,473.32f,1.37f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30))
               {
                   m_uiLichKingGuid = LichKing->GetObjectGuid();
                   LichKing->GetMotionMaster()->MovePoint(0, 3738.691f, 3564.734f, 477.442f);
               }
               JumpNextStep(3000);
               break;
            case 3:
               if (Creature* Valanar = m_creature->SummonCreature(NPC_VALANAR,3748.80f,3615.27f,473.3232f,4.50f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000))
               {
                   m_uiValanarGuid = Valanar->GetObjectGuid();
                   Valanar->SetRespawnDelay(DAY);
                   Valanar->GetMotionMaster()->MovePoint(0, 3739.823f, 3570.564f, 477.442f);
                   ((Creature*)Valanar)->UpdateEntry(NPC_VISUAL_FORM, HORDE); //huck! Replace spell (spell "Valanar Transformation" don't work!) He must update Valanar Entry and Change Name!
                   Valanar->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                   Valanar->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                   Valanar->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(0));
               }
               JumpNextStep(20000);
               break;
            case 4:
               if (Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
               {
                   ((Creature*)Valanar)->UpdateEntry(NPC_VALANAR);
                   Valanar->SetStandState(UNIT_STAND_STATE_KNEEL);
                   ((Creature*)Valanar)->LoadEquipment(STAFF_ID);
                   DoScriptText(SAY_DIA01, Valanar);
               }
               if (Creature* Leryssa = m_creature->SummonCreature(NPC_LERYSSA,3753.105f,3614.296f,473.32f,1.37f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10))
               {
                   m_uiLeryssaGuid = Leryssa->GetObjectGuid();
                   Leryssa->GetMotionMaster()->MovePoint(0, 3745.636f, 3572.464f, 477.442f);
                   Leryssa->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                   Leryssa->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
               }
               if (Creature* Arlos = m_creature->SummonCreature(NPC_ARLOS,3744.82f,3616.11f,473.32f,1.37f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10))
               {
                   m_uiArlosGuid = Arlos->GetObjectGuid();
                   Arlos->GetMotionMaster()->MovePoint(0, 3734.854f, 3573.028f, 477.442f);
                   Arlos->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                   Arlos->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
               }
               JumpNextStep(8000);
               break;
            case 5:
               if (Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
DoScriptText(SAY_DIA02, Valanar);
               JumpNextStep(11000);
               break;
            case 6:
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
                   Leryssa->CastSpell(Leryssa,45066,false);
               if (Unit* Arlos = m_creature->GetMap()->GetCreature(m_uiArlosGuid))
                   Arlos->CastSpell(Arlos,45066,false);
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
                   DoScriptText(SAY_DIA03, LichKing);
               JumpNextStep(9000);
               break;
            case 7:
               m_creature->SetWalk(false);
               m_creature->GetMotionMaster()->MovePoint(0, 3722.271f, 3566.878f, 477.442f);
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
                   m_creature->SetFacingToObject(LichKing);
               DoScriptText(SAY_DIA04, m_creature);
               JumpNextStep(2000);
               break;
            case 8:
               if (Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
                   Valanar->SetStandState(UNIT_STAND_STATE_STAND);
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
                   LichKing->SetFacingToObject(m_creature);
               DoScriptText(SAY_DIA05, m_creature);
               JumpNextStep(6000);
               break;
            case 9:
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
                   DoScriptText(SAY_DIA06, LichKing);
               JumpNextStep(17000);
               break;
            case 10:
               DoScriptText(SAY_DIA07, m_creature);
               JumpNextStep(9000);
               break;
            case 11:
               if (Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
                   DoScriptText(SAY_DIA08, Valanar);
               JumpNextStep(6000);
               break;
            case 12:
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
               {
                   if(Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
                   {
                       LichKing->SetFacingToObject(Valanar);
DoScriptText(SAY_DIA09, LichKing);
                   }
               }
               JumpNextStep(4000);
               break;
            case 13:
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
                   LichKing->SetFacingToObject(m_creature);
               JumpNextStep(500);
               break;
            case 14:
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
                   LichKing->HandleEmoteCommand(25);
               JumpNextStep(5500);
               break;
            case 15:
               if (Unit* Valanar = m_creature->GetMap()->GetCreature(m_uiValanarGuid))
               {
                   DoScriptText(SAY_DIA10, Valanar);
                   Valanar->SetFacingToObject(m_creature);
                   Valanar->setFaction(14);
                   m_creature->AI()->AttackStart(Valanar);
               }
               if (Unit* LichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGuid))
Kill(LichKing);
               JumpNextStep(500);
               break;
case 17:
               EnterEvadeMode();
               m_creature->GetMotionMaster()->MovePoint(0, 3721.361f, 3566.910f, 477.442f);
               m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, UNIT_STAND_STATE_STAND);
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
               {
                   DoScriptText(SAY_DIA14, Leryssa);
                   Leryssa->RemoveAurasDueToSpell(45066);
               }
               if (Unit* Arlos = m_creature->GetMap()->GetCreature(m_uiArlosGuid))
                   DoScriptText(SAY_DIA11, Arlos);
               JumpNextStep(3000);
               break;
            case 18:
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
               {
                   ((Creature*)Leryssa)->SetWalk(false);
                   Leryssa->GetMotionMaster()->MovePoint(0, 3726.749f, 3568.097f, 477.442f);
                   Leryssa->SetFacingToObject(m_creature);
                   Leryssa->SetStandState(UNIT_STAND_STATE_SIT);
                   m_creature->SetFacingToObject(Leryssa);
                   m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
               }
               if (Unit* Arlos = m_creature->GetMap()->GetCreature(m_uiArlosGuid))
               {
                   DoScriptText(SAY_DIA12, Arlos);
                   Arlos->RemoveAurasDueToSpell(45066);
                   Arlos->SetStandState(UNIT_STAND_STATE_DEAD);
               }
               JumpNextStep(5000);
               break;
            case 19:
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
                   DoScriptText(SAY_DIA15, Leryssa);
               JumpNextStep(5000);
               break;
            case 20:
               DoScriptText(SAY_DIA13, m_creature);
               JumpNextStep(3000);
               break;
            case 21:
               DoScriptText(SAY_DIA16, m_creature);
               JumpNextStep(6000);
               break;
            case 22:
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
                   DoScriptText(SAY_DIA17, Leryssa);
               JumpNextStep(9000);
               break;
            case 23:
               DoScriptText(SAY_DIA18, m_creature);
               JumpNextStep(9000);
               break;
            case 24:
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
                   DoScriptText(SAY_DIA19, Leryssa);
               JumpNextStep(8000);
               break;
            case 25:
               DoScriptText(SAY_DIA20, m_creature);
               JumpNextStep(6000);
               break;
            case 26:
               m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
               JumpNextStep(60000);
               break;
            case 27:
               if (Unit* Leryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGuid))
Kill(Leryssa);
               if (Unit* Arlos = m_creature->GetMap()->GetCreature(m_uiArlosGuid))
                   Kill(Arlos);
               JumpNextStep(6000);
               EnterEvadeMode();
               m_creature->SetStandState(UNIT_STAND_STATE_STAND);
               break;
         }
    }

    void UpdateAI(const uint32 uiDiff)
    {
         if (m_uiStepTimer <= uiDiff)
             Event();
         else
             m_uiStepTimer -= uiDiff;

         if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

         DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_thassarian(Creature* pCreature)
{
    return new npc_thassarianAI(pCreature);
}

/*Prince Valanar AI*/

enum
{
   SPELL_SHADOW_BOLT = 15537,
   SPELL_SOUL_BLAST = 50992,
   SPELL_SOUL_MIRROR = 51009
};

struct MANGOS_DLL_DECL npc_valanarAI : public ScriptedAI
{
    npc_valanarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        SetCombatMovement(false);
    }

    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShadowBlastTimer;
    uint32 m_uiSoulTimer;

    void Reset()
    {
        m_uiShadowBoltTimer = 1300;
        m_uiShadowBlastTimer = 7300;
        m_uiSoulTimer = 17300;
    }
 
    void JustDied(Unit* pKiller)
    {
        if (Creature* pTas = GetClosestCreatureWithEntry(m_creature, NPC_THASSARIAN, 80.0f))
           ((npc_thassarianAI*)pTas->AI())->StartEvent(17);
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_SOUL_BLAST)
           m_creature->DealDamage(target, target->GetMaxHealth() / 5,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by->HasAura(SPELL_SOUL_MIRROR))
           m_creature->DealDamage(done_by, damage ,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShadowBoltTimer < diff)
        {
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
               DoCast(pTarget, SPELL_SHADOW_BOLT);
m_uiShadowBoltTimer = 3300;
        }
        else
            m_uiShadowBoltTimer -= diff;

        if (m_uiShadowBlastTimer < diff)
        {
           m_creature->InterruptNonMeleeSpells(false);
           if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
              DoCast(pTarget,SPELL_SOUL_BLAST);
m_uiShadowBlastTimer = urand(6000, 12000);
        }
        else
            m_uiShadowBlastTimer -= diff;

        if (m_uiSoulTimer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(),SPELL_SOUL_MIRROR);
            m_uiSoulTimer = urand(15000, 20000);
        }
        else
            m_uiSoulTimer -= diff;
    }
};

bool GossipHello_npc_thassarian(Player *player, Creature *pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_INCOMPLETE)
        if(((npc_thassarianAI*)pCreature->AI())->m_uiStep == 0 && pCreature->GetAreaId() == AREA_ID)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_T, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(pCreature),pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_thassarian(Player *player, Creature *pCreature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
          ((npc_thassarianAI*)pCreature->AI())->StartEvent(1);
          player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

CreatureAI* GetAI_npc_valanar(Creature* pCreature)
{
    return new npc_valanarAI(pCreature);
}

/*######
## npc_mootoo_the_younger
######*/
enum Mootoo_the_Younger_Texts
{
    SAY_1                    = -1750040,
    SAY_2                    = -1750041,
    SAY_3                    = -1750042,
    SAY_4                    = -1750043,
    SAY_5                    = -1750044
};
enum Mootoo_the_Younger
{
    NPC_MOOTOO_THE_YOUNGER          =25504,
    QUEST_ESCAPING_THE_MIST         =11664
};


struct MANGOS_DLL_DECL npc_mootoo_the_youngerAI : public npc_escortAI
{
        npc_mootoo_the_youngerAI(Creature *pCreature) : npc_escortAI(pCreature) {}

        void Reset(){}

        void JustDied(Unit* /*killer*/)
        {
            if (Player* pPlayer=GetPlayerForEscort())
                pPlayer->FailQuest(QUEST_ESCAPING_THE_MIST);
        }

        void WaypointReached(uint32 uiPointId)
        {
            Player* pPlayer = GetPlayerForEscort();

            if (!pPlayer)
                return;

            switch(uiPointId)
            {
            case 10:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                DoScriptText(SAY_2, m_creature);
                break;
            case 12:
                DoScriptText(SAY_3, m_creature);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LOOT);
                break;
            case 16:
                DoScriptText(SAY_4, m_creature);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                break;
            case 20:
                m_creature->SetPhaseMask(1,true);
                DoScriptText(SAY_5, m_creature);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                if (pPlayer)
                    pPlayer->GroupEventHappens(QUEST_ESCAPING_THE_MIST, m_creature);
                SetRun(true);
                break;
            }
        }
};

bool QuestAccept_npc_mootoo_the_younger(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPING_THE_MIST)
    {
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);
        pCreature->setFaction((pPlayer->GetTeam() == ALLIANCE) ? FACTION_ESCORT_A_PASSIVE : FACTION_ESCORT_H_PASSIVE);
        DoScriptText(SAY_1, pCreature);

		if (npc_mootoo_the_youngerAI* pEscortAI = dynamic_cast<npc_mootoo_the_youngerAI*>(pCreature->AI()))
            	pEscortAI->Start(false, pPlayer, pQuest, true);

    }
    return true;
}

CreatureAI* GetAI_npc_mootoo_the_younger(Creature* pCreature)
{
    return new npc_mootoo_the_youngerAI(pCreature);
}

/*######
## npc_bonker_togglevolt
######*/

enum Bonker_Togglevolt
{
    NPC_BONKER_TOGGLEVOLT           = 25589,
    QUEST_GET_ME_OUTA_HERE          = 11673
};
enum Bonker_Togglevolt_Texts
{
	SAY_bonker_1					= -1700001,
    SAY_bonker_2                   	= -1700002,
    SAY_bonker_3                    = -1700003
	
};

struct MANGOS_DLL_DECL npc_bonker_togglevoltAI : public npc_escortAI
    {
        npc_bonker_togglevoltAI(Creature *pCreature) : npc_escortAI(pCreature) {}
        
        void Reset(){}

        void JustDied(Unit* /*killer*/)
        {
            if (Player* pPlayer = GetPlayerForEscort())
                pPlayer->FailQuest(QUEST_ESCAPING_THE_MIST);
        }

        void WaypointReached(uint32 uiPointId)
        {
            Player* pPlayer = GetPlayerForEscort();

            if (!pPlayer)
                return;

            switch(uiPointId)
            {
			case 1:
				DoScriptText(SAY_bonker_1,m_creature);
				break;
			case 3:
				DoScriptText(SAY_bonker_2,m_creature);
				break;
            case 29:
				DoScriptText(SAY_bonker_3,m_creature);
                if (pPlayer)
                    pPlayer->GroupEventHappens(QUEST_GET_ME_OUTA_HERE, m_creature);
                break;
            }
        }
    };

    

bool QuestAccept_npc_bonker_togglevolt(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
    {
        if (pQuest->GetQuestId() == QUEST_GET_ME_OUTA_HERE)
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
	        pCreature->setFaction((pPlayer->GetTeam() == ALLIANCE) ? FACTION_ESCORT_A_PASSIVE : FACTION_ESCORT_H_PASSIVE);            
	        DoScriptText(SAY_bonker_2, pCreature, pPlayer);
            
			if (npc_bonker_togglevoltAI* pEscortAI = dynamic_cast<npc_bonker_togglevoltAI*>(pCreature->AI()))
            		pEscortAI->Start(false, pPlayer, pQuest, true);
        }
        return true;
    }

CreatureAI* GetAI_npc_bonker_togglevolt(Creature* pCreature)
{
    return new npc_bonker_togglevoltAI(pCreature);
}

/*######
##Bury Those Cockroaches!
######*/
enum
{
    QUEST_BURY_THOSE_COCKROACHES            = 11608,
    SPELL_SEAFORIUM_DEPTH_CHARGE_EXPLOSION  = 45502


};
struct npc_seaforium_depth_chargeAI : public ScriptedAI
{
    npc_seaforium_depth_chargeAI(Creature *pCreature) : ScriptedAI(pCreature) {}

    uint32 uiExplosionTimer;
    void Reset()
    {
        uiExplosionTimer = urand(5000,10000);
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if (uiExplosionTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_SEAFORIUM_DEPTH_CHARGE_EXPLOSION);          
            for(uint8 i = 0; i < 4; ++i)
            {
                if(Creature* cCredit = GetClosestCreatureWithEntry(m_creature, 25402 + i, 10.0f))//25402-25405 credit markers
                {
                    if(Player *pPlayer = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid()))
                    {
                        if(pPlayer->GetQuestStatus(QUEST_BURY_THOSE_COCKROACHES) == QUEST_STATUS_INCOMPLETE)
                            pPlayer->KilledMonsterCredit(cCredit->GetEntry(),cCredit->GetObjectGuid());
                    }                    
                }
            }
            m_creature->ForcedDespawn(1000);
        } else uiExplosionTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_seaforium_depth_charge(Creature* pCreature)
{
    return new npc_seaforium_depth_chargeAI(pCreature);
}


/*######
##En'kilah Necrolord
######*/
enum
{
    OVERLORD_SAURFANG_MODEL_ID = 23360,
    OVERLORD_SAURFANG_ATTACKER_MODEL_ID = 23033,
    SAURFANGS_AXE_ID = 366,

    ENKILAH_NECROLORD_ID = 25730,
    VARIDUS_ID = 25618,
    SHADOWSTALKER_GETRY_ID = 25729,

    SPELL_SAURFANG_RAGE = 45950,
    SPELL_SAURFANG_SHIELD_CHARGE = 15749,
    SPELL_SAURFANG_CLEAVE = 16044,
    SPELL_SAURFANG_MORTAL_STRIKE = 24573,
    SPELL_SAURFANG_WHIRLWIND = 41097,
    SPELL_SAURFANG_TERYFING_ROAR = 14100,
    
    ENKILAH_NECROLORD_SAY1 = -1999970,
    ENKILAH_NECROLORD_SAY2 = -1999971,
    ENKILAH_NECROLORD_SAY3 = -1999972,
    ENKILAH_NECROLORD_SAY4 = -1999973,
    ENKILAH_NECROLORD_SAY5 = -1999974,
    ENKILAH_NECROLORD_SAY6 = -1999975,
    ENKILAH_NECROLORD_SAY7 = -1999976,
};

enum PHASES
{
    PHASE_ZERO = 0,
    PHASE_ONE = 1,
    PHASE_TWO = 2,
    PHASE_THREE = 3,
    PHASE_FOUR = 4,
    PHASE_FIVE = 5
};

const float ENKILAH_NECROLORD_SPAWN_COORDINATES[3] = {3132.2980f, 6540.0517f, 80.141441f};

struct npc_enkilah_necrolordAI : public ScriptedAI
{

    Creature *pCreature;
    Player *pPlayer;
    Creature *pVaridus;
    uint8 m_uiPhase;
    uint8 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 mortalStrikeTimer;
    uint32 whirlwindTimer;
    uint32 teryfingRoarTimer;
    uint32 cleaveTimer;


    npc_enkilah_necrolordAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_uiStepTimer = 0;
        pPlayer = 0;
        this->pCreature = pCreature;
        m_uiPhase = PHASE_TWO;
        m_uiStep = 0;
        mortalStrikeTimer = urand(6000,11000); // initial spell cast
        cleaveTimer = urand(7000,11000); 
        whirlwindTimer = urand(4000, 9000); 
        teryfingRoarTimer = urand(15000, 23000);
    }

    
    void Reset()
    {
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(m_uiPhase == PHASE_TWO)
            m_creature->SetFacingToObject(pPlayer);

        if(m_uiPhase == PHASE_FOUR)
            NextStep();
    }

    void NextStep(uint32 Time = 0) 
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

	void Spacer()
	{
		m_uiStepTimer = 1000000; 
	}

    void setPhase(PHASES phase, uint32 stepTimer = 0)
    {
        m_uiStepTimer = stepTimer; 
        m_uiStep = 1;
        m_uiPhase = phase;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiStepTimer < uiDiff)
		{
			if(m_uiPhase == PHASE_TWO)  
			{
				switch( m_uiStep) 
				{
					case 1:
                        pCreature->SetWalk(true);
                        if((pPlayer->GetPositionX() > 3105 && pPlayer->GetPositionX() < 3144) && //only walk to player if he is in range else walk to getry
                           (pPlayer->GetPositionY() > 6536 && pPlayer->GetPositionY() < 6589) &&
                           (pPlayer->GetPositionZ() > 74   && pPlayer->GetPositionZ() < 81))
                                pCreature->GetMotionMaster()->MovePoint(1, pPlayer->GetPositionX(), pPlayer->GetPositionY() - 4,
                                pPlayer->GetPositionZ());
                        else
                            pCreature->GetMotionMaster()->MovePoint(1, 3122.8056f, 6549.4526f, 79.422905f);
                       
                        NextStep(35*IN_MILLISECONDS);
					    break;

                    case 2:
                        if(Creature* pVaridus = GetClosestCreatureWithEntry(m_creature, VARIDUS_ID, 100.0f)) 
                            m_creature->SetFacingToObject(pVaridus);
                        NextStep(1*IN_MILLISECONDS);
                        break;
                     
                    case 3:
                        m_creature->SetDisplayId(OVERLORD_SAURFANG_MODEL_ID);
                        m_creature->SetName("High Overlord Saurfang");
                        NextStep(1*IN_MILLISECONDS);
                        break;
                     //morphing to saurfang

                    case 4:
                        DoScriptText(ENKILAH_NECROLORD_SAY1,m_creature); //surprise!!!
                        m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        NextStep(2*IN_MILLISECONDS);
                        break;

                    case 5:
                        if(pPlayer)
                            m_creature->SetFacingToObject(pPlayer);
                        NextStep(1*IN_MILLISECONDS);
                        break;

                    case 6:
                        if(pPlayer)
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_TALK);                      
                            DoScriptText(ENKILAH_NECROLORD_SAY2,m_creature, pPlayer); //You were never alone %N!
                        }
                        NextStep(4*IN_MILLISECONDS);
                        break;

                    case 7:
                        if(Creature* pVaridus = GetClosestCreatureWithEntry(m_creature, VARIDUS_ID, 100.0f)) 
                            m_creature->SetFacingToObject(pVaridus);
                        NextStep(1*IN_MILLISECONDS);
                        break;

                    case 8:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        DoScriptText(ENKILAH_NECROLORD_SAY3,m_creature); //This world you seek to...
                        NextStep(3*IN_MILLISECONDS);
                        break;

                    case 9:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        DoScriptText(ENKILAH_NECROLORD_SAY4,m_creature); //We will fight you...
                        NextStep(13*IN_MILLISECONDS);
                        break;

                    case 10:
                        m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                        DoScriptText(ENKILAH_NECROLORD_SAY5,m_creature); //A fool who is about to end you...
                        NextStep(6*IN_MILLISECONDS);
                        break;

                    case 11:
                        m_creature->SetDisplayId(OVERLORD_SAURFANG_ATTACKER_MODEL_ID);
                        m_creature->LoadEquipment(SAURFANGS_AXE_ID);
                        NextStep(2*IN_MILLISECONDS);
                        break;

                    case 12:
                        DoCastSpellIfCan(pCreature,SPELL_SAURFANG_RAGE,false);
                        NextStep(3.7*IN_MILLISECONDS);
                        break;

                    case 13:
                        if(pVaridus = GetClosestCreatureWithEntry(m_creature, VARIDUS_ID, 100.0f))
                            DoCastSpellIfCan(pVaridus, SPELL_SAURFANG_SHIELD_CHARGE, false);   
                        NextStep();
                        break;

                    case 14:
                        setPhase(PHASE_THREE);
                        break;                    
				}
			}
            else if(m_uiPhase == PHASE_THREE)
            {
                if(pVaridus) 
                {
                    if(pVaridus->isDead())
                    {
                        setPhase(PHASE_FOUR);
                        return;
                    }
                 }        
                   
                if(mortalStrikeTimer < uiDiff)
                {
                    if(whirlwindTimer >= 18000) //if WW didn't end do...
                        mortalStrikeTimer = whirlwindTimer - 18000 + urand(0,6000);
                    else
                    {
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_SAURFANG_MORTAL_STRIKE);
                        mortalStrikeTimer = urand(6000, 12000);  
                    }
                }
                else
                    mortalStrikeTimer -= uiDiff; 

                if(cleaveTimer < uiDiff)
                {
                    if(whirlwindTimer >= 18000) 
                        cleaveTimer = whirlwindTimer - 18000 + urand(0,4000);
                    else
                    {
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_SAURFANG_CLEAVE);
                        cleaveTimer = urand(7000, 11000);  
                    }
                }
                else
                    cleaveTimer -= uiDiff; 
                
                if(whirlwindTimer < uiDiff) 
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SAURFANG_WHIRLWIND);
                    whirlwindTimer = 33000;  
                }
                else 
                    whirlwindTimer -= uiDiff;
                                                    
                /*if(teryfingRoarTimer < uiDiff) 
                {
                    if(whirlwindTimer >= 18000) 
                        teryfingRoarTimer = whirlwindTimer - 18000 + urand(0,6000);
                    else
                    {
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_SAURFANG_TERYFING_ROAR);
                        teryfingRoarTimer = urand(18000, 24000);   
                    }
                }
                else
                    teryfingRoarTimer -= uiDiff;*/ 

                //Teryfing roar somehow is making Varidus not be able to autoattack
                

                DoMeleeAttackIfReady();
            }

            else if(m_uiPhase == PHASE_FOUR)
            {
                switch(m_uiStep)
                {
                    case 1: //Get back to talk with Getry
                        pCreature->SetWalk(false);                      
                        pCreature->GetMotionMaster()->MovePoint(2, 3122.8056f, 6549.4526f, 79.422905f);
                        Spacer();
                        break;

                    case 2:
                        if(Creature* pGetry = GetClosestCreatureWithEntry(m_creature, SHADOWSTALKER_GETRY_ID, 100.0f))
                            pCreature->SetFacingToObject(pGetry);
                        NextStep(16*IN_MILLISECONDS);
                        break;

                    case 3:
                        DoScriptText(ENKILAH_NECROLORD_SAY7, pCreature);
                        NextStep(2*IN_MILLISECONDS);
                        break;

                    case 4:
                        pCreature->GetMotionMaster()->MovePoint(3, ENKILAH_NECROLORD_SPAWN_COORDINATES[0],
                            ENKILAH_NECROLORD_SPAWN_COORDINATES[1], ENKILAH_NECROLORD_SPAWN_COORDINATES[2]); 
                        Spacer();
                        break;

                    case 5:
                        pCreature->ForcedDespawn();
                        break;
                }
            }
		}
		else 
		{
            if((m_uiPhase == PHASE_TWO) || (m_uiPhase == PHASE_THREE) || (m_uiPhase == PHASE_FOUR))
			    m_uiStepTimer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_enkilah_necrolord(Creature* pCreature)
{
    return new npc_enkilah_necrolordAI(pCreature);
}


/*######
##Varidus the Flenser
######*/

enum
{
    WARSONG_ABERATION_ID = 25625,

    SPEL_RELEASE_ABERATION = 45805,
    SPEL_SHADOW_NOVA = 32711,
    SPEL_SHIELD_OF_SUFFERING = 50329,

    VARIDUS_THE_FLENSER_SAY1 = -1999950,
    VARIDUS_THE_FLENSER_SAY2 = -1999951,
    VARIDUS_THE_FLENSER_SAY3 = -1999952,
    VARIDUS_THE_FLENSER_SAY4 = -1999953,
    VARIDUS_THE_FLENSER_SAY5 = -1999954,
    VARIDUS_THE_FLENSER_SAY6 = -1999955,
    VARIDUS_THE_FLENSER_SAY7 = -1999956,
    VARIDUS_THE_FLENSER_SAY8 = -1999957,
    VARIDUS_THE_FLENSER_SAY9 = -1999958,
    VARIDUS_THE_FLENSER_SAY10 = -1999959,
    VARIDUS_THE_FLENSER_SAY11 = -1999960,
    VARIDUS_THE_FLENSER_SAY12 = -1999977,
    VARIDUS_THE_FLENSER_SAY13 = -1999978,
    VARIDUS_THE_FLENSER_SAY14 = -1999979,

    INFESTED_PRISONER_COORDINATES_SIZE = 22
};

const float INFESTED_PRISONER_COORDINATES[INFESTED_PRISONER_COORDINATES_SIZE][3] =
{
    {3122.21f,6510.26f ,81.1079f },
    {3116.76f,6524.07f ,80.9485f },
    {3114.64f, 6501.5f ,81.7178f },
    {3105.6f, 6534.98f, 80.9596f },
    {3122.1f, 6518.81f, 80.8091f },
    {3112.07f, 6511.91f, 81.4951f},
    {3112.36f, 6527.99f, 80.9751f},
    {3129.41f, 6511.59f, 80.8066f},
    {3108.6f, 6531.9f, 80.9754f},
    {3098.93f, 6517.5f, 81.875f},
    {3093.04f, 6547.07f, 80.8595f},
    {3116.41f, 6507.34f, 81.4439f},
    {3093.24f, 6538.74f, 81.1652f},
    {3104.28f, 6528.08f, 81.2888f},
    {3099.79f, 6524.31f, 81.6106f},
    {3096.96f, 6510.87f, 82.1854f},
    {3093.88f, 6522.22f, 81.8956f},
    {3102.45f, 6538.24f, 80.9342f},
    {3103.04f, 6504.89f, 82.1467f},
    {3126.8f, 6505.61f, 81.113f},
    {3104.28f, 6519.58f, 81.6076f},
    {3096.46f, 6535.53f, 81.2335f}
};

struct npc_varidus_the_flenserAI : public ScriptedAI
{
	uint8 m_uiStep;
	uint8 m_uiPhase;
	uint32 m_uiStepTimer;
    uint16 m_uiShadowNovaTimer;
    uint16 m_uiShieldOfSuferingTimer;
	Player* pPlayer;
    Creature* pGetry;
    std::list<Creature*> abberationList;

    npc_varidus_the_flenserAI(Creature *pCreature) : ScriptedAI(pCreature) 
	{
		m_uiStep = 0;
		m_uiPhase = 0;
		m_uiStepTimer = 0;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pGetry = 0;
        m_uiShadowNovaTimer = 7000;
        m_uiShieldOfSuferingTimer = 12000;
	}

    void Reset()
    {
        m_uiStep = 0;
		m_uiPhase = 0;
		m_uiStepTimer = 0;
        pGetry = 0;   
        abberationList.clear();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustDied(Unit* pKiller)
    {
        pPlayer->KilledMonsterCredit(VARIDUS_ID);
    }

	void NextStep(uint32 Time = 0) // function for changing action steps and setting time between steps
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

	void Spacer()
	{
		m_uiStepTimer = 1000000; // setting large timer number this way creature will not make same action over again
	}

    void Kill(Unit* Oo)
    {
        Oo->DealDamage(Oo, Oo->GetMaxHealth() ,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void setPhase(PHASES phase, uint32 stepTimer = 0)
    {
        m_uiStepTimer = stepTimer; 
        m_uiStep = 1;
        m_uiPhase = phase;
    }

    void UpdateAI(const uint32 uiDiff)
    {        
		if(m_uiStepTimer < uiDiff)
		{
            if(m_uiPhase == PHASE_ZERO)
            {
                switch(urand(0,2))
                {
                    case 0:
                        DoScriptText( VARIDUS_THE_FLENSER_SAY12, m_creature);
                        break;

                    case 1:
                        DoScriptText( VARIDUS_THE_FLENSER_SAY13, m_creature);
                        break;

                    case 2:
                        DoScriptText( VARIDUS_THE_FLENSER_SAY14, m_creature);
                        break;
                }

                m_uiStepTimer = 30000;
            }

			if(m_uiPhase == PHASE_TWO)  // First phase action block
			{
				switch(m_uiStep) 
				{
					case 1: //"SAY: Is this it? Is this all mighty horde..."
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
					    DoScriptText(VARIDUS_THE_FLENSER_SAY1, m_creature); 
					    NextStep(2*IN_MILLISECONDS);
					    break;

					case 2:
                        if(Creature* pGetry = GetClosestCreatureWithEntry(m_creature, SHADOWSTALKER_GETRY_ID, 100.0f))
                            m_creature->SetFacingToObject(pGetry);  
                        else
                        {
                            Reset();
                            return;
                        }
					    NextStep(1*IN_MILLISECONDS);
                        break;

					case 3:  //"SAY: Pathetic!!"
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
					    DoScriptText(VARIDUS_THE_FLENSER_SAY2, m_creature); // Pathetic
					    Spacer();
					    break;

					case 4:
					    m_creature->HandleEmote(EMOTE_ONESHOT_SPELLCAST);
					    NextStep(10*IN_MILLISECONDS);
					    break;

					case 5:  //SAY: i've never understood your type the hero
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
					    DoScriptText(VARIDUS_THE_FLENSER_SAY3, m_creature);
                        NextStep(5*IN_MILLISECONDS);
					    break;

                    case 6: //SAY: why don't you let go?
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
					    DoScriptText(VARIDUS_THE_FLENSER_SAY4, m_creature); 
                        NextStep(4*IN_MILLISECONDS);
					    break;

                    case 7: //SAY: Arthas can not be stoped accept it 
                        m_creature->HandleEmote(EMOTE_ONESHOT_NO);
					    DoScriptText(VARIDUS_THE_FLENSER_SAY5, m_creature); 
                        NextStep(3*IN_MILLISECONDS);
					    break;

                    case 8: //SAY: This world is coming to an end, let it burn
                        m_creature->HandleEmote(EMOTE_ONESHOT_POINT );
                        DoScriptText(VARIDUS_THE_FLENSER_SAY6, m_creature); 
                        NextStep(6*IN_MILLISECONDS);
					    break;

                    case 9:
                        if(Creature* pNecroLord = GetClosestCreatureWithEntry(m_creature, ENKILAH_NECROLORD_ID, 100.0f))
                            m_creature->SetFacingToObject(pNecroLord);                                                                              
                        NextStep(1*IN_MILLISECONDS);
                        break;

                    case 10: //SAY: Take him and prepare for reanimation
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        DoScriptText(VARIDUS_THE_FLENSER_SAY7, m_creature); 
                        NextStep(8*IN_MILLISECONDS);
					    break;

                    case 11: // SAY: Whats this now
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        DoScriptText(VARIDUS_THE_FLENSER_SAY8, m_creature); 
                        NextStep(20*IN_MILLISECONDS);
                        break;

                    case 12: //SAY: Then you are a fool
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        DoScriptText(VARIDUS_THE_FLENSER_SAY9, m_creature); 
                        NextStep(7*IN_MILLISECONDS);
                        break;

                    case 13:
                        m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);
                        NextStep(3*IN_MILLISECONDS);
                        break;

                    case 14:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        DoScriptText(VARIDUS_THE_FLENSER_SAY10, m_creature);
                        NextStep(2*IN_MILLISECONDS);
                        break;

                    case 15:
                        m_creature->HandleEmote(EMOTE_ONESHOT_POINT); 
                        NextStep(2*IN_MILLISECONDS);
                        break;

                    case 16: 
                        DoScriptText(VARIDUS_THE_FLENSER_SAY11, m_creature);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); //makes Varidus attackable
                        m_creature->RemoveAllAuras();

                        while(Creature* pInfestedPrisoner = GetClosestCreatureWithEntry(m_creature, 25624, 200.0f))
                        {
                            Kill(pInfestedPrisoner);
                        }

                        for(int i=0; i<INFESTED_PRISONER_COORDINATES_SIZE; i++)
                        { //Summon lots of aberations  
                            abberationList.push_back(m_creature->SummonCreature(WARSONG_ABERATION_ID,INFESTED_PRISONER_COORDINATES[i][0],
                                INFESTED_PRISONER_COORDINATES[i][1], INFESTED_PRISONER_COORDINATES[i][2],0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000));
                        }

                        NextStep(2*IN_MILLISECONDS);
                        break;


                    case 17:
                        if(Creature* pNecroLord = GetClosestCreatureWithEntry(m_creature, ENKILAH_NECROLORD_ID, 100.0f))
                        {   
                            std::list<Creature*>::iterator it;
                    
                            for(it = abberationList.begin(); it != abberationList.end(); it++)
                            {
                                (*it)->SetWalk(false);
                                (*it)->AI()->AttackStart(pNecroLord);
                            }
                        }
                        NextStep();
                        break;

                    case 18:  
                        if(Creature* pNecroLord = GetClosestCreatureWithEntry(m_creature, ENKILAH_NECROLORD_ID, 100.0f))
                        {
                            AttackStart(pNecroLord);
                            setPhase(PHASE_THREE);  
                        }
                        break;
		        }
			}
            else if(m_uiPhase == PHASE_THREE)
            {
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if(m_uiShadowNovaTimer < uiDiff) 
                {                    
                    DoCastSpellIfCan(m_creature, SPEL_SHADOW_NOVA);
                    m_uiShadowNovaTimer = 12000;                    
                }
                else
                    m_uiShadowNovaTimer -= uiDiff;

                if(m_uiShieldOfSuferingTimer < uiDiff) 
                {                    
                    DoCastSpellIfCan(m_creature, SPEL_SHIELD_OF_SUFFERING);
                    m_uiShieldOfSuferingTimer = 28000;                    
                }
                else
                    m_uiShieldOfSuferingTimer -= uiDiff;
               
                DoMeleeAttackIfReady(); //TODO: What if Varidus would be taunted and taken away from battlefield?? :/
                //TODO: Varidus sometimes get's buged in a battle
            }
		}
		else 
		{
            if((m_uiPhase == PHASE_ZERO) || (m_uiPhase == PHASE_TWO) || (m_uiPhase == PHASE_THREE))
			    m_uiStepTimer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_varidus_the_flenser(Creature* pCreature)
{
    return new npc_varidus_the_flenserAI(pCreature);
}

/*######
##Shadowstalker Getry
######*/
enum
{
    QUEST_FOOLISH_ENDEAVORS = 11705,
	WAYPOINT_QUANTITY = 11,
    GESTRYS_DAGGERS_ID = 428,

    SHADOWSTALKER_GETRY_SAY1 = -1999940,
    SHADOWSTALKER_GETRY_SAY2 = -1999941,
    SHADOWSTALKER_GETRY_SAY3 = -1999942,
    SHADOWSTALKER_GETRY_SAY4 = -1999943,
    SHADOWSTALKER_GETRY_SAY5 = -1999944

};

const float GETRY_WAYPOINTS[WAYPOINT_QUANTITY][3] =
{
    {3120.1799f, 6574.9199f, 97.868599f},
    {3115.7080f, 6575.7163f, 97.977615f},
    {3108.7746f, 6586.3958f, 91.500740f},
	{3114.2470f, 6596.2807f, 91.339890f},
	{3124.5117f, 6590.1328f, 91.379028f},
	{3125.5366f, 6582.6499f, 88.812263f},
	{3117.5932f, 6582.4340f, 86.207214f},
	{3117.2939f, 6590.1330f, 83.417160f},
	{3126.9267f, 6590.5400f, 79.770805f}, 
	{3126.1733f, 6578.5327f, 77.960869f},
	{3112.8618f, 6555.6508f, 79.744034f}
};

struct npc_shadowstalker_getryAI : public ScriptedAI
{	
	uint8 m_uiStep;
	uint8 m_uiPhase;
	uint32 m_uiStepTimer;
    bool eventStarted;
	Player* pPlayer;
	Creature* Enkilah_Necrolord;
    Creature* pVaridus;

    npc_shadowstalker_getryAI(Creature *pCreature) : ScriptedAI(pCreature) 
	{
		m_uiStep = 1;
		m_uiPhase = 0;
		uint32 m_uiStepTimer = 0;
        eventStarted = false;
        Enkilah_Necrolord = 0;
        pPlayer = 0;
        pVaridus = 0;
	}
    
    void Reset()
    {
        m_creature->SetWalk(true);
        m_uiStep = 1;
		m_uiPhase = 0;
		m_uiStepTimer = 0;
        Enkilah_Necrolord = 0;   
        pVaridus = 0;
        eventStarted = false;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }
    
	void MovementInform(uint32 uiType, uint32 uiPointId) // function called at the end of some walk
    {
        NextStep();
    }

	 void NextStep(uint32 Time = 0) // function for changing action steps and setting time between steps
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

	void Spacer()
	{
		m_uiStepTimer = 1000000; // setting large timer number this way creature will not maek same action... 
								// ...over and over again ... little bit ugly i know 
	}

    void setPhase(PHASES phase, uint32 stepTimer = 0)
    {
        m_uiStepTimer = stepTimer; 
        m_uiStep = 1;
        m_uiPhase = phase;
    }

    void StartEventForPlayer(Player* player)
    {
        if(!eventStarted)
        {
            this->pPlayer = player;
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_uiPhase = PHASE_ONE;
            eventStarted = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if(m_uiStepTimer < uiDiff)
		{
			if(m_uiPhase == PHASE_ONE)  // First phase action block
			{
				switch( m_uiStep) 
				{
					case 1:   //SAY: "lets go"
					    DoScriptText(SHADOWSTALKER_GETRY_SAY1, m_creature); 
					    NextStep(5*IN_MILLISECONDS);
					    break;

					case 2:  
					    m_creature->GetMotionMaster()->MovePoint(1, GETRY_WAYPOINTS[1][0], GETRY_WAYPOINTS[1][1], GETRY_WAYPOINTS[1][2]);
					    Spacer();
					    break;

					case 3: 
					    m_creature->GetMotionMaster()->MovePoint(2, GETRY_WAYPOINTS[2][0], GETRY_WAYPOINTS[2][1], GETRY_WAYPOINTS[2][2]);
					    Spacer();
					    break;

					case 4: 
                        if(pPlayer)
					        DoScriptText(SHADOWSTALKER_GETRY_SAY2, m_creature, pPlayer);

					    m_creature->GetMotionMaster()->MovePoint(3, GETRY_WAYPOINTS[3][0], GETRY_WAYPOINTS[3][1], GETRY_WAYPOINTS[3][2]);
					    Spacer();
					    break;

					case 5:
					    m_creature->GetMotionMaster()->MovePoint(4, GETRY_WAYPOINTS[4][0], GETRY_WAYPOINTS[4][1], GETRY_WAYPOINTS[4][2]);
					    Spacer();
					    break;

					case 6:
					    m_creature->GetMotionMaster()->MovePoint(5, GETRY_WAYPOINTS[5][0], GETRY_WAYPOINTS[5][1], GETRY_WAYPOINTS[5][2]);
					    Spacer();
					    break;

					case 7:
					    m_creature->GetMotionMaster()->MovePoint(6, GETRY_WAYPOINTS[6][0], GETRY_WAYPOINTS[6][1], GETRY_WAYPOINTS[6][2]);
					    Spacer();
					    break;

					case 8:
					    m_creature->GetMotionMaster()->MovePoint(7, GETRY_WAYPOINTS[7][0], GETRY_WAYPOINTS[7][1], GETRY_WAYPOINTS[7][2]);
					    Spacer();
					    break;

					case 9:
					    m_creature->GetMotionMaster()->MovePoint(8, GETRY_WAYPOINTS[8][0], GETRY_WAYPOINTS[8][1], GETRY_WAYPOINTS[8][2]);
					    Spacer();
					    break;

					case 10:
					    m_creature->GetMotionMaster()->MovePoint(9, GETRY_WAYPOINTS[9][0], GETRY_WAYPOINTS[9][1], GETRY_WAYPOINTS[9][2]);
					    Spacer();
					    break;

					case 11:  // Getry is turning on stealth
					    DoCast(m_creature,34189);
					    NextStep(0.1*IN_MILLISECONDS);
					    break;       

					case 12: // Getry is sneaking up to attack Varidus here 
					    m_creature->GetMotionMaster()->MovePoint(10, GETRY_WAYPOINTS[10][0], GETRY_WAYPOINTS[10][1], GETRY_WAYPOINTS[10][2]);
					    if(pVaridus = GetClosestCreatureWithEntry(m_creature, 25618, 200.0f)) 
					    {  
						    npc_varidus_the_flenserAI* pVaridusAI = dynamic_cast<npc_varidus_the_flenserAI*>(pVaridus->AI()); // need pointer to start second phase 
							
                            pVaridusAI->setPhase(PHASE_TWO, 6*IN_MILLISECONDS);

						    pVaridusAI->pPlayer = pPlayer;
                            pVaridusAI->pGetry = m_creature;

                            Spacer();
					    }
                        else
                            setPhase(PHASE_FIVE);
                        
					    break;

					
					case 13: 
                        setPhase(PHASE_TWO);
					    break;
				} 
			}

			else if(m_uiPhase == PHASE_TWO)
			{
				switch(m_uiStep)
				{
					case 1:
					    if(pVaridus) 
					    {
						    npc_varidus_the_flenserAI* pVaridusAI = dynamic_cast<npc_varidus_the_flenserAI*>(pVaridus->AI()); 
						    pVaridusAI->NextStep();
					    }
                        // Summon enk'ilah Necrolord
					    Enkilah_Necrolord =  m_creature->SummonCreature(ENKILAH_NECROLORD_ID, 
					    ENKILAH_NECROLORD_SPAWN_COORDINATES[0], ENKILAH_NECROLORD_SPAWN_COORDINATES[1],
					    ENKILAH_NECROLORD_SPAWN_COORDINATES[2], 0, TEMPSUMMON_TIMED_DESPAWN, 10000000);

                        if(Enkilah_Necrolord)
                        {
                            npc_enkilah_necrolordAI* pNecrolordAI = dynamic_cast<npc_enkilah_necrolordAI*>(Enkilah_Necrolord->AI());
                            pNecrolordAI->pPlayer = pPlayer;
                            pNecrolordAI->NextStep(1*IN_MILLISECONDS);
                        }
                    
					    NextStep(0.6*IN_MILLISECONDS);
					    break;

					case 2:
					    DoCast(m_creature, 45922);			// since shadow prison is only self cast, player and 
					    m_creature->RemoveAllAuras();		// Getry must cast it on them selfs
                        if(pPlayer)
					        pPlayer->CastSpell(pPlayer, 45922, false);
				        NextStep(2*IN_MILLISECONDS);
					    break;	

                    case 3:
                        if(!m_creature->HasAura(45922)) 
                        {
                            m_creature->LoadEquipment(GESTRYS_DAGGERS_ID);
                            
                            if(Creature* pVaridus = GetClosestCreatureWithEntry(m_creature, 25618, 100.0f)) 
                                AttackStart(pVaridus);

                            setPhase(PHASE_THREE);
                        } 
                        else
                        {
                            m_uiStepTimer = 1000;
                        }                   
                    break;
		        }
    
			} 
            else if(m_uiPhase == PHASE_THREE)
			{
                if(GetClosestCreatureWithEntry(m_creature, VARIDUS_ID, 200.0f) == 0)
                {
                    setPhase(PHASE_FOUR);
                    return;
                }
                else
                    DoMeleeAttackIfReady();
            }
            else if(m_uiPhase == PHASE_FOUR)
            {
                switch(m_uiStep)
                {
                    case 1:
                        m_creature->LoadEquipment(0, true);
                        m_creature->SetWalk(false);
                        m_creature->GetMotionMaster()->MovePoint(10, GETRY_WAYPOINTS[10][0], GETRY_WAYPOINTS[10][1], GETRY_WAYPOINTS[10][2]);
                        Spacer();
                        break;

                    case 2:
                        NextStep(3*IN_MILLISECONDS);
                        break;

                    case 3:
                        if(Creature* pNecroLord = GetClosestCreatureWithEntry(m_creature,ENKILAH_NECROLORD_ID , 200.0f)) 
                        {
                            m_creature->SetFacingToObject(pNecroLord);
                        }
                        else
                        {
                            setPhase(PHASE_FIVE);
                            return;
                        }
                        NextStep(2*IN_MILLISECONDS);
                        break;

                    case 4:
                        DoScriptText(SHADOWSTALKER_GETRY_SAY3, m_creature);
                        m_creature->HandleEmote(EMOTE_ONESHOT_BOW);
                        NextStep(5*IN_MILLISECONDS);
                        break;

                    case 5:
                        DoScriptText(SHADOWSTALKER_GETRY_SAY4, m_creature);
                        NextStep(9*IN_MILLISECONDS);
                        break;

                    case 6:
                        DoScriptText(SHADOWSTALKER_GETRY_SAY5, m_creature);
                        m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                        NextStep(7*IN_MILLISECONDS);
                        break;

                    case 7:
                        setPhase(PHASE_FIVE);
                        break;                   
                }
            }
            else if(m_uiPhase == PHASE_FIVE)
            {
                switch(m_uiStep)
                {
                    case 1:
                        m_creature->SetWalk(false);
					    m_creature->GetMotionMaster()->MovePoint(11, GETRY_WAYPOINTS[9][0], GETRY_WAYPOINTS[9][1], GETRY_WAYPOINTS[9][2]);
					    Spacer();
					    break;

                    case 2:
					    m_creature->GetMotionMaster()->MovePoint(12, GETRY_WAYPOINTS[8][0], GETRY_WAYPOINTS[8][1], GETRY_WAYPOINTS[8][2]);
					    Spacer();
					    break;

                    case 3:
					    m_creature->GetMotionMaster()->MovePoint(13, GETRY_WAYPOINTS[7][0], GETRY_WAYPOINTS[7][1], GETRY_WAYPOINTS[7][2]);
					    Spacer();
					    break;

                    case 4:
					    m_creature->GetMotionMaster()->MovePoint(14, GETRY_WAYPOINTS[6][0], GETRY_WAYPOINTS[6][1], GETRY_WAYPOINTS[6][2]);
					    Spacer();
					    break;

                    case 5:
					    m_creature->GetMotionMaster()->MovePoint(15, GETRY_WAYPOINTS[5][0], GETRY_WAYPOINTS[5][1], GETRY_WAYPOINTS[5][2]);
					    Spacer();
					    break;

                    case 6:
					    m_creature->GetMotionMaster()->MovePoint(16, GETRY_WAYPOINTS[4][0], GETRY_WAYPOINTS[4][1], GETRY_WAYPOINTS[4][2]);
					    Spacer();
					    break;

                    case 7:
					    m_creature->GetMotionMaster()->MovePoint(17, GETRY_WAYPOINTS[3][0], GETRY_WAYPOINTS[3][1], GETRY_WAYPOINTS[3][2]);
					    Spacer();
					    break;

                    case 8: 
					    m_creature->GetMotionMaster()->MovePoint(18, GETRY_WAYPOINTS[2][0], GETRY_WAYPOINTS[2][1], GETRY_WAYPOINTS[2][2]);
					    Spacer();
					    break;

                    case 9:  
					    m_creature->GetMotionMaster()->MovePoint(19, GETRY_WAYPOINTS[1][0], GETRY_WAYPOINTS[1][1], GETRY_WAYPOINTS[1][2]);
					    Spacer();
					    break;
                        
                    case 10:
                        pVaridus->Respawn();
                        m_creature->AI()->EnterEvadeMode();
                        break;
                }
            }
		}
		else
		{
            if((m_uiPhase == PHASE_ONE) || (m_uiPhase == PHASE_TWO) || (m_uiPhase == PHASE_THREE) || (m_uiPhase == PHASE_FOUR) || (m_uiPhase == PHASE_FIVE))
			    m_uiStepTimer -= uiDiff;
		}
    }
};

CreatureAI* GetAI_npc_shadowstalker_getry(Creature* pCreature)
{
    return new npc_shadowstalker_getryAI(pCreature);
}

bool QuestAccept_npc_shadowstalker_getry(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FOOLISH_ENDEAVORS)
    {
		if (npc_shadowstalker_getryAI* pGetryAI = dynamic_cast<npc_shadowstalker_getryAI*>(pCreature->AI()))
		{
            if(Creature* pVaridus = GetClosestCreatureWithEntry(pCreature, VARIDUS_ID, 100.0f))
            {
                if(pVaridus->isAlive())
                    pGetryAI->StartEventForPlayer(pPlayer);                                           
            }
		}
    }

    return true;
}

enum
{
    QUEST_LOADER_UP     = 11881,
    GO_PACKAGE          = 142181,   //wrong ID
    MAX_PACKAGE_COUNT   = 10,

};
struct MANGOS_DLL_DECL npc_jennyAI : public ScriptedAI
{
    npc_jennyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }
    bool m_bIsEvent;
   
    uint32 m_uiPackageCount;

    void Reset()
    {
        m_uiPackageCount = 0;
        m_bIsEvent = true;
    }
  
    Unit* SearchPlayer()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (pPlayer && pPlayer->GetQuestStatus(QUEST_LOADER_UP) == QUEST_STATUS_INCOMPLETE) 
                    return pPlayer;
        }
        return NULL;
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        m_uiPackageCount += 1;
        //m_creature->SummonGameobject(GO_PACKAGE, m_creature->GetPositionX()+1.0f, m_creature->GetPositionY()+1.0f, m_creature->GetPositionZ(), 0.0f, 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bIsEvent)
        {
            if(Unit *pPlayer = SearchPlayer())
            {
                m_creature->GetMotionMaster()->MoveFollow(pPlayer, 1.0f, 0);
            }

            if(m_uiPackageCount >= MAX_PACKAGE_COUNT)
            {
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->ForcedDespawn();
            }
        }
    }
};

CreatureAI* GetAI_npc_jenny(Creature* pCreature)
{
    return new npc_jennyAI(pCreature);
}

void AddSC_borean_tundra()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_taunkale_evacuee";
    pNewScript->GetAI = &GetAI_npc_taunkale_evacuee;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fizzcrank_fullthrottle";
    pNewScript->pGossipHello = &GossipHello_npc_fizzcrank_fullthrottle;
    pNewScript->pGossipSelect = &GossipSelect_npc_fizzcrank_fullthrottle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_iruk";
    pNewScript->pGossipHello = &GossipHello_npc_iruk;
    pNewScript->pGossipSelect = &GossipSelect_npc_iruk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kara_thricestar";
    pNewScript->pGossipHello = &GossipHello_npc_kara_thricestar;
    pNewScript->pGossipSelect = &GossipSelect_npc_kara_thricestar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nesingwary_trapper";
    pNewScript->GetAI = &GetAI_npc_nesingwary_trapper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oil_stained_wolf";
    pNewScript->GetAI = &GetAI_npc_oil_stained_wolf;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_oil_stained_wolf;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_oil_stained_wolf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_caribou_trap";
    pNewScript->pGOUse = &GOUse_go_caribou_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sinkhole_kill_credit";
    pNewScript->GetAI = &GetAI_npc_sinkhole_kill_credit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_surristrasz";
    pNewScript->pGossipHello = &GossipHello_npc_surristrasz;
    pNewScript->pGossipSelect = &GossipSelect_npc_surristrasz;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tiare";
    pNewScript->pGossipHello = &GossipHello_npc_tiare;
    pNewScript->pGossipSelect = &GossipSelect_npc_tiare;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lurgglbr";
    pNewScript->GetAI = &GetAI_npc_lurgglbr;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_lurgglbr;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_valanar";
    pNewScript->GetAI = &GetAI_npc_valanar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_thassarian";
    pNewScript->GetAI = &GetAI_npc_thassarian;
    pNewScript->pGossipHello = &GossipHello_npc_thassarian;
    pNewScript->pGossipSelect = &GossipSelect_npc_thassarian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mootoo_the_younger";
    pNewScript->GetAI = &GetAI_npc_mootoo_the_younger;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_mootoo_the_younger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bonker_togglevolt";
    pNewScript->GetAI = &GetAI_npc_bonker_togglevolt;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_bonker_togglevolt;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_seaforium_depth_charge";
    pNewScript->GetAI = &GetAI_npc_seaforium_depth_charge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_scourge_cage";
    pNewScript->pGOUse = &GOHello_go_scourge_cage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nexus_drake";
    pNewScript->GetAI = &GetAI_npc_nexus_drake;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_beryl_sorcerer";
    pNewScript->GetAI = &GetAI_npc_beryl_sorcerer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_seaforium_depth_charge";
    pNewScript->GetAI = &GetAI_npc_seaforium_depth_charge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_enkilah_necrolord";
    pNewScript->GetAI = &GetAI_npc_enkilah_necrolord;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_shadowstalker_getry";
    pNewScript->GetAI = &GetAI_npc_shadowstalker_getry;
	pNewScript->pQuestAcceptNPC = &QuestAccept_npc_shadowstalker_getry;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_varidus_the_flenser";
    pNewScript->GetAI = &GetAI_npc_varidus_the_flenser;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_jenny";
    pNewScript->GetAI = &GetAI_npc_jenny;
    pNewScript->RegisterSelf();
}
