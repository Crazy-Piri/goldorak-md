#include "goldorak.h"


bool VDP_drawImage_nopal(VDPPlane plane, const Image *image, u16 x, u16 y)
{
    u16 numTile;
    u16 result;

    numTile = image->tileset->numTile;
    // not enough tiles to display the image, get back to first user index
    if ((curTileInd + numTile) > TILE_USER_MAX_INDEX)
        curTileInd = TILE_USER_INDEX;

    result = VDP_drawImageEx(plane, image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, curTileInd), x, y, FALSE, DMA);

    curTileInd += numTile;

    return result;
}

void part1(void)
{
    curTileInd = TILE_USER_INDEX;

    int frame = 0;
    int step = 50;

    PAL_fadeOut(0, 63, 20, FALSE);

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    VDP_drawImage(BG_B, &goldo_intro_fond, 0, 0);


    while (1) {
        u16 joy1 = JOY_readJoypad(JOY_1);
        if (joy1 & BUTTON_B) {
            break;
        }

        frame++;

        if (frame == step * 1) {
            VDP_drawImage_nopal(BG_B, &goldo_intro_vignette_fr_1, 1, 0);
        } else if (frame == step * 2) {
            VDP_drawImage_nopal(BG_A, &goldo_intro_vignette_fr_2, 17, 1);
        } else if (frame == step * 3) {
            VDP_drawImage_nopal(BG_B, &goldo_intro_vignette_fr_3, 22, 7);
        } else if (frame == step * 4) {
            VDP_drawImage_nopal(BG_A, &goldo_intro_vignette_fr_4, 6, 12);
        } else if (frame == step * 5) {
            break;
        }

        SYS_doVBlankProcess();
    }

} /* part1 */


void part2(void)
{
    curTileInd = TILE_USER_INDEX;

    SPR_init();

    int frame = 0;
    int step = 8;

    PAL_setPalette(0, palette_black, CPU);
    VDP_drawImage(BG_B, &goldo_intro_part2, 0, 0);
    VDP_clearPlane(BG_A, TRUE);

    Sprite *spr = SPR_addSprite(&actarus, 142, 31 + 12, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));  // 7
    SPR_setAnim(spr, 0);

    while (1) {
        u16 joy1 = JOY_readJoypad(JOY_1);
        if (joy1 & BUTTON_B) {
            break;
        }

        frame++;

        if (frame == step * 1) {
            SPR_setPosition(spr, 140, 38 + 12);  // 8
        } else if (frame == step * 2) {
            SPR_setAnim(spr, 1);
            SPR_setPosition(spr, 138, 46 + 12);  // 9
        } else if (frame == step * 3) {
            SPR_setAnim(spr, 2);
            SPR_setPosition(spr, 138, 55 + 12); // 10
        } else if (frame == step * 4) {
            SPR_setAnim(spr, 3);
            SPR_setPosition(spr, 136, 63 + 12); // 11
        } else if (frame == step * 5) {
            SPR_setAnim(spr, 4);
            SPR_setPosition(spr, 136, 70 + 12); // 12
        } else if (frame == step * 6) {
            SPR_setAnim(spr, 5);
            SPR_setPosition(spr, 136, 76 + 12); // 13
        } else if (frame == step * 7) {
            SPR_setAnim(spr, 5);
            SPR_setPosition(spr, 138, 81 + 12); // 14
        } else if (frame == step * 8) {
            SPR_setAnim(spr, 5);
            SPR_setPosition(spr, 134, 87 + 12); // 15
        } else if (frame == step * 9) {
            SPR_setAnim(spr, 5);
            SPR_setPosition(spr, 134, 92 + 12); // 16
        } else if (frame == step * 10) {
            SPR_setAnim(spr, 5);
            SPR_setPosition(spr, 132, 96 + 12); // 17
        } else if (frame == step * 11) {
            break;
        }

        SPR_update();
        SYS_doVBlankProcess();
    }

    SPR_releaseSprite(spr);

} /* part2 */


void part3(void)
{
    curTileInd = TILE_USER_INDEX;

    int frame = 0;
    u8 frame3;
    u8 frame16;
    int step = 50;

    PAL_setPalette(0, palette_black, CPU);
    VDP_drawImage(BG_B, &goldo_intro_fond, 0, 0);

    while (1) {
        u16 joy1 = JOY_readJoypad(JOY_1);
        if (joy1 & BUTTON_B) {
            break;
        }

        frame++;

        if (frame == step * 1) {
            VDP_drawImage_nopal(BG_B, &goldo_intro_vignette_fr_5, 3, 15);
        } else if (frame == step * 2) {
            VDP_drawImage_nopal(BG_A, &goldo_intro_vignette_fr_6, 17, 20);
        } else if (frame == step * 3) {
            break;
        }

        SYS_doVBlankProcess();
    }

    u16 anim[3];

    anim[0] = TILE_USER_INDEX + goldo_intro_fond.tileset->numTile + goldo_intro_vignette_fr_6.tileset->numTile + goldo_intro_vignette_fr_5.tileset->numTile;
    VDP_loadTileSet(goldo_intro_vignette_fr_5.tileset, anim[0], DMA);

    anim[1] = anim[0] + goldo_intro_vignette_fr_5.tileset->numTile;
    VDP_loadTileSet(goldo_intro_vignette_fr_7.tileset, anim[1], DMA);

    anim[2] = anim[1] + goldo_intro_vignette_fr_7.tileset->numTile;
    VDP_loadTileSet(goldo_intro_vignette_fr_8.tileset, anim[2], DMA);


    frame = 0;
    frame3 = 0;
    frame16 = 0;

    u8 speed = 6;

    while (1) {
        u16 joy1 = JOY_readJoypad(JOY_1);
        if (joy1 & BUTTON_B) {
            break;
        }

        frame++;

        frame16++;
        if (frame16 > speed) {
            frame16 = 0;

            frame3++;
            if (frame3 == 3) {
                frame3 = 0;
            }

            if (frame3 == 0) {
                VDP_setTileMapEx(BG_B, goldo_intro_vignette_fr_5.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, anim[0]), 3, 15, 0, 0, 136 / 8, 88 / 8, CPU);
            } else if (frame3 == 1) {
                VDP_setTileMapEx(BG_B, goldo_intro_vignette_fr_7.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, anim[1]), 3, 15, 0, 0, 136 / 8, 88 / 8, CPU);
            } else {
                VDP_setTileMapEx(BG_B, goldo_intro_vignette_fr_8.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, anim[2]), 3, 15, 0, 0, 136 / 8, 88 / 8, CPU);
            }

            // waitPressSTART();

        }

        if (frame == step * 1) {
            speed--;
        } else if (frame == step * 2) {
            speed--;
        } else if (frame == step * 3) {
            speed--;
        } else if (frame == step * 4) {
            speed--;
        } else
        if (frame == step * 5) {
            break;
        }

        SYS_doVBlankProcess();
    }

} /* part3 */

void intro(void)
{
    part1();

    part2();

    part3();

    curTileInd = TILE_USER_INDEX; // restore curTileInd

} /* intro */
