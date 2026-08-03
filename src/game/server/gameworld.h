/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEWORLD_H
#define GAME_SERVER_GAMEWORLD_H

#include "save.h"

#include <game/shared/gameworld.h>

/*
	Class: Game World
		Tracks all entities in the game. Propagates tick and
		snap calls to all entities.
*/
class CGameWorld : public CGameWorldBase
{
private:
	void Reset();
	void RemoveEntities() override;

	class CGameContext *m_pGameServer;
	class CConfig *m_pConfig;
	class IServer *m_pServer;

	bool NoWeakHookAndBounce() override;

public:
	class CGameContext *GameServer() { return m_pGameServer; }
	class CConfig *Config() { return m_pConfig; }
	class IServer *Server() { return m_pServer; }

	bool m_ResetRequested;

	CGameWorld();
	~CGameWorld() {}

	void SetGameServer(CGameContext *pGameServer);
	void Init(CCollision *pCollision, CTuningParams *pTuningList) override;

	/*
		Function: InsertEntity
			Adds an entity to the world.

		Arguments:
			pEntity - Entity to add
	*/
	void InsertEntity(CEntityBase *pEntity) override;

	void RemoveEntitiesFromPlayer(int PlayerId);
	void RemoveEntitiesFromPlayers(int PlayerIds[], int NumPlayers);

	/*
		Function: Snap
			Calls Snap on all the entities in the world to create
			the snapshot.

		Arguments:
			SnappingClient - ID of the client which snapshot
			is being created.
	*/
	void Snap(int SnappingClient);

	/*
		Function: Tick
			Calls Tick on all the entities in the world to progress
			the world to the next tick.
	*/
	void Tick() override;

	/*
		Function: SwapClients
			Calls SwapClients on all the entities in the world to ensure that /swap
			command is handled safely.
	*/
	void SwapClients(int Client1, int Client2);

	/*
		Function: BlocksSave
			Checks if any entity would block /save
	*/
	ESaveResult BlocksSave(int ClientId);
};

#endif
