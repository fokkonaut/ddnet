/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "gameworld.h"

#include "entities/character.h"
#include "entity.h"

#include <engine/shared/config.h>

#include <game/collision.h>

#include <algorithm>
#include <utility>

//////////////////////////////////////////////////
// game world
//////////////////////////////////////////////////
CGameWorldBase::CGameWorldBase()
{
	m_pCollision = nullptr;
	m_pTuningList = nullptr;
	for(auto &pFirstEntityType : m_apFirstEntityTypes)
		pFirstEntityType = nullptr;
}

CGameWorldBase::~CGameWorldBase()
{
	Clear();
}

void CGameWorldBase::Init(CCollision *pCollision, CTuningParams *pTuningList)
{
	m_pCollision = pCollision;
	m_pTuningList = pTuningList;
}

void CGameWorldBase::Init(CCollision *pCollision, CTuningParams *pTuningList, const CMapBugs *pMapBugs)
{
	Init(pCollision, pTuningList);
}

void CGameWorldBase::Clear()
{
	// delete all entities
	for(auto &pFirstEntityType : m_apFirstEntityTypes)
		while(pFirstEntityType)
			delete pFirstEntityType; // NOLINT(clang-analyzer-cplusplus.NewDelete)
}

CEntityBase *CGameWorldBase::FindFirst(int Type)
{
	return Type < 0 || Type >= NUM_ENTTYPES ? nullptr : m_apFirstEntityTypes[Type];
}

CEntityBase *CGameWorldBase::FindLast(int Type)
{
	CEntityBase *pLast = FindFirst(Type);
	if(pLast)
		while(pLast->TypeNext())
			pLast = pLast->TypeNext();
	return pLast;
}

int CGameWorldBase::FindEntities(vec2 Pos, float Radius, CEntityBase **ppEnts, int Max, int Type)
{
	if(Type < 0 || Type >= NUM_ENTTYPES)
		return 0;

	int Num = 0;
	for(CEntityBase *pEnt = m_apFirstEntityTypes[Type]; pEnt; pEnt = pEnt->m_pNextTypeEntity)
	{
		if(distance(pEnt->m_Pos, Pos) < Radius + pEnt->m_ProximityRadius)
		{
			if(ppEnts)
				ppEnts[Num] = pEnt;
			Num++;
			if(Num == Max)
				break;
		}
	}

	return Num;
}

void CGameWorldBase::InsertEntity(CEntityBase *pEnt)
{
	// insert it
	if(m_apFirstEntityTypes[pEnt->m_ObjType])
		m_apFirstEntityTypes[pEnt->m_ObjType]->m_pPrevTypeEntity = pEnt;
	pEnt->m_pNextTypeEntity = m_apFirstEntityTypes[pEnt->m_ObjType];
	pEnt->m_pPrevTypeEntity = nullptr;
	m_apFirstEntityTypes[pEnt->m_ObjType] = pEnt;
}

void CGameWorldBase::RemoveEntity(CEntityBase *pEnt)
{
	// not in the list
	if(!pEnt->m_pNextTypeEntity && !pEnt->m_pPrevTypeEntity && m_apFirstEntityTypes[pEnt->m_ObjType] != pEnt)
		return;

	// remove
	if(pEnt->m_pPrevTypeEntity)
		pEnt->m_pPrevTypeEntity->m_pNextTypeEntity = pEnt->m_pNextTypeEntity;
	else
		m_apFirstEntityTypes[pEnt->m_ObjType] = pEnt->m_pNextTypeEntity;
	if(pEnt->m_pNextTypeEntity)
		pEnt->m_pNextTypeEntity->m_pPrevTypeEntity = pEnt->m_pPrevTypeEntity;

	// keep list traversing valid
	if(m_pNextTraverseEntity == pEnt)
		m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;

	pEnt->m_pNextTypeEntity = nullptr;
	pEnt->m_pPrevTypeEntity = nullptr;
}

void CGameWorldBase::RemoveEntities()
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

