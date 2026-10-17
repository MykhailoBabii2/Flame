//
// CState recompiled functions
//

#include "dk2/CState.h"

#include "dk2/CWorld.h"
#include "dk2/entities/CCreature.h"
#include "dk2/entities/CRoom.h"
#include "dk2/utils/Vec3i.h"
#include "dk2/world/map/MyMapElement.h"
#include "dk2_functions.h"
#include "dk2_globals.h"
#include "dk2/entities/creature_state.h"
#include "patches/micro_patches.h"


// original: 0045A000 CState_leavingDungeon_195_45A000
// Creature is leaving the dungeon: if it stands on its own portal (or is flagged as leaving),
// walk to the portal center.
// Bug: MyMapElement::getRoom() returns NULL when there is no room on the creature's tile
// (for example the portal was destroyed or the tile was claimed/sold under the creature).
// The original code dereferenced the result without checking it (crash at 0045A059).
BOOL dk2::CState::leavingDungeon_195_45A000() {
    CCreature *creature = this->creature;
    MyMapElement *mapElem = this->world->v_getMapElem_2(&creature->f16_pos);

    BOOL onPortal = mapElem->sub_4539B0(3, creature->f24_playerId, 0);
    if (!onPortal && (creature->stateFlags & StateFlags1_Leaving) == 0) return FALSE;

    CRoom *room = mapElem->getRoom();
    if (patch::leaving_dungeon_null_room_fix::enabled) {
        if (room == nullptr) return FALSE;
    }

    Vec3i target;
    target.x = (room->centrePos.x << 12) + 0x800;
    target.y = (room->centrePos.y << 12) + 0x800;
    target.z = 0;
    // stop one half-tile before the portal center, on the creature's side
    if (target.y > creature->f16_pos.y) {
        target.y -= 0x400;
    } else {
        target.y += 0x400;
    }
    return this->gotoLoc(&target, 0xC4, 0x6E) != 0;
}
