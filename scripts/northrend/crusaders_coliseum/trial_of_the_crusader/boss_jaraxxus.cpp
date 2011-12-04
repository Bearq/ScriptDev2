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
SDComment: by Bearq
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Equipment
{
    EQUIP_MAIN           = 47266,
    EQUIP_OFFHAND        = 46996,
    EQUIP_RANGED         = 47267,
    EQUIP_DONE           = EQUIP_NO_CHANGE,
};

enum Summons
{
    NPC_LEGION_FLAME     = 34784,
    NPC_INFERNAL_VOLCANO = 34813,
    NPC_FEL_INFERNAL     = 34815,
    NPC_NETHER_PORTAL    = 34825,
    NPC_MISTRESS         = 34826,
};

enum BossSpells
{
    SPELL_NETHER_POWER          = 67108,
    SPELL_FEL_FIREBALL          = 66532,
    SPELL_FEL_LIGHTING          = 66528,
    SPELL_INCINERATE_FLESH      = 66237,
    SPELL_BURNING_INFERNO       = 66242,
    SPELL_TOUCH_OF_JARAXXUS     = 66209,	//Only on Heroic
    SPELL_CURSE_OF_NETHER       = 66211,	//When target in Touch of Yaraxxus is near player.
    SPELL_BERSERK               = 26662,

    //Infernal Volcano
    SPELL_INFERNAL              = 66258,
    SPELL_SHOOT                 = 66253,	//Effect Shoot ball. Summon Infernal
    SPELL_SHOOT_2               = 67068,	
    SPELL_SHOOT_TWO_BALS        = 67069,

    //Infernal
    SPELL_INFERNAL_ERUPTION     = 66255,
    SPELL_FEL_INFERNO           = 67047,
    SPELL_FEL_STREAK            = 66494,

    //Flame
    SPELL_LEGION_FLAME          = 66197,
    SPELL_LEGION_FLAME_0        = 66199,
    SPELL_LEGION_FLAME_1        = 66197,

    //Nether Portal
    SPELL_NETHER_PORTAL         = 66264,	
    SPELL_PORTAL                = 67105,    //Portal in herioc. Dont respawn when Mistress Pain summon.
    SPELL_PORTAL_2              = 67103,    //Portal in normal. Respawn when Mistress Paim summon.

    //Mistress Pain
    SPELL_SHIVAN_SLASH          = 67098,
    SPELL_SPINNING_STRIKE       = 66316,
    SPELL_MISTRESS_KISS         = 67077,
};

/*######
## boss_jaraxxus
######*/

