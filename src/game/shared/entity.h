/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SHARED_ENTITY_H
#define GAME_SHARED_ENTITY_H

#include "gameworld.h"

#include <base/vmath.h>

#include <game/alloc.h>

#include <optional>

class CCollision;
class CEntity;
class CGameWorld;

/*
	Class: Entity
		Basic entity class.
*/
class CEntityBase
{
	MACRO_ALLOC_HEAP()

private:
	friend CGameWorldBase; // entity list handling
	friend CGameWorld;
	CEntityBase *m_pPrevTypeEntity;
	CEntityBase *m_pNextTypeEntity;

protected:
	/* Identity */
	CGameWorldBase *m_pGameWorld;
	CCollision *m_pCollision;

	std::optional<int> m_Id;
	int m_ObjType;

	/*
		Variable: m_ProximityRadius
			Contains the physical size of the entity.
	*/
	float m_ProximityRadius;

	/* State */
	bool m_MarkedForDestroy;

public: // TODO: Maybe make protected
	/*
		Variable: m_Pos
			Contains the current posititon of the entity.
	*/
	vec2 m_Pos;

	/* Getters */
	std::optional<int> GetId() const { return m_Id; }

	int m_SnapTicks;

	/* Constructor */
	CEntityBase(CGameWorldBase *pGameWorld, int Objtype, vec2 Pos = vec2(0, 0), int ProximityRadius = 0);
	CEntityBase() = default;

	/* Destructor */
	virtual ~CEntityBase() {}

	/* Objects */
	std::vector<SSwitchers> &Switchers() { return m_pGameWorld->Switchers(); }
	CTuningParams *GlobalTuning() { return &m_pGameWorld->TuningList()[0]; }
	CTuningParams *TuningList() { return m_pGameWorld->TuningList(); }
	CTuningParams *GetTuning(int i) { return m_pGameWorld->GetTuning(i); }
	CCollision *Collision() { return m_pCollision; }

	/* Getters */
	CEntityBase *TypeNext() { return m_pNextTypeEntity; }
	CEntityBase *TypePrev() { return m_pPrevTypeEntity; }
	const vec2 &GetPos() const { return m_Pos; }
	float GetProximityRadius() const { return m_ProximityRadius; }

	/* Other functions */

	/*
		Function: Destroy
			Destroys the entity.
	*/
	virtual void Destroy() { delete this; }

	/*
		Function: Reset
			Called when the game resets the map. Puts the entity
			back to its starting state or perhaps destroys it.
	*/
	virtual void Reset() {}

	/*
		Function: Tick
			Called to progress the entity to the next tick. Updates
			and moves the entity to its new state and position.
	*/
	virtual void Tick() {}

	/*
		Function: TickDeferred
			Called after all entities Tick() function has been called.
	*/
	virtual void TickDeferred() {}

	/*
		Function: TickPaused
			Called when the game is paused, to freeze the state and position of the entity.
	*/
	virtual void TickPaused() {}

	/*
		Function GetOwnerId
		Returns:
			ClientId of the initiator from this entity. -1 created by map.
			This is used by save/load to remove related entities to the tee.
			CCharacter should not return the PlayerId, because they get
			handled separately in save/load code.
	*/
	virtual int GetOwnerId() const { return -1; }

	/*
		Function: Snap
			Called when a new snapshot is being generated for a specific
			client.

		Arguments:
			SnappingClient - ID of the client which snapshot is
				being generated. Could be -1 to create a complete
				snapshot of everything in the game for demo
				recording.
	*/
	virtual void Snap(int SnappingClient) {}

	bool GameLayerClipped(vec2 CheckPos);
	virtual bool CanCollide(int ClientId) { return true; }

	// DDRace

	bool GetNearestAirPos(vec2 Pos, vec2 PrevPos, vec2 *pOutPos);
	bool GetNearestAirPosPlayer(vec2 PlayerPos, vec2 *pOutPos);

	int m_Number;
	int m_Layer;
};

#endif
