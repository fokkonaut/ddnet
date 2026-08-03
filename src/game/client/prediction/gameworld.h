/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_PREDICTION_GAMEWORLD_H
#define GAME_CLIENT_PREDICTION_GAMEWORLD_H

#include <game/teamscore.h>

#include <game/shared/gameworld.h>

#include <list>

class CCharacter;
class CEntity;

class CGameWorld : public CGameWorldBase
{
public:
	CTeamsCore m_Teams;

	CGameWorld();
	~CGameWorld();
	void Init(CCollision *pCollision, CTuningParams *pTuningList, const CMapBugs *pMapBugs) override;

	void InsertEntity(CEntityBase *pEntity, bool Last = false);
	void RemoveEntity(CEntityBase *pEntity) override;
	void RemoveCharacter(CCharacter *pChar);
	void Tick() override;

	// DDRace

	int m_GameTick;

	// getter for server variables
	int GameTick() const { return m_GameTick; }
	int GameTickSpeed() const { return SERVER_TICK_SPEED; }
	CTeamsCore *Teams() { return &m_Teams; }
	CEntity *GetEntity(int Id, int EntityType);
	CCharacter *GetCharacterById(int Id) { return (Id >= 0 && Id < MAX_CLIENTS) ? m_apCharacters[Id] : nullptr; }

	// from gamecontext
	void CreateExplosion(vec2 Pos, int Owner, int Weapon, bool NoDamage, int ActivatedTeam, CClientMask Mask, int Id = -1);

	// for client side prediction
	struct
	{
		bool m_IsDDRace;
		bool m_IsVanilla;
		bool m_IsFNG;
		bool m_InfiniteAmmo;
		bool m_PredictTiles;
		int m_PredictFreeze;
		bool m_PredictWeapons;
		bool m_PredictDDRace;
		bool m_IsSolo;
		bool m_UseTuneZones;
		bool m_BugDDRaceInput;
		bool m_NoWeakHookAndBounce;
		bool m_PredictEvents;
	} m_WorldConfig;

	bool m_IsValidCopy;
	CGameWorld *m_pParent;
	CGameWorld *m_pChild;

	int m_LocalClientId;

	bool IsLocalTeam(int OwnerId) const;
	void OnModified() const;
	void NetObjBegin(CTeamsCore Teams, int LocalClientId);
	void NetCharAdd(int ObjId, CNetObj_Character *pChar, CNetObj_DDNetCharacter *pExtended, int GameTeam, bool IsLocal);
	void NetObjAdd(int ObjId, int ObjType, const void *pObjData, const CNetObj_EntityEx *pDataEx);
	void NetObjEnd();
	void CopyWorld(CGameWorld *pFrom);
	CEntity *FindMatch(int ObjId, int ObjType, const void *pObjData);

	bool EmulateBug(int Bug) const;

	class CPredictedEvent
	{
	public:
		int m_EventId;
		vec2 m_Pos; // NetEvent's Pos are integers
		int m_Id; // identifier to prevent adding the same event multiple times
		int m_Tick;

		int m_ExtraInfo;
		bool m_Handled = false;

		CPredictedEvent(int EventId, vec2 Pos, int Id, int Tick, int ExtraInfo = -1) :
			m_EventId(EventId), m_Pos(vec2((int)Pos.x, (int)Pos.y)), m_Id(Id), m_Tick(Tick), m_ExtraInfo(ExtraInfo)
		{
		}
	};

	std::vector<CPredictedEvent> m_PredictedEvents;

	void CreatePredictedEvent(const CPredictedEvent &NewEvent);
	bool CheckPredictedEventHandled(const CPredictedEvent &CheckEvent);
	void PlayPredictedEvents(int Tick);

	void CreatePredictedSound(vec2 Pos, int SoundId, int Id = -1);
	void CreatePredictedExplosionEvent(vec2 Pos, int Id = -1);
	void CreatePredictedHammerHitEvent(vec2 Pos, int Id = -1);
	void CreatePredictedDamageIndEvent(vec2 Pos, float Angle, int Amount, int Id = -1);

private:
	void RemoveEntities() override;

	CCharacter *m_apCharacters[MAX_CLIENTS];

	const CMapBugs *m_pMapBugs;

	bool NoWeakHookAndBounce() override;
};

class CCharOrder
{
public:
	std::list<int> m_Ids; // reverse of the order in the gameworld, since entities will be inserted in reverse
	CCharOrder()
	{
		Reset();
	}
	void Reset()
	{
		m_Ids.clear();
		for(int i = 0; i < MAX_CLIENTS; i++)
			m_Ids.push_back(i);
	}
	void GiveStrong(int c)
	{
		if(0 <= c && c < MAX_CLIENTS)
		{
			m_Ids.remove(c);
			m_Ids.push_front(c);
		}
	}
	void GiveWeak(int c)
	{
		if(0 <= c && c < MAX_CLIENTS)
		{
			m_Ids.remove(c);
			m_Ids.push_back(c);
		}
	}
	bool HasStrongAgainst(int From, int To)
	{
		for(int i : m_Ids)
		{
			if(i == To)
				return false;
			else if(i == From)
				return true;
		}
		return false;
	}
};

#endif