struct MANGOS_DLL_DECL boss_jaraxxusAI : public BSWScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        m_bIsHardMode = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        m_bIsNormalMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;
    bool m_bIsNormalMode;
    bool m_bIsHardMode;

    Difficulty m_uiMapDifficulty;
    ObjectGuid m_VolcanoGuid;
    uint8 m_uiCase;
    uint32 m_uiFelFireballTimer;
    uint32 m_uiFelLightningTimer;
    uint32 m_uiIncinerateFleshTimer;
    uint32 m_uiNetherPowerTimer;
    uint32 m_uiLegionFlameTimer;
    uint32 m_uiTouchOfJaraxxusTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiCastShootTimer;
    uint32 m_uiBerserkTimer;

    void Reset()
    {
        m_uiFelFireballTimer = 5*IN_MILLISECONDS;
        m_uiFelLightningTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiIncinerateFleshTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        m_uiNetherPowerTimer = 20*IN_MILLISECONDS;
        m_uiLegionFlameTimer = 30*IN_MILLISECONDS;
        m_uiTouchOfJaraxxusTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiSummonTimer = 10000;//1*MINUTE*IN_MILLISECONDS;
        m_uiBerserkTimer = 6*MINUTE*IN_MILLISECONDS;
        m_uiCastShootTimer = 5000;

        m_uiCase = 1;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, NOT_STARTED);
    }

    void JustReachedHome()
    {
        if (!m_pInstance) return;
            m_pInstance->SetData(TYPE_JARAXXUS, FAIL);
            m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) return;
            DoScriptText(-1713525,m_creature);
            m_pInstance->SetData(TYPE_JARAXXUS, DONE);
            m_pInstance->SetData(TYPE_EVENT,2000);
            m_pInstance->SetData(TYPE_STAGE,0);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        m_creature->SetInCombatWithZone();
        m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
        DoScriptText(-1713514,m_creature);
    }

    void Summon()
    {
		if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
		{
			if (Creature* pVolcano = m_creature->SummonCreature(NPC_INFERNAL_VOLCANO, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
			{
                m_VolcanoGuid = pVolcano->GetObjectGuid();
            }
		}
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiNetherPowerTimer <= uiDiff)
        {
            if(DoCastSpellIfCan(m_creature, SPELL_NETHER_POWER) == CAST_OK)
            {
                if(m_creature->HasAura(SPELL_NETHER_POWER))
                {
                    if(SpellAuraHolderPtr pHolder = m_creature->GetSpellAuraHolder(SPELL_NETHER_POWER))
                    {
                        pHolder->SetStackAmount(5);
                        if(m_bIsHardMode)
                        {
                            pHolder->SetStackAmount(5);
                        }
                    }
                }
            }
            m_uiNetherPowerTimer = urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS);
        }else m_uiNetherPowerTimer -= uiDiff;

        if(m_uiFelFireballTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEL_FIREBALL);
            m_uiFelFireballTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        }else m_uiFelFireballTimer -= uiDiff;

        if(m_uiFelLightningTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_FEL_LIGHTING);
                m_uiFelLightningTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
        }else m_uiFelLightningTimer -= uiDiff;

        if(m_uiLegionFlameTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoScriptText(-1713518, m_creature);
                DoCastSpellIfCan(pTarget, SPELL_LEGION_FLAME);
            }
            m_uiLegionFlameTimer = 30*IN_MILLISECONDS;
        }else m_uiLegionFlameTimer -= uiDiff;

        if(m_uiIncinerateFleshTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1)) 
            {
                DoScriptText(-1713522 ,m_creature, pTarget);
                DoCastSpellIfCan(pTarget, SPELL_INCINERATE_FLESH);
            }
            m_uiIncinerateFleshTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        }else m_uiIncinerateFleshTimer -= uiDiff;

        if(m_uiSummonTimer < uiDiff)
        {
            switch(m_uiCase)
            {
                case 0:
                    DoScriptText(-1713519,m_creature);
                    Summon();
                    m_uiCase = 1;
                    break;
                case 1:
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        m_creature->SummonCreature(NPC_NETHER_PORTAL, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                        DoScriptText(-1713520,m_creature);
                    }
                    m_uiCase = 0;
                    break;
            }
            m_uiSummonTimer = 25000; //1*MINUTE*IN_MILLISECONDS;
        }else m_uiSummonTimer -= uiDiff;

        if(m_uiCastShootTimer < uiDiff)
        {
            if (Creature* pVolcano = m_creature->GetMap()->GetCreature(m_VolcanoGuid))
            {
                if(pVolcano->isAlive())
                {
                    DoCastSpellIfCan(m_creature, SPELL_SHOOT);
                }
            }
            m_uiCastShootTimer = 5000;
        }else m_uiCastShootTimer -= uiDiff;


        if(m_uiBerserkTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BERSERK);
            m_uiBerserkTimer = 6*MINUTE*IN_MILLISECONDS;
        }else m_uiBerserkTimer -= uiDiff;

        if(m_bIsHeroicMode)
        {
            if (m_uiTouchOfJaraxxusTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                {
                    DoCastSpellIfCan(pTarget, SPELL_TOUCH_OF_JARAXXUS);
                }
                m_uiTouchOfJaraxxusTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }else m_uiTouchOfJaraxxusTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};


/****************
** Legion FLame
*****************/
struct MANGOS_DLL_DECL mob_legion_flameAI : public BSWScriptedAI
{
    mob_legion_flameAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiRangeCheck_Timer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance) return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), 4.0f))
            {
                DoCastSpellIfCan(m_creature, SPELL_LEGION_FLAME_0);
            }
            
            if (m_creature->getVictim()) 
            {
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_creature->SetSpeedRate(MOVE_RUN, 0.5);
            }
            m_uiRangeCheck_Timer = 2000;
        }else m_uiRangeCheck_Timer -= uiDiff;
    }
};

/********************
** Infernal Volcano
*********************/
struct MANGOS_DLL_DECL mob_infernal_volcanoAI : public BSWScriptedAI
{
    mob_infernal_volcanoAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        m_bIsNormalMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;
    bool m_bIsNormalMode;

    Difficulty m_uiMapDifficulty;
    uint32 m_uiInfernalEruptionTimer;
    uint32 m_uiDiedTimer;

    void Reset()
    {
        m_uiInfernalEruptionTimer = 5000;
        m_uiDiedTimer = 16000;
        m_creature->SetRespawnDelay(DAY);

        SetCombatMovement(false);
        m_creature->SetObjectScale(1.0f);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if(m_bIsHeroicMode)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if(m_uiDiedTimer < uiDiff)
        {
            m_creature->ForcedDespawn();
            m_uiDiedTimer = 16000;
        }else m_uiDiedTimer -= uiDiff;

        /*if(m_bIsNormalMode)
        {
            if(m_uiSummonInfernalTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHOOT);
                m_uiSummonInfernalTimer = 5000;
            }else m_uiSummonInfernalTimer -= uiDiff;
        }*/
    }
};

