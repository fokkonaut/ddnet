/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "gameworld.h"

#include "entities/character.h"
#include "entity.h"
#include "gamecontext.h"
#include "gamecontroller.h"

#include <engine/shared/config.h>

#include <game/collision.h>

#include <algorithm>
#include <utility>

//////////////////////////////////////////////////
// game world
//////////////////////////////////////////////////
CGameWorld::CGameWorld() : CGameWorldBase()
{
	m_pGameServer = nullptr;
	m_pConfig = nullptr;
	m_pServer = nullptr;

	m_Paused = false;
	m_ResetRequested = false;
}

void CGameWorld::SetGameServer(CGameContext *pGameServer)
{
	m_pGameServer = pGameServer;
	m_pConfig = m_pGameServer->Config();
	m_pServer = m_pGameServer->Server();
}

void CGameWorld::Init(CCollision *pCollision, CTuningParams *pTuningList)
{
	CGameWorldBase::Init(pCollision, pTuningList);
	m_Core.InitSwitchers(pCollision->m_HighestSwitchNumber);
}

bool CGameWorld::NoWeakHookAndBounce()
{
	return g_Config.m_SvNoWeakHook;
}

void CGameWorld::InsertEntity(CEntityBase *pEnt)
{
#ifdef CONF_DEBUG
	for(CEntityBase *pCur = m_apFirstEntityTypes[pEnt->m_ObjType]; pCur; pCur = pCur->m_pNextTypeEntity)
		dbg_assert(pCur != pEnt, "err");
#endif

	CGameWorldBase::InsertEntity(pEnt);
}

//
void CGameWorld::Snap(int SnappingClient)
{
	for(CEntityBase *pEnt = m_apFirstEntityTypes[ENTTYPE_CHARACTER]; pEnt;)
	{
		m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
		pEnt->Snap(SnappingClient);
		pEnt = m_pNextTraverseEntity;
	}

	for(int i = 0; i < NUM_ENTTYPES; i++)
	{
		if(i == ENTTYPE_CHARACTER)
			continue;

		for(CEntityBase *pEnt = m_apFirstEntityTypes[i]; pEnt;)
		{
			m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
			pEnt->Snap(SnappingClient);
			pEnt = m_pNextTraverseEntity;
		}
	}
}

void CGameWorld::Reset()
{
	// reset all entities
	for(auto *pEnt : m_apFirstEntityTypes)
		for(; pEnt;)
		{
			m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
			pEnt->Reset();
			pEnt = m_pNextTraverseEntity;
		}
	RemoveEntities();

	GameServer()->m_pController->OnReset();
	RemoveEntities();

	m_ResetRequested = false;

	GameServer()->CreateAllEntities(false);
}

void CGameWorld::RemoveEntitiesFromPlayer(int PlayerId)
{
	RemoveEntitiesFromPlayers(&PlayerId, 1);
}

void CGameWorld::RemoveEntitiesFromPlayers(int PlayerIds[], int NumPlayers)
{
	for(auto *pEnt : m_apFirstEntityTypes)
	{
		for(; pEnt;)
		{
			m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
			for(int i = 0; i < NumPlayers; i++)
			{
				if(pEnt->GetOwnerId() == PlayerIds[i])
				{
					RemoveEntity(pEnt);
					pEnt->Destroy();
					break;
				}
			}
			pEnt = m_pNextTraverseEntity;
		}
	}
}

void CGameWorld::RemoveEntities()
{
	// destroy objects marked for destruction
	for(auto *pEnt : m_apFirstEntityTypes)
		for(; pEnt;)
		{
			m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
			if(pEnt->m_MarkedForDestroy)
			{
				RemoveEntity(pEnt);
				pEnt->Destroy();
			}
			pEnt = m_pNextTraverseEntity;
		}
}

void CGameWorld::Tick()
{
	if(m_ResetRequested)
		Reset();

	CGameWorldBase::Tick();

	// find the characters' strong/weak id
	int StrongWeakId = 0;
	for(CCharacter *pChar = dynamic_cast<CCharacter *>(FindFirst(ENTTYPE_CHARACTER)); pChar; pChar = dynamic_cast<CCharacter *>(pChar->TypeNext()))
	{
		pChar->m_StrongWeakId = StrongWeakId;
		StrongWeakId++;
	}
}

ESaveResult CGameWorld::BlocksSave(int ClientId)
{
	// check all objects
	for(auto *pEnt : m_apFirstEntityTypes)
		for(; pEnt;)
		{
			m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
			ESaveResult Result = dynamic_cast<CEntity *>(pEnt)->BlocksSave(ClientId);
			if(Result != ESaveResult::SUCCESS)
				return Result;
			pEnt = m_pNextTraverseEntity;
		}
	return ESaveResult::SUCCESS;
}

void CGameWorld::SwapClients(int Client1, int Client2)
{
	// update all objects
	for(auto *pEnt : m_apFirstEntityTypes)
		for(; pEnt;)
		{
			m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
			dynamic_cast<CEntity *>(pEnt)->SwapClients(Client1, Client2);
			pEnt = m_pNextTraverseEntity;
		}
}
