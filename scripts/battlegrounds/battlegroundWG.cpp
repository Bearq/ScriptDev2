/* Copyright (C) 2006 - 2012 /dev/rsa for ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: BattleGround Wintergrasp 
SD%Complete: 
SDComment: Support Vehicle and Npc in Wintergrasp battle
SDCategory: 
EndScriptData */

#include "precompiled.h"
#include "BattleGroundWG.h"
#include "Vehicle.h"

/******************
** npc_wg_catapult
*******************/
struct MANGOS_DLL_DECL npc_wg_catapultAI : public ScriptedAI
{
    npc_wg_catapultAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
    }

    bool done;
    BattleGround *bg;

    void Aggro(Unit* who){ m_creature->CombatStop(); }

    void EnterCombat(Unit *pEnemy)
    {
        if (!m_creature->isCharmed())
            m_creature->CombatStop();
    }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (((BattleGroundWG*)bg)->GetDefender() == pPlayer->GetTeam() || bg->GetStatus() == STATUS_WAIT_JOIN)
                return;

            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    bool mustDespawn(BattleGround *bg)
    {
        if (bg->GetStatus() == STATUS_WAIT_JOIN && ((BattleGroundWG*)bg)->GetDefender() == ALLIANCE)
        {
            float x = m_creature->GetPositionX();
            if (x < 1400.0f)
                return true;
        }
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
            {
                m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_WG_CATAPULT));
                if (mustDespawn(bg))
                    m_creature->ForcedDespawn();
            }
        }
    }
};

bool GossipHello_npc_wg_catapult(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_wg_catapultAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

/********************
** npc_wg_demolisher
*********************/
struct MANGOS_DLL_DECL npc_wg_demolisherAI : public ScriptedAI
{
    npc_wg_demolisherAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
    }

    bool done;
    BattleGround *bg;

    void Aggro(Unit* who){ m_creature->CombatStop(); }

    void EnterCombat(Unit *pEnemy)
    {
        if (!m_creature->isCharmed())
            m_creature->CombatStop();
    }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (((BattleGroundWG*)bg)->GetDefender() == pPlayer->GetTeam() || bg->GetStatus() == STATUS_WAIT_JOIN)
                return;

            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    bool mustDespawn(BattleGround *bg)
    {
        if (bg->GetStatus() == STATUS_WAIT_JOIN && ((BattleGroundWG*)bg)->GetDefender() == ALLIANCE)
        {
            float x = m_creature->GetPositionX();
            if (x < 1400.0f)
                return true;
        }
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
            {
                m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_BG_DEMOLISHER));
                if (mustDespawn(bg))
                    m_creature->ForcedDespawn();
            }
        }
    }
};

bool GossipHello_npc_wg_demolisher(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_wg_demolisherAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

/********************
** npc_wg_siege_engine
*********************/
struct MANGOS_DLL_DECL npc_wg_siege_engineAI : public ScriptedAI
{
    npc_wg_siege_engineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
    }

    bool done;
    BattleGround *bg;

    void Aggro(Unit* who){ m_creature->CombatStop(); }

    void EnterCombat(Unit *pEnemy)
    {
        if (!m_creature->isCharmed())
            m_creature->CombatStop();
    }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (((BattleGroundWG*)bg)->GetDefender() == pPlayer->GetTeam() || bg->GetStatus() == STATUS_WAIT_JOIN)
                return;

            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    bool mustDespawn(BattleGround *bg)
    {
        if (bg->GetStatus() == STATUS_WAIT_JOIN && ((BattleGroundWG*)bg)->GetDefender() == ALLIANCE)
        {
            float x = m_creature->GetPositionX();
            if (x < 1400.0f)
                return true;
        }
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
            {
                m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_WG_SIEGE_ENGINE));
                if (mustDespawn(bg))
                    m_creature->ForcedDespawn();
            }
        }
    }
};

bool GossipHello_npc_wg_siege_engine(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_wg_siege_engineAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

/****************
** npc_wg_turret
*****************/
struct MANGOS_DLL_DECL npc_wg_turretAI : public ScriptedAI
{
    npc_wg_turretAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
    }

    bool done;
    BattleGround *bg;

    void Aggro(Unit* who){ m_creature->CombatStop(); }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (bg->GetDefender() != pPlayer->GetTeam() || bg->GetStatus() == STATUS_WAIT_JOIN)
                return;

            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
                m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_WG_TURRET));
        }
    }
};

bool GossipHello_npc_wg_turret(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_wg_turretAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

/****************
** npc_wg_cannon
*****************/
struct MANGOS_DLL_DECL npc_wg_cannonAI : public ScriptedAI
{
    npc_wg_cannonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
    }

    bool done;
    BattleGround *bg;

    void Aggro(Unit* who){ m_creature->CombatStop(); }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (bg->GetDefender() != pPlayer->GetTeam() || bg->GetStatus() == STATUS_WAIT_JOIN)
                return;

            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
                m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_SA_CANNON));
        }
    }
};

bool GossipHello_npc_wg_cannon(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_wg_cannonAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

/***************
** npc_wg_rpg
****************/
bool GOHello_go_wg_rpg(Player* pPlayer, GameObject* pGo)
{
    if (!pPlayer || !pGo)
        return false;

    if (pPlayer->GetMapId() == 4197)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (pPlayer->GetTeam() != bg->GetDefender())
            {
                //pPlayer->CastSpell(pPlayer, 52415, false);
                pGo->Delete();
            }
        }
    }
    return true;
}


CreatureAI* GetAI_npc_wg_catapult(Creature* pCreature)
{
    return new npc_wg_catapultAI(pCreature);
}

CreatureAI* GetAI_npc_wg_demolisher(Creature* pCreature)
{
    return new npc_wg_demolisherAI(pCreature);
}

CreatureAI* GetAI_npc_wg_siege_engine(Creature* pCreature)
{
    return new npc_wg_siege_engineAI(pCreature);
}

CreatureAI* GetAI_npc_wg_turret(Creature* pCreature)
{
    return new npc_wg_turretAI(pCreature);
}

CreatureAI* GetAI_npc_wg_cannon(Creature* pCreature)
{
    return new npc_wg_cannonAI(pCreature);
}

void AddSC_battlegroundWG()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_wg_catapult";
    pNewScript->GetAI = &GetAI_npc_wg_catapult;
    pNewScript->pGossipHello = &GossipHello_npc_wg_catapult;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wg_demolisher";
    pNewScript->GetAI = &GetAI_npc_wg_demolisher;
    pNewScript->pGossipHello = &GossipHello_npc_wg_demolisher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wg_siege_engine";
    pNewScript->GetAI = &GetAI_npc_wg_siege_engine;
    pNewScript->pGossipHello = &GossipHello_npc_wg_siege_engine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wg_turret";
    pNewScript->GetAI = &GetAI_npc_wg_turret;
    pNewScript->pGossipHello = &GossipHello_npc_wg_turret;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wg_cannon";
    pNewScript->GetAI = &GetAI_npc_wg_cannon;
    pNewScript->pGossipHello = &GossipHello_npc_wg_cannon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="go_wg_rpg";
    pNewScript->pGOUse = &GOHello_go_wg_rpg;
    pNewScript->RegisterSelf();
}