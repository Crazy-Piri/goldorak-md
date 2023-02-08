#include <genesis.h>

#include "sfx.h"

#define SFX_ROLL            65


void SFX_init(void)
{
    // XGM_setPCM(SFX_JUMP, sonic_jump_sfx, sizeof(sonic_jump_sfx));
    XGM_setPCM(SFX_ROLL, sonic_roll_sfx, sizeof(sonic_roll_sfx));
    // XGM_setPCM(SFX_STOP, sonic_stop_sfx, sizeof(sonic_stop_sfx));
}