void CGameWorldBase::Tick()
{
	if(!m_Paused)
	{
		// update all objects
		for(int i = 0; i < NUM_ENTTYPES; i++)
		{
			// It's important to call PreTick() and Tick() after each other.
			// If we call PreTick() before, and Tick() after other entities have been processed, it causes physics changes such as a stronger shotgun or grenade.
			if(NoWeakHookAndBounce() && i == ENTTYPE_CHARACTER)
			{
				auto *pEnt = m_apFirstEntityTypes[i];
				for(; pEnt;)
				{
					m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
					dynamic_cast<CCharacterBase *>(pEnt)->PreTick();
					pEnt = m_pNextTraverseEntity;
				}
			}

			auto *pEnt = m_apFirstEntityTypes[i];
			for(; pEnt;)
			{
				m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
				pEnt->Tick();
				pEnt = m_pNextTraverseEntity;
			}
		}

		for(auto *pEnt : m_apFirstEntityTypes)
			for(; pEnt;)
			{
				m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
				pEnt->TickDeferred();
				pEnt->m_SnapTicks++;
				pEnt = m_pNextTraverseEntity;
			}
	}
	else
	{
		// update all objects
		for(auto *pEnt : m_apFirstEntityTypes)
			for(; pEnt;)
			{
				m_pNextTraverseEntity = pEnt->m_pNextTypeEntity;
				pEnt->TickPaused();
				pEnt = m_pNextTraverseEntity;
			}
	}

	RemoveEntities();
}

CCharacterBase *CGameWorldBase::IntersectCharacter(vec2 Pos0, vec2 Pos1, float Radius, vec2 &NewPos, const CCharacterBase *pNotThis, int CollideWith, const CCharacterBase *pThisOnly)
{
	return dynamic_cast<CCharacterBase *>(IntersectEntity(Pos0, Pos1, Radius, ENTTYPE_CHARACTER, NewPos, pNotThis, CollideWith, pThisOnly));
}

CEntityBase *CGameWorldBase::IntersectEntity(vec2 Pos0, vec2 Pos1, float Radius, int Type, vec2 &NewPos, const CEntityBase *pNotThis, int CollideWith, const CEntityBase *pThisOnly)
{
	float ClosestLen = distance(Pos0, Pos1) * 100.0f;
	CEntityBase *pClosest = nullptr;

	CEntityBase *pEntity = FindFirst(Type);
	for(; pEntity; pEntity = pEntity->TypeNext())
	{
		if(pEntity == pNotThis)
			continue;

		if(pThisOnly && pEntity != pThisOnly)
			continue;

		if(CollideWith != -1 && !pEntity->CanCollide(CollideWith))
			continue;

		vec2 IntersectPos;
		if(closest_point_on_line(Pos0, Pos1, pEntity->m_Pos, IntersectPos))
		{
			float Len = distance(pEntity->m_Pos, IntersectPos);
			if(Len < pEntity->m_ProximityRadius + Radius)
			{
				Len = distance(Pos0, IntersectPos);
				if(Len < ClosestLen)
				{
					NewPos = IntersectPos;
					ClosestLen = Len;
					pClosest = pEntity;
				}
			}
		}
	}

	return pClosest;
}

CCharacterBase *CGameWorldBase::ClosestCharacter(vec2 Pos, float Radius, const CEntityBase *pNotThis)
{
	// Find other players
	float ClosestRange = Radius * 2;
	CCharacterBase *pClosest = nullptr;

	CCharacterBase *p = dynamic_cast<CCharacterBase *>(FindFirst(ENTTYPE_CHARACTER));
	for(; p; p = dynamic_cast<CCharacterBase *>(p->TypeNext()))
	{
		if(p == pNotThis)
			continue;

		float Len = distance(Pos, p->m_Pos);
		if(Len < p->m_ProximityRadius + Radius)
		{
			if(Len < ClosestRange)
			{
				ClosestRange = Len;
				pClosest = p;
			}
		}
	}

	return pClosest;
}

std::vector<CCharacterBase *> CGameWorldBase::IntersectedCharacters(vec2 Pos0, vec2 Pos1, float Radius, const CEntityBase *pNotThis)
{
	std::vector<CCharacterBase *> vpCharacters;
	CCharacterBase *pChr = dynamic_cast<CCharacterBase *>(FindFirst(CGameWorldBase::ENTTYPE_CHARACTER));
	for(; pChr; pChr = dynamic_cast<CCharacterBase *>(pChr->TypeNext()))
	{
		if(pChr == pNotThis)
			continue;

		vec2 IntersectPos;
		if(closest_point_on_line(Pos0, Pos1, pChr->m_Pos, IntersectPos))
		{
			float Len = distance(pChr->m_Pos, IntersectPos);
			if(Len < pChr->m_ProximityRadius + Radius)
			{
				vpCharacters.push_back(pChr);
			}
		}
	}
	return vpCharacters;
}

void CGameWorldBase::ReleaseHooked(int ClientId)
{
	CCharacterBase *pChr = dynamic_cast<CCharacterBase *>(FindFirst(CGameWorldBase::ENTTYPE_CHARACTER));
	for(; pChr; pChr = dynamic_cast<CCharacterBase *>(pChr->TypeNext()))
	{
		if(pChr->Core()->HookedPlayer() == ClientId && !pChr->IsSuper())
		{
			pChr->ReleaseHook();
		}
	}
}
