#include "goldorak.h"

#define CONTROLLER_STATE_NONE     0
#define CONTROLLER_STATE_ONE_3BTN 1
#define CONTROLLER_STATE_ONE_6BTN 2
#define CONTROLLER_STATE_TWO_3BTN 4
#define CONTROLLER_STATE_TWO_6BTN 8

bool paused;


static u8 controllerState = CONTROLLER_STATE_NONE;

void system_init(u8 *controller)
{
    u8 joyState1, joyState2;



    // // Set the appropriate delta time
    // if (IS_PALSYSTEM) {
    //     VDP_setScreenWidth320();
    //     VDP_setScreenHeight240();
    // } else {
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    // }

    // Detect connected controllers
    joyState1 = JOY_getJoypadType(PORT_1);
    joyState2 = JOY_getJoypadType(PORT_2);

    if (joyState1 == JOY_TYPE_PAD3) {
        *controller |= CONTROLLER_STATE_ONE_3BTN;
    } else if (joyState1 == JOY_TYPE_PAD6) {
        *controller |= CONTROLLER_STATE_ONE_6BTN;
    }

    if (joyState2 == JOY_TYPE_PAD3) {
        *controller |= CONTROLLER_STATE_TWO_3BTN;
    } else if (joyState2 == JOY_TYPE_PAD6) {
        *controller |= CONTROLLER_STATE_TWO_6BTN;
    }

} /* system_init */


void loadImage(const Image *image)
{
    // VDP_loadTileSet(image->tileset, TILE_USER_INDEX, DMA);  // load background tilesets in VRAM
    // TileMap *ctmap = unpackTileMap(image->tilemap, NULL);
    // VDP_setTileMapEx(BG_A, ctmap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 0, 0,  0, 0, image->tilemap->w, image->tilemap->h, CPU); // draw backgrounds

    PAL_fadeOut(0, 63, 20, FALSE);

    VDP_drawImage(BG_B, image, 0, 0);

    PAL_setColors(0, (u16 *)image->palette->data, image->palette->length, CPU);

}



int main(bool hard)
{
    system_init(&controllerState);

    // init SFX
    // SFX_init();
    XGM_setPCM(SFX_ROLL, sonic_roll_sfx, sizeof(sonic_roll_sfx));
    XGM_setPCM(SFX_ARRIMAGE, arrimage_sfx, sizeof(arrimage_sfx));
    XGM_setPCM(SFX_DIE, die_sfx, sizeof(die_sfx));
    XGM_setPCM(SFX_EXPLODE3, explode3_sfx, sizeof(explode3_sfx));
    XGM_setPCM(SFX_GOLDO_EXPLODE, goldo_explode_sfx, sizeof(goldo_explode_sfx));
    XGM_setPCM(SFX_LASER, laser_sfx, sizeof(laser_sfx));
    XGM_setPCM(SFX_SOUND_LIFE, sound_life_sfx, sizeof(sound_life_sfx));
    XGM_setPCM(SFX_MAIL_ARRIVE, mail_arrive_sfx, sizeof(mail_arrive_sfx));

    // start music
    XGM_setLoopNumber(-1);
    // XGM_startPlay(goldorak_music);
    XGM_startPlay(goldo_ingame_music);

    VDP_loadFont(custom_font.tileset, DMA);

    paused = FALSE;

    // set all palette to black
    PAL_setColors(0, (u16 *)palette_black, 64, CPU);


#ifndef DEBUG
    // loadImage(&logo_cpiri);
    // waitPressSTART();

    loadImage(&logo_intro);

    // Wait until START gets released
    while (JOY_readJoypad(JOY_1) & BUTTON_START)
        SYS_doVBlankProcess();

    u16 frame = 0;

    VDP_drawText("BETA 0.2", 0, 0);

#define SPEEDTEXT 20

    // Wait until START is pressed
    while (!(JOY_readJoypad(JOY_1) & BUTTON_START)) {
        frame++;
        if (frame == SPEEDTEXT) {
            VDP_drawText("APPUYEZ SUR START", 11, 15 + 5);
            VDP_drawText("POUR COMMENCER", 13, 17 + 5);
        } else if (frame == SPEEDTEXT * 2) {
            VDP_drawText("                 ", 11, 15 + 5);
            VDP_drawText("              ", 13, 17 + 5);
            frame = 0;
        }
        SYS_doVBlankProcess();
    }

    intro();
#endif /* ifndef DEBUG */

    kprintf("coucou\n");


    // XGM_startPlay(goldo_ingame_music);

    game();

    // loadImage(&logo_cpiri);
    // waitPressSTART();

} /* main */
