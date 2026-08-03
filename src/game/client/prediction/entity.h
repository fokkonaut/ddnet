/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_PREDICTION_ENTITY_H
#define GAME_CLIENT_PREDICTION_ENTITY_H

#include "gameworld.h"

#include <base/vmath.h>

#include <game/alloc.h>
#include <game/shared/entity.h>

class CEntity : public virtual CEntityBase
{
public:
	CEntity(CGameWorld *pGameWorld, int Objtype, vec2 Pos = vec2(0, 0), int ProximityRadius = 0);
	virtual ~CEntity();

	CGameWorld *GameWorld() const { return static_cast<CGameWorld *>(m_pGameWorld); }

	int m_DestroyTick;
	int m_LastRenderTick;
	CEntity *m_pParent;
	CEntity *m_pChild;
	void Keep()
	{
		m_SnapTicks = 0;
		m_MarkedForDestroy = false;
	}

	CEntity()
	{
		m_Id = -1;
		m_pGameWorld = nullptr;
	}

	CEntity(CGameWorld *pGameWorld)
	{
		m_pGameWorld = pGameWorld;
	}
};

#endif
