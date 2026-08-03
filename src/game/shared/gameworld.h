/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SHARED_GAMEWORLD_H
#define GAME_SHARED_GAMEWORLD_H

#include <game/gamecore.h>

#include <vector>

class CCollision;
class CEntityBase;
class CCharacterBase;
class CMapBugs;

/*
	Class: Game World
		Tracks all entities in the game. Propagates tick and
		snap calls to all entities.
*/
class CGameWorldBase
{
public:
	enum
	{
		ENTTYPE_PROJECTILE = 0,
		ENTTYPE_LASER,
		ENTTYPE_DOOR,
		ENTTYPE_DRAGGER,
		ENTTYPE_LIGHT,
		ENTTYPE_GUN,
		ENTTYPE_PLASMA,
		ENTTYPE_PICKUP,
		ENTTYPE_FLAG,
		ENTTYPE_CHARACTER,
		NUM_ENTTYPES
	};

protected:
	virtual void RemoveEntities();

	CEntityBase *m_pNextTraverseEntity = nullptr;
	CEntityBase *m_apFirstEntityTypes[NUM_ENTTYPES];

	CCollision *m_pCollision;
	CTuningParams *m_pTuningList;

	virtual bool NoWeakHookAndBounce() { return false; }

public:

	CWorldCore m_Core;

	bool m_Paused;

	CGameWorldBase();
	virtual ~CGameWorldBase();

	void Clear();

	virtual void Init(CCollision *pCollision, CTuningParams *pTuningList);
	virtual void Init(CCollision *pCollision, CTuningParams *pTuningList, const CMapBugs *pMapBugs);

	CEntityBase *FindFirst(int Type);
	CEntityBase *FindLast(int Type);

	/*
		Function: FindEntities
			Finds entities close to a position and returns them in a list.

		Arguments:
			Pos - Position.
			Radius - How close the entities have to be.
			ppEnts - Pointer to a list that should be filled with the pointers
				to the entities.
			Max - Number of entities that fits into the ents array.
			Type - Type of the entities to find.

		Returns:
			Number of entities found and added to the ents array.
	*/
	int FindEntities(vec2 Pos, float Radius, CEntityBase **ppEnts, int Max, int Type);

	/**
	 * Finds the CCharacter that intersects the line.
	 *
	 * @see IntersectEntity
	 *
	 * @param Pos0 Start position
	 * @param Pos1 End position
	 * @param Radius How far from the line the @link CCharacter @endlink is allowed to be
	 * @param NewPos Intersection position
	 * @param pNotThis Character to ignore intersecting with
	 * @param CollideWith Only find entities that can collide with that Client Id (pass -1 to ignore this check)
	 * @param pThisOnly Only search this specific character and ignore all others
	 *
	 * @return Pointer to the closest hit or `nullptr` if there is no intersection.
	 */
	CCharacterBase *IntersectCharacter(vec2 Pos0, vec2 Pos1, float Radius, vec2 &NewPos, const CCharacterBase *pNotThis = nullptr, int CollideWith = -1, const CCharacterBase *pThisOnly = nullptr);

	/**
	 * Finds the CEntity that intersects the line.
	 *
	 * @see IntersectCharacter
	 *
	 * @param Pos0 Start position
	 * @param Pos1 End position
	 * @param Radius How far from the line the @link CEntity @endlink is allowed to be
	 * @param Type Type of the entity to intersect
	 * @param NewPos Intersection position
	 * @param pNotThis Entity to ignore intersecting with
	 * @param CollideWith Only find entities that can collide with that Client Id (pass -1 to ignore this check)
	 * @param pThisOnly Only search this specific entity and ignore all others
	 *
	 * @return Pointer to the closest hit or `nullptr` if there is no intersection.
	 */
	CEntityBase *IntersectEntity(vec2 Pos0, vec2 Pos1, float Radius, int Type, vec2 &NewPos, const CEntityBase *pNotThis = nullptr, int CollideWith = -1, const CEntityBase *pThisOnly = nullptr);

	/*
		Function: ClosestCharacter
			Finds the closest CCharacter to a specific point.

		Arguments:
			Pos - The center position.
			Radius - How far off the CCharacter is allowed to be
			pNotThis - Entity to ignore

		Returns:
			Returns a pointer to the closest CCharacter or nullptr if no CCharacter is close enough.
	*/
	CCharacterBase *ClosestCharacter(vec2 Pos, float Radius, const CEntityBase *pNotThis);

	/*
		Function: InsertEntity
			Adds an entity to the world.

		Arguments:
			pEntity - Entity to add
	*/
	virtual void InsertEntity(CEntityBase *pEntity);

	/*
		Function: RemoveEntity
			Removes an entity from the world.

		Arguments:
			pEntity - Entity to remove
	*/
	virtual void RemoveEntity(CEntityBase *pEntity);

	/*
		Function: Tick
			Calls Tick on all the entities in the world to progress
			the world to the next tick.
	*/
	virtual void Tick();

	// DDRace
	void ReleaseHooked(int ClientId);

	/*
		Function: IntersectedCharacters
			Finds all CCharacters that intersect the line.

		Arguments:
			Pos0 - Start position
			Pos1 - End position
			Radius - How for from the line the CCharacter is allowed to be.
			pNotThis - Entity to ignore intersecting with

		Returns:
			Returns list with all Characters on line.
	*/
	std::vector<CCharacterBase *> IntersectedCharacters(vec2 Pos0, vec2 Pos1, float Radius, const CEntityBase *pNotThis = nullptr);

	const CTuningParams *TuningList() const { return m_pTuningList; }
	CTuningParams *TuningList() { return m_pTuningList; }
	const CTuningParams *GlobalTuning() const { return &TuningList()[0]; }
	CTuningParams *GlobalTuning() { return &TuningList()[0]; }
	const CTuningParams *GetTuning(int i) const { return &TuningList()[i]; }
	CTuningParams *GetTuning(int i) { return &TuningList()[i]; }

	const CCollision *Collision() const { return m_pCollision; }
	CCollision *Collision() { return m_pCollision; }
	
	std::vector<SSwitchers> &Switchers() { return m_Core.m_vSwitchers; }
};

#endif
