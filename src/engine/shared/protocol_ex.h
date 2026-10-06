#ifndef ENGINE_SHARED_PROTOCOL_EX_H
#define ENGINE_SHARED_PROTOCOL_EX_H

#include <engine/shared/uuid_manager.h>

class CMsgPacker;
class CUnpacker;

enum // NOLINT(readability-enum-initial-value)
{
	NETMSG_EX_INVALID = UUID_INVALID,
	NETMSG_EX_UNKNOWN = UUID_UNKNOWN,

	OFFSET_NETMSG_UUID = OFFSET_UUID,

	__NETMSG_UUID_HELPER = OFFSET_NETMSG_UUID - 1,
#define UUID(id, name) id,
#include "protocol_ex_msgs.h"
#undef UUID
	OFFSET_TEEHISTORIAN_UUID,
};

enum
{
	UNPACKMESSAGE_ERROR = 0,
	UNPACKMESSAGE_OK,
	UNPACKMESSAGE_ANSWER,
};

enum
{
	SERVERCAP_CURVERSION = 5,
	SERVERCAPFLAG_DDNET = 1 << 0,
	SERVERCAPFLAG_CHATTIMEOUTCODE = 1 << 1,
	SERVERCAPFLAG_ANYPLAYERFLAG = 1 << 2,
	SERVERCAPFLAG_PINGEX = 1 << 3,
	SERVERCAPFLAG_ALLOWDUMMY = 1 << 4,
	SERVERCAPFLAG_SYNCWEAPONINPUT = 1 << 5,
};

// The client capabilities are designed to replace client version checks on the server-side.
// They do not have to be supported forever by the server in order to reduce maintenance.
// Clients will always keep this list of capabilities to send to future unupdated servers.
// Client capabilities allow clients to gradually communicate their supported features to servers
// without implementing previous breaking changes just to support one specific feature.
enum
{
	CLIENTCAP_CURVERSION = 1,
	CLIENTCAPFLAG_WHISPER = 0,
	CLIENTCAPFLAG_ANTIPING_PROJECTILE,
	CLIENTCAPFLAG_UPDATER_FIXED,
	CLIENTCAPFLAG_GAMETICK,
	CLIENTCAPFLAG_EARLY_VERSION,
	CLIENTCAPFLAG_MSG_LEGACY,
	CLIENTCAPFLAG_INDEPENDENT_SPECTATORS_TEAM,
	CLIENTCAPFLAG_WEAPON_SHIELDS,
	CLIENTCAPFLAG_NEW_HUD,
	CLIENTCAPFLAG_MULTI_LASER,
	CLIENTCAPFLAG_ENTITY_NETOBJS,
	CLIENTCAPFLAG_REDIRECT,
	CLIENTCAPFLAG_PLAYERFLAG_SPEC_CAM,
	CLIENTCAPFLAG_RECONNECT,
	CLIENTCAPFLAG_128_PLAYERS,
	CLIENTCAPFLAG_PREINPUT,
	CLIENTCAPFLAG_SAVE_CODE,
	CLIENTCAPFLAG_IMPORTANT_ALERT,
	CLIENTCAPFLAG_MAP_BESTTIME,
	CLIENTCAPFLAG_128_TEAMS,
	CLIENTCAPFLAG_PICKUP_FREEZE,

	// TODO: When exceeding 31, send flags in chunks of multiple integers, reconstructed to std::bitset on server-side
	NUM_SUPPORTED_CLIENTCAPFLAGS,
	CLIENTCAPFLAG_ALL_FLAGS_SET = 0xFFFFFFFF & ((1 << NUM_SUPPORTED_CLIENTCAPFLAGS) - 1),
};

void RegisterUuids(CUuidManager *pManager);
int UnpackMessageId(int *pId, bool *pSys, CUuid *pUuid, CUnpacker *pUnpacker, CMsgPacker *pPacker);

#endif // ENGINE_SHARED_PROTOCOL_EX_H
