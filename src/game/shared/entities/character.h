/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SHARED_ENTITIES_CHARACTER_H
#define GAME_SHARED_ENTITIES_CHARACTER_H

#include <game/shared/entity.h>

class CGameTeams;
class CGameWorldBase;

class CCharacterBase : public virtual CEntityBase
{
protected:
	CCharacterBase();

	CCharacterCore m_Core;

public:
	void PreTick() {}
	CCharacterCore *Core() { return &m_Core; };
	bool IsSuper() { return false; }
	void ReleaseHook() {}
	int Team() { return 0; }
	bool Freeze() { return false; }
	bool Unfreeze() { return false; }

	int m_StrongWeakId;
};

#endif
