/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITY_H
#define GAME_SERVER_ENTITY_H

#include "gameworld.h"
#include "save.h"

#include <base/vmath.h>

#include <game/alloc.h>
#include <game/shared/entity.h>

class CGameContext;

/*
	Class: Entity
		Basic entity class.
*/
class CEntity : public virtual CEntityBase
{
public:
	/* Constructor */
	CEntity(CGameWorld *pGameWorld, int Objtype, bool SnapFreeId, vec2 Pos = vec2(0, 0), int ProximityRadius = 0);
	CEntity(bool SnapFreeId) : CEntity(nullptr, 0, SnapFreeId) {}

	/* Destructor */
	virtual ~CEntity();

	/* Objects */
	CGameWorld *GameWorld() const { return static_cast<CGameWorld *>(m_pGameWorld); }
	class CConfig *Config() { return GameWorld()->Config(); }
	class CGameContext *GameServer() { return GameWorld()->GameServer(); }
	class IServer *Server() { return GameWorld()->Server(); }

	/*
		Function: SwapClients
			Called when two players have swapped their client ids.

		Arguments:
			Client1 - First client ID
			Client2 - Second client ID
	*/
	virtual void SwapClients(int Client1, int Client2) {}

	/*
		Function: BlocksSave
			Called to check if a team can be saved

		Arguments:
			ClientId - Client ID
	*/
	virtual ESaveResult BlocksSave(int ClientId) { return ESaveResult::SUCCESS; }

	/*
		Function GetOwnerId
		Returns:
			ClientId of the initiator from this entity. -1 created by map.
			This is used by save/load to remove related entities to the tee.
			CCharacter should not return the PlayerId, because they get
			handled separately in save/load code.
	*/
	int GetOwnerId() const override { return -1; }

	/*
		Function: NetworkClipped
			Performs a series of test to see if a client can see the
			entity.

		Arguments:
			SnappingClient - ID of the client which snapshot is
				being generated. Could be -1 to create a complete
				snapshot of everything in the game for demo
				recording.

		Returns:
			True if the entity doesn't have to be in the snapshot.
	*/
	bool NetworkClipped(int SnappingClient) const;
	bool NetworkClipped(int SnappingClient, vec2 CheckPos) const;
	bool NetworkClippedLine(int SnappingClient, vec2 StartPos, vec2 EndPos) const;
};

bool NetworkClipped(const CGameContext *pGameServer, int SnappingClient, vec2 CheckPos);
bool NetworkClippedLine(const CGameContext *pGameServer, int SnappingClient, vec2 StartPos, vec2 EndPos);

#endif