/**************
** Fel Infernal
***************/
struct MANGOS_DLL_DECL mob_fel_infernalAI : public BSWScriptedAI
{
    mob_fel_infernalAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFelInfernoTimer;
    uint32 m_uiFelStreakTimer;

    void Reset()
    {
        m_uiFelInfernoTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiFelStreakTimer = urand(20*IN_MILLISECONDS, 15*IN_MILLISECONDS);

        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetObjectScale(1.0f);
    }

    void Aggro(Unit *who)
    {
        AttackStart(who);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiFelInfernoTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_FEL_INFERNO);
            }
            m_uiFelInfernoTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        }else m_uiFelInfernoTimer -= uiDiff;
        
        if(m_uiFelStreakTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_FEL_STREAK);
            }
            m_uiFelStreakTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        }else m_uiFelStreakTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*****************
** Nether Portal
******************/
struct MANGOS_DLL_DECL mob_nether_portalAI : public BSWScriptedAI
{
    mob_nether_portalAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    Difficulty m_uiMapDifficulty;
    uint32 m_uiSummonPainTimer;

    void Reset()
    {
        m_uiSummonPainTimer = 5000;
        m_creature->SetRespawnDelay(DAY);

        m_creature->SetObjectScale(1.0f);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->CastSpell(m_creature, SPELL_PORTAL_2, true);

        if(m_bIsHeroicMode)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->CastSpell(m_creature, SPELL_PORTAL, true);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance) return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
    }
};

/*******************
** Mistress of Pain
********************/
struct MANGOS_DLL_DECL mob_mistress_of_painAI : public BSWScriptedAI
{
    mob_mistress_of_painAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    Difficulty m_uiMapDifficulty;
    uint32 m_uiShivanSlashTimer;
    uint32 m_uiSpinningStrikeTimer;
    uint32 m_uiMistressKissTimer;

    void Reset()
    {
        m_uiShivanSlashTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiSpinningStrikeTimer = urand(18*IN_MILLISECONDS, 28*IN_MILLISECONDS);
        m_uiMistressKissTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);

        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetSpeedRate(MOVE_RUN, 1.5f);
        m_creature->SetObjectScale(1.0f);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance) return;
        DoScriptText(-1713523,m_creature, who);
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
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiShivanSlashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIVAN_SLASH);
            m_uiShivanSlashTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        }else m_uiShivanSlashTimer -= uiDiff;

        if(m_uiSpinningStrikeTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                DoCastSpellIfCan(pTarget, SPELL_SPINNING_STRIKE);
                m_uiSpinningStrikeTimer = urand(18*IN_MILLISECONDS, 28*IN_MILLISECONDS);
            }
        }else m_uiSpinningStrikeTimer -= uiDiff;
    
        if(m_bIsHeroicMode)
        {
            if(m_uiMistressKissTimer <= uiDiff)
            {
                if (Unit* target = SelectEnemyTargetWithinMana())
                {
                    DoCastSpellIfCan(target, SPELL_MISTRESS_KISS);
                    m_uiMistressKissTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);
                }
            }else m_uiMistressKissTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}
CreatureAI* GetAI_mob_legion_flame(Creature* pCreature)
{
    return new mob_legion_flameAI(pCreature);
}
CreatureAI* GetAI_mob_mistress_of_pain(Creature* pCreature)
{
    return new mob_mistress_of_painAI(pCreature);
}

CreatureAI* GetAI_mob_infernal_volcano(Creature* pCreature)
{
    return new mob_infernal_volcanoAI(pCreature);
}
CreatureAI* GetAI_mob_fel_infernal(Creature* pCreature)
{
    return new mob_fel_infernalAI(pCreature);
}

CreatureAI* GetAI_mob_nether_portal(Creature* pCreature)
{
    return new mob_nether_portalAI(pCreature);
}

void AddSC_boss_jaraxxus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_jaraxxus";
    newscript->GetAI = &GetAI_boss_jaraxxus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_legion_flame";
    newscript->GetAI = &GetAI_mob_legion_flame;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_infernal_volcano";
    newscript->GetAI = &GetAI_mob_infernal_volcano;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_fel_infernal";
    newscript->GetAI = &GetAI_mob_fel_infernal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nether_portal";
    newscript->GetAI = &GetAI_mob_nether_portal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mistress_of_pain";
    newscript->GetAI = &GetAI_mob_mistress_of_pain;
    newscript->RegisterSelf();
}
