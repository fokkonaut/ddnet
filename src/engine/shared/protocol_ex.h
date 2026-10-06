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
// Please use them wisely similar to server capabilities, many things can be solved without.
enum
{
	CLIENTCAP_CURVERSION = 1,
	CLIENTCAPFLAG_WHISPER = 1U << 0,
	CLIENTCAPFLAG_ANTIPING_PROJECTILE = 1U << 1,
	CLIENTCAPFLAG_UPDATER_FIXED = 1U << 2,
	CLIENTCAPFLAG_GAMETICK = 1U << 3,
	CLIENTCAPFLAG_EARLY_VERSION = 1U << 4,
	CLIENTCAPFLAG_MSG_LEGACY = 1U << 5,
	CLIENTCAPFLAG_INDEPENDENT_SPECTATORS_TEAM = 1U << 6,
	CLIENTCAPFLAG_WEAPON_SHIELDS = 1U << 7,
	CLIENTCAPFLAG_NEW_HUD = 1U << 8,
	CLIENTCAPFLAG_MULTI_LASER = 1U << 9,
	CLIENTCAPFLAG_ENTITY_NETOBJS = 1U << 10,
	CLIENTCAPFLAG_REDIRECT = 1U << 11,
	CLIENTCAPFLAG_PLAYERFLAG_SPEC_CAM = 1U << 12,
	CLIENTCAPFLAG_RECONNECT = 1U << 13,
	CLIENTCAPFLAG_128_PLAYERS = 1U << 14,
	CLIENTCAPFLAG_PREINPUT = 1U << 15,
	CLIENTCAPFLAG_SAVE_CODE = 1U << 16,
	CLIENTCAPFLAG_IMPORTANT_ALERT = 1U << 17,
	CLIENTCAPFLAG_MAP_BESTTIME = 1U << 18,
	CLIENTCAPFLAG_128_TEAMS = 1U << 19,
	CLIENTCAPFLAG_PICKUP_FREEZE = 1U << 20,

	// TODO: When exceeding 32, send flags in chunks of multiple integers
	NUM_SUPPORTED_CLIENTCAPFLAGS = 21,
	CLIENTCAPFLAG_ALL_FLAGS_SET = 0xFFFFFFFF & ((1U << NUM_SUPPORTED_CLIENTCAPFLAGS) - 1),
};

void RegisterUuids(CUuidManager *pManager);
int UnpackMessageId(int *pId, bool *pSys, CUuid *pUuid, CUnpacker *pUnpacker, CMsgPacker *pPacker);

#endif // ENGINE_SHARED_PROTOCOL_EX_H
