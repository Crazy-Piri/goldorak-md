#include <genesis.h>

#include "goldorak.h"

#include "gfx.h"

#define MONSTER_EXPLOSION_TYPE 254

#ifdef DEBUG
#define FRAME_PER_ANIM         1
#else
#define FRAME_PER_ANIM         8
#endif

typedef struct monster_sequence {
    u8 w;
    u8 p;
    u8 m;
    u16 x, y;
    u16 xd, yd;
    u16 r;
    u8 dir;
    u16 w1;
    u8 h;
    u8 e;
    // {w=50,p=1,m=1,y=5,xd=280,r=1000,dir=1},

    s8 blink;

    Sprite *sprite;
} monster_sequencep;

typedef struct sFireMonster {
    u16 x, y;
    s8 dirx, diry;
    u8 type;

    Sprite *sprite;
} tFireMonster;


int monster_hitx[] = {6, 8, 16, 10, 10, 15};
int monster_hity[] = {0, 25, 0, 6, 6, 5};
int monster_hitwidht[] = {26, 36, 22, 30, 30, 26};
int monster_hitheight[] = {27, 29, 41, 29, 29, 21};


int monster_step[16];
monster_sequencep monster_def[16];
int monsterx[16];
int monstery[16];

int firex[16];
int firey[16];
int fire_type[16];
int fire_step[16];
Sprite *fire_sprite[16];

int firemonster_step[16];
tFireMonster firemonster[16];

void game_loop(void);
void initGame(void);
void initLevel(u8 l);
void displayFire(void);
void displayMonster(void);
void detectCollision(void);
void detectMonsterFireCollision(void);
void monster(monster_sequencep *m);

int offset;
Sprite *goldorakSprite = NULL;

Sprite *goldo_anim_lonely_sprite;
Sprite *goldo_anim_join_sprite;

int goldo_x, goldo_y;
int goldo_accelx;
int frame, waveframe, wavepos;
int lostLevel;

int keyXPressed;
int goldorak_launch_count;
int goldorak_launch_count_type;
int goldorak_launch_uniq;

int displayedMonster;

bool nextWave;

int level;
int life;
int score;

u16 hud_onoff_idx, hud_goldo_idx;

Map *levelMap;

#define NBLEVEL 2

monster_sequencep wavedef[] =
{
    {.w = 50, .p = 1, .m = 1, .y = 5, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 15, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 25, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 35, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 45, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 55, .r = 500, .dir = 1},

    {.w = 150, .p = 1, .m = 1, .y = 5, .xd = 20, .r = 1000, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 72, .r = 1000, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 124, .r = 1000, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 176, .r = 1000, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 228, .r = 1000, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 280, .r = 1000, .dir = 1},

    {.w = 50, .p = 0, .m = 1, .y = 35, .xd = 270, .r = 1000, .dir = 1},
    {.w = 50, .p = 0, .m = 1, .y = 35, .xd = 218, .r = 1000, .dir = 1},
    {.w = 50, .p = 0, .m = 1, .y = 35, .xd = 166, .r = 1000, .dir = 1},
    {.w = 50, .p = 0, .m = 1, .y = 35, .xd = 114, .r = 1000, .dir = 1},
    {.w = 50, .p = 0, .m = 1, .y = 35, .xd = 62, .r = 1000, .dir = 1},
    {.w = 50, .p = 0, .m = 1, .y = 35, .xd = 10, .r = 1000, .dir = 1},

    {.w1 = 1, .p = 3, .m = 1},

    {.w1 = 1, .p = 4, .m = 6, .x = 40, .y = 4, .h = 5, .r = 40},

    {.w1 = 1, .p = 1, .m = 1, .y = 5, .xd = 20, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 72, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 124, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 176, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 228, .r = 500, .dir = 1},
    {.w = 50, .p = 1, .m = 1, .y = 5, .xd = 280, .r = 500, .dir = 1},


    {.w1 = 1, .p = 0, .m = 3, .r = 100},
    {.w = 100, .p = 0, .m = 3, .r = 100},

    {.w1 = 1, .p = 2, .x = 218, .m = 1},
    {.w = 50, .p = 2, .x = 218, .m = 1},
    {.w = 50, .p = 2, .x = 218, .m = 1},
    {.w = 50, .p = 2, .x = 218, .m = 1},

    {.w1 = 1, .p = 2, .x = 62, .m = 1},
    {.w = 50, .p = 2, .x = 62, .m = 1},
    {.w = 50, .p = 2, .x = 62, .m = 1},
    {.w = 50, .p = 2, .x = 62, .m = 1},

    {.w = 50, .p = 1, .m = 1},
    {.w = 50, .p = 1, .m = 1},
    {.w = 50, .p = 1, .m = 1},
    {.w = 50, .p = 1, .m = 1},
    {.w = 50, .p = 1, .m = 1},
    {.w = 50, .p = 1, .m = 1},

    {.w = 250, .p = 4, .m = 2, .x = 40, .y = 4, .h = 5, .r = 40},

    {.w1 = 1, .e = 1}
};


void goldoAnim_firstframe(void)
{
    goldo_anim_join_sprite = SPR_addSprite(&goldo_anim_join, (320 - 64) / 2, 10, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
    goldo_anim_lonely_sprite = SPR_addSprite(&goldo_anim_lonely, (320 - 64) / 2, 146 - 1, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));

    SPR_setVisibility(goldo_anim_join_sprite, VISIBLE);
    SPR_setVisibility(goldo_anim_lonely_sprite, VISIBLE);
}

void goldoAnim_lastframe(void)
{
    goldo_x = (320 - 64) / 2;
    goldo_y = 10;

    goldorakSprite = SPR_addSprite(&goldorak_launch_001, goldo_x, goldo_y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
    SPR_setVisibility(goldorakSprite, VISIBLE);

    SPR_setVisibility(goldo_anim_join_sprite, HIDDEN);
    SPR_releaseSprite(goldo_anim_join_sprite);

    SPR_setVisibility(goldo_anim_lonely_sprite, HIDDEN);
    SPR_releaseSprite(goldo_anim_lonely_sprite);

}

void showLife(void)
{
    u8 n;

    for (n = 0; n < life; n++) {
        VDP_setTileMapEx(BG_A, hud_goldo.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, hud_goldo_idx), 7 + 2 * n, 25, 0, 0, 2, 2, CPU); // On
    }
    for (n = life; n < 4; n++) {
        VDP_setTileMapEx(BG_A, hud_goldo.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, hud_goldo_idx), 7 + 2 * n, 25, 0, 2, 2, 2, CPU); // Off
    }
}

void setArmeLeft(u8 arme, u8 on)
{
    VDP_setTileMapEx(BG_A, hud_onoff.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, hud_onoff_idx), 7 + 2 * arme, 22, 0, on * 2, 2, 2, CPU); // Off
}

void setArmeRight(u8 arme, u8 on)
{
    VDP_setTileMapEx(BG_A, hud_onoff.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, hud_onoff_idx), 25 + 2 * arme, 22, 0, on * 2, 2, 2, CPU); // Off
}


void goldoAnim(void)
{
    if (frame == 1) {
        u8 n;

        for (n = 0; n < 4; n++) {
            setArmeLeft(n, 0);
            setArmeRight(n, 0);
        }

        goldoAnim_firstframe();
    }

    if (frame < FRAME_PER_ANIM * 5) {
        SPR_setPosition(goldo_anim_join_sprite, (320 - 64) / 2, 18);
        SPR_setPosition(goldo_anim_lonely_sprite, (320 - 64) / 2, 146 - frame);

        SPR_setAnim(goldo_anim_lonely_sprite, 0);
        SPR_setAnim(goldo_anim_join_sprite, 0);

        // mydraw(goldo_anim_01, (320-64)/2,10)
        // mydraw(goldo_anim_03, (320-44)/2,146-frame)
    } else if (frame < FRAME_PER_ANIM * 7 ) {
        SPR_setPosition(goldo_anim_join_sprite, (320 - 64) / 2, 18);
        SPR_setPosition(goldo_anim_lonely_sprite, (320 - 64) / 2, 146 - frame);

        SPR_setAnim(goldo_anim_lonely_sprite, 1);
        SPR_setAnim(goldo_anim_join_sprite, 0);

        // mydraw(goldo_anim_02, (320-64)/2,10)
        // mydraw(goldo_anim_03, (320-44)/2,146-frame)
    } else if ( frame < FRAME_PER_ANIM * 11 ) {
        SPR_setPosition(goldo_anim_join_sprite, (320 - 64) / 2, 18);
        SPR_setPosition(goldo_anim_lonely_sprite, (320 - 64) / 2, 146 - frame);

        SPR_setAnim(goldo_anim_lonely_sprite, 1);
        SPR_setAnim(goldo_anim_join_sprite, 1);

        // mydraw(goldo_anim_02, (320-64)/2,10)
        // mydraw(goldo_anim_04, (320-16)/2,146-frame)
    } else if ( frame < FRAME_PER_ANIM * 12 ) {

        if (frame == FRAME_PER_ANIM * 11) {
            XGM_startPlayPCM(SFX_ARRIMAGE, 1, SOUND_PCM_CH2);
        }

        SPR_setPosition(goldo_anim_join_sprite, (320 - 64) / 2, 18);

        SPR_setVisibility(goldo_anim_lonely_sprite, HIDDEN);

        SPR_setAnim(goldo_anim_join_sprite, 2);

        // mydraw(goldo_anim_05, (320-64)/2,10)
    } else if ( frame < FRAME_PER_ANIM * 13 ) {
        SPR_setPosition(goldo_anim_join_sprite,  (320 - 64) / 2, 11);

        SPR_setAnim(goldo_anim_join_sprite, 3);

        // mydraw(goldo_anim_06, (320-64)/2,10)
    } else if ( frame < FRAME_PER_ANIM * 14 ) {
        SPR_setPosition(goldo_anim_join_sprite, (320 - 64) / 2, 3);

        SPR_setAnim(goldo_anim_join_sprite, 4);

        // mydraw(goldo_anim_07, (320-64)/2,10-(40-26))
    } else if ( frame < FRAME_PER_ANIM * 15 ) {
        SPR_setPosition(goldo_anim_join_sprite, (320 - 64) / 2, 2);

        SPR_setAnim(goldo_anim_join_sprite, 5);

        // mydraw(goldo_anim_08, (320-64)/2,10-(34-26))
    } else {
        // mydraw(goldorak_launch_001_00, (320-64)/2,10-(33-26)+(frame-framePerIdx*15)*2)

        if (frame == FRAME_PER_ANIM * 16) {
            setArmeLeft(0, 1);
            setArmeRight(3, 1);
        }
        if (frame == FRAME_PER_ANIM * 17) {
            setArmeLeft(0, 0);
            setArmeLeft(1, 1);
            setArmeRight(3, 0);
            setArmeRight(2, 1);
        }
        if (frame == FRAME_PER_ANIM * 18) {
            setArmeLeft(1, 0);
            setArmeLeft(2, 1);
            setArmeRight(2, 0);
            setArmeRight(1, 1);
        }
        if (frame == FRAME_PER_ANIM * 19) {
            setArmeLeft(2, 0);
            setArmeLeft(3, 1);
            setArmeRight(1, 0);
            setArmeRight(0, 1);
        }

        if (frame == FRAME_PER_ANIM * 20) {
            setArmeLeft(3, 0);
            setArmeLeft(0, 1);
            setArmeRight(0, 0);
            goldoAnim_lastframe();
        }
    }


    // love.graphics.setFont(font_12)

    // local infoX

    // if frame < 40 then
    //     infoX = frame-30
    // elseif frame < 180 then
    //     infoX = 10
    // else
    //     infoX = 10-(frame-180)*4
    // end

    // love.graphics.print("Level " .. level,infoX,124)
    // love.graphics.print(levelName,infoX,140)

} /* goldoAnim */


bool VDP_drawImage_nopal_priority(VDPPlane plane, const Image *image, u16 x, u16 y)
{
    u16 numTile;
    u16 result;

    numTile = image->tileset->numTile;
    // not enough tiles to display the image, get back to first user index
    if ((curTileInd + numTile) > TILE_USER_MAX_INDEX)
        curTileInd = TILE_USER_INDEX;

    result = VDP_drawImageEx(plane, image, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, curTileInd), x, y, FALSE, DMA);

    curTileInd += numTile;

    return result;
}

void game(void)
{
    PAL_fadeOut(0, 63, 20, FALSE);

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    SPR_init();

    curTileInd = TILE_USER_INDEX;
    VDP_drawImage_nopal_priority(BG_A, &hud, 0, (224 - 48) / 8);

    u16 idx = TILE_USER_INDEX + hud.tileset->numTile;
    VDP_loadTileSet(&level1_tileset, idx, DMA);
    levelMap = MAP_create(&level1_map, BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx));

    hud_onoff_idx = idx + level1_tileset.numTile;
    VDP_loadTileSet(hud_onoff.tileset, hud_onoff_idx, DMA);

    hud_goldo_idx = hud_onoff_idx + hud_onoff.tileset->numTile;
    VDP_loadTileSet(hud_goldo.tileset, hud_goldo_idx, DMA);

    PAL_setColors(0, sprites_pal.data, 16, CPU);
    PAL_setColors(32, level1_pal.data, 16, CPU);


    // VDP_drawImage(BG_B, &background_01, 0, 0);
    // VDP_drawImage(BG_B, &background_07, 0, 0);


    initGame();

    while (1)
        game_loop();

} /* game */

void monster(monster_sequencep *m)
{
    int n;

    for (n = 0; n < 16; n++) {

        if (monster_step[n] == 0) {
            memcpy(&monster_def[n], m, sizeof(monster_sequencep));

            monster_step[n] = 1;

            if (m->p == 0) {
                monsterx[n] = (m->x != 0) ? m->x : -50;
                monstery[n] = (m->y != 0) ? m->y : 10;

                monster_def[n].xd = (m->xd != 0) ? m->xd : 320;

            } else if (m->p == 1) {
                monsterx[n] = (m->x != 0) ? m->x : 320;
                monstery[n] = (m->y != 0) ? m->y : 50;

                monster_def[n].xd = (m->xd != 0) ? m->xd : -50;

            } else if ( m->p == 2 ) {
                if (m->x != 0) {
                    monsterx[n] = m->x;
                } else {
                    monsterx[n] = 230;
                }
                if (m->y != 0) {
                    monstery[n] = m->y;
                } else {
                    monstery[n] = -50;
                }
            } else if ( m->p == 3 ) {
                if (m->y != 0) {
                    monstery[n] = m->y;
                } else {
                    monstery[n] = -50;
                }
            } else if ( m->p == 4 ) {
                if (m->x != 0) {
                    monsterx[n] = m->x;
                } else {
                    monsterx[n] = 100;
                }
                if (m->y != 0) {
                    monstery[n] = m->y;
                } else {
                    monstery[n] = 50;
                }
            }

            if (m->h == 0) {
                monster_def[n].h = 1;
            }
            if (m->r == 0) {
                monster_def[n].r = 0;
            }
            if (m->dir == 0) {
                monster_def[n].dir = 0;
            }

            switch (monster_def[n].m) {
                case 1:
                    monster_def[n].sprite = SPR_addSprite(&vega, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 2:
                    monster_def[n].sprite = SPR_addSprite(&golgoth1, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 3:
                    monster_def[n].sprite = SPR_addSprite(&golgoth2, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 4:
                    monster_def[n].sprite = SPR_addSprite(&golgoth3, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 5:
                    monster_def[n].sprite = SPR_addSprite(&golgoth4, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 6:
                    monster_def[n].sprite = SPR_addSprite(&golgoth5, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                default:
                    break;
            } /* switch */

            return;
        }

    }

} /* monster */

void initGame(void)
{
    level = 0;
    life = 4;

    score = 0;

    initLevel(level);
}

void initLevel(u8 l)
{
    int n;

    goldorak_launch_count_type = 0;

    for (n = 0; n < 16; n++) {
        if (fire_step[n] != 0) {
            SPR_releaseSprite(fire_sprite[n]);
            fire_step[n] = 0;
        }

        firex[n] = 0;
        firey[n] = 0;
        fire_type[n] = 0;
    }

    for (n = 0; n < 16; n++) {
        if (monster_step[n] != 0) {
            SPR_releaseSprite(monster_def[n].sprite);
            monster_step[n] = 0;
        }

        monsterx[n] = 0;
        monstery[n] = 0;
    }

    for (n = 0; n < 16; n++) {
        if (firemonster_step[n] != 0) {
            SPR_releaseSprite(firemonster[n].sprite);
            firemonster_step[n] = 0;
        }
    }

    if (goldorakSprite != NULL) {
        SPR_releaseSprite(goldorakSprite);
    }

    lostLevel = 0;

    goldo_x = 150;
    goldo_y = 40;

    wavepos = 0;

    frame = 0;

    waveframe = 0;

    offset = 2016 - (224 - 48);

    showLife();
} /* initLevel */

void displayFire(void)
{
    int n;

    for (n = 0; n < 16; n++) {

        if (fire_step[n] != 0) {
            fire_step[n] = fire_step[n] + 1;

            if (fire_type[n] == 0) {
                SPR_setPosition(fire_sprite[n], firex[n], firey[n]);

                firey[n] = firey[n] - 2;
                if (firey[n] < -10) {
                    fire_step[n] = 0;
                    SPR_releaseSprite(fire_sprite[n]);
                }
            }

            if (fire_type[n] == 1) {
                if (fire_step[n] < 60) {
                    firey[n] = firey[n] - 1;
                } else {
                    if (firex[n] > goldo_x + 24) {
                        firex[n] = firex[n] - 2;
                    }

                    if (firex[n] < goldo_x + 24) {
                        firex[n] = firex[n] + 2;
                    }

                    if (firey[n] < goldo_y) {
                        firey[n] = firey[n] + 2;
                    } else if (firey[n] > goldo_y) {
                        firey[n] = firey[n] - 1;
                    } else if (firex[n] == goldo_x + 24) {
                        fire_step[n] = 0;
                        goldorak_launch_count = 10;
                        SPR_releaseSprite(fire_sprite[n]);
                    }
                }

                SPR_setPosition(fire_sprite[n], firex[n], firey[n]);
            }

            if (fire_type[n] == 2) {
                int framePerIdx = 10;

                firex[n] = goldo_x + 25;
                firey[n] = goldo_y - 28;

                SPR_setPosition(fire_sprite[n], firex[n], firey[n]);

                if (fire_step[n] < 1 * framePerIdx) {
                    // mydraw(cornofulgure_01, firex[n], firey[n]);
                } else if (fire_step[n] < 16 * framePerIdx) {
                    // int i = (fire_step[n]-1*framePerIdx)/framePerIdx)%2;
                    // if (i == 1) {
                    //     mydraw(cornofulgure_02, firex[n], firey[n]);
                    // } else {
                    //     mydraw(cornofulgure_03, firex[n], firey[n]);
                    // }
                } else if (fire_step[n] < 17 * framePerIdx) {
                    // mydraw(cornofulgure_01, firex[n], firey[n]);
                } else {
                    SPR_releaseSprite(fire_sprite[n]);
                    fire_step[n] = 0;
                    goldorak_launch_uniq = -1; // ready to launch another
                }
            }

        }
    }

} /* displayFire */

void drawMonster(Sprite *sprite, int n)
{
    if (monster_def[n].blink != 0) {
        monster_def[n].blink--;

        int i = (monster_def[n].blink / 4) % 2;
        SPR_setVisibility(sprite, i);

        if (monster_def[n].blink == 0) {
            SPR_setVisibility(sprite, TRUE);
            monster_def[n].blink = -1;
        }
    }

    SPR_setPosition(sprite, monsterx[n], monstery[n]);

} /* drawMonster */


void do_firemonster(u16 gx, u16 gy, int type, int dirx, int dy, int sx, int sy)
{
    u16 n;

    for (n = 0; n < 16; n++) {
        if (firemonster_step[n] == 0) {

            int dx = 0;

            if (dirx != 0) {
                dx = (goldo_x - (monsterx[n] + sx)) / (goldo_y - (monstery[n] + sy));
                if (dx > 2) {
                    dx = 2;
                }
                if (dx < -2) {
                    dx = -2;
                }
            }

            firemonster[n].x = gx + sx;
            firemonster[n].y = gy + sy;
            firemonster[n].dirx = dx;
            firemonster[n].diry = dy;
            firemonster[n].type = type;
            firemonster_step[n] = 1;

            switch (firemonster[n].type) {
                case 1:
                    firemonster[n].sprite = SPR_addSprite(&fire_monster_1, firemonster[n].x, firemonster[n].y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 2:
                    firemonster[n].sprite = SPR_addSprite(&fire_monster_2, firemonster[n].x, firemonster[n].y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 3:
                    firemonster[n].sprite = SPR_addSprite(&fire_monster_3, firemonster[n].x, firemonster[n].y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                case 4:
                    firemonster[n].sprite = SPR_addSprite(&fire_monster_4, firemonster[n].x, firemonster[n].y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
                default:
                    firemonster[n].sprite = SPR_addSprite(&fire_monster_1, firemonster[n].x, firemonster[n].y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                    break;
            }

            return;
        }
    }
} /* do_firemonster */

void monsterFireAndCollision(void)
{
    int n;


    for (n = 0; n < 16; n++) {

        if (firemonster_step[n] != 0) {

            firemonster[n].x = firemonster[n].x + firemonster[n].dirx;
            firemonster[n].y = firemonster[n].y + firemonster[n].diry;

            SPR_setPosition(firemonster[n].sprite, firemonster[n].x, firemonster[n].y);

            if ( (firemonster[n].y > 180) || (firemonster[n].x > 320) || (firemonster[n].x < 10)) {
                firemonster_step[n] = 0;
                SPR_releaseSprite(firemonster[n].sprite);
            }

        }

    }

    if (lostLevel == 0) {
        detectMonsterFireCollision();
    }

} /* monsterFireAndCollision */

void displayMonster(void)
{

    displayedMonster = 0;

    int n;

    for (n = 0; n < 16; n++) {

        if (monster_step[n] != 0) {
            displayedMonster = displayedMonster + 1;

            monster_step[n] = monster_step[n] + 1;

            if (monster_def[n].m == 1) {
                drawMonster(monster_def[n].sprite, n);
                if ((monster_def[n].r != 0) && ((random() % monster_def[n].r) == 0)) {
                    do_firemonster(monsterx[n], monstery[n], 2, monster_def[n].dir, 2, 17, 27);
                }

            } else if (monster_def[n].m == 2) {
                drawMonster(monster_def[n].sprite, n);
                if ((monster_def[n].r != 0) && ((random() % monster_def[n].r) == 0)) {
                    do_firemonster(monsterx[n], monstery[n], 1, monster_def[n].dir, 2, 24, 71);
                }

            } else if (monster_def[n].m == 3) {
                drawMonster(monster_def[n].sprite, n);
                if ((monster_def[n].r != 0) && ((random() % monster_def[n].r) == 0)) {
                    do_firemonster(monsterx[n], monstery[n], 4, monster_def[n].dir, 2, 25, 41);;
                }

            } else if (monster_def[n].m == 4) {
                drawMonster(monster_def[n].sprite, n);
                if ((monster_def[n].r != 0) && ((random() % monster_def[n].r) == 0)) {
                    do_firemonster(monsterx[n], monstery[n], 3, monster_def[n].dir, 2, 18, 35);
                }

            } else if (monster_def[n].m == 5) {
                drawMonster(monster_def[n].sprite, n);
                if ((monster_def[n].r != 0) && ((random() % monster_def[n].r) == 0)) {
                    do_firemonster(monsterx[n], monstery[n], 3, monster_def[n].dir, 2, 18, 35);
                }

            } else if (monster_def[n].m == 6) {
                drawMonster(monster_def[n].sprite, n);
                if ((monster_def[n].r != 0) && ((random() % monster_def[n].r) == 0)) {
                    do_firemonster(monsterx[n], monstery[n], 4, monster_def[n].dir, 2, 25, 25);
                }

            } else if (monster_def[n].m == MONSTER_EXPLOSION_TYPE) {
                if (monster_step[n] == 25) {
                    monster_step[n] = 0;
                    SPR_releaseSprite(monster_def[n].sprite);
                }
            }

            if (monster_def[n].p == 0 ) {
                if (monsterx[n] != monster_def[n].xd) {
                    monsterx[n] = monsterx[n] + 1;
                    if (monsterx[n] == 320) {
                        monster_step[n] = 0;
                        SPR_releaseSprite(monster_def[n].sprite);
                    }
                } else {
                    monster_def[n].p = 5;
                }
            }

            if (monster_def[n].p == 1 ) {
                if (monsterx[n] != monster_def[n].xd) {
                    monsterx[n] = monsterx[n] - 1;
                    if (monsterx[n] == -50) {
                        monster_step[n] = 0;
                        SPR_releaseSprite(monster_def[n].sprite);
                    }

                } else {
                    monster_def[n].p = 5;
                }
            }

            if (monster_def[n].p == 2 ) {
                monstery[n] = monstery[n] + 2;
                if (monstery[n] >= 200) {
                    monster_step[n] = 0;
                    SPR_releaseSprite(monster_def[n].sprite);
                }
            }

            if (monster_def[n].p == 3 ) {
                monsterx[n] = 0; // math.sin(monster_step[n] / 25) * ((400 - monster_step[n]) / 2) + 160 - 20;
                monstery[n] = monster_step[n] / 2 - 50;
                if (monstery[n] >= 200) {
                    monster_step[n] = 0;
                    SPR_releaseSprite(monster_def[n].sprite);
                }
            }

            if (monster_def[n].p == 4 ) {
                u8 dir = (monster_step[n] / 100) % 2;
                if (dir == 0) {
                    monsterx[n] = monsterx[n] + 2;
                    if (monsterx[n] > 260) {
                        monsterx[n] = 260;
                    }
                } else {
                    monsterx[n] = monsterx[n] - 2;
                    if (monsterx[n] < 20) {
                        monsterx[n] = 20;
                    }
                }
            }

            if (monster_def[n].p == 5 ) {
                u8 dir = (frame / 50) % 2;
                if (dir == 0) {
                    monsterx[n] = monsterx[n] + 1;
                } else {
                    monsterx[n] = monsterx[n] - 1;
                }

            }

        }
    }

} /* displayMonster */


void cursor(void)
{
    u16 joy1 = JOY_readJoypad(JOY_1);

    if (joy1 & BUTTON_UP) {
        goldo_y--;
        if (goldo_y < 30) goldo_y = 30;
    } else if (joy1 & BUTTON_DOWN) {
        goldo_y += 2;
        if (goldo_y > 160) goldo_y = 160;
    }

    if (joy1 & BUTTON_LEFT) {
        goldo_accelx--;
        if (goldo_accelx < -16) {
            goldo_accelx = -16;
        }
    } else if (joy1 & BUTTON_RIGHT) {
        goldo_accelx++;
        if (goldo_accelx > 16) {
            goldo_accelx = 16;
        }
    } else {
        if (goldo_accelx < 0) {
            goldo_accelx++;
        } else if (goldo_accelx > 0) {
            goldo_accelx--;
        }
    }

    if (goldo_accelx < 0) {
        goldo_x += (goldo_accelx / 4);
        if (goldo_x < -32) goldo_x = -32;
        SPR_setFrame(goldorakSprite, 1);
    } else if (goldo_accelx > 0) {
        goldo_x += (goldo_accelx / 4);
        if (goldo_x > 320 - 32) goldo_x = 320 - 32;
        SPR_setFrame(goldorakSprite, 2);
    } else {
        SPR_setFrame(goldorakSprite, 0);
    }


    if (joy1 & BUTTON_B) {
        if (keyXPressed == 0) {
            keyXPressed = 1;

            setArmeLeft(goldorak_launch_count_type, 0);


            if (goldorak_launch_count_type == 0) {

                SPR_setAnim(goldorakSprite, 1);
                goldorak_launch_count_type = 1;
                goldorak_launch_uniq = -1;
            } else if ((goldorak_launch_count_type == 1) && (goldorak_launch_uniq == -1)) {

                SPR_setAnim(goldorakSprite, 2);
                goldorak_launch_count_type = 2;
            } else if ((goldorak_launch_count_type == 2) && (goldorak_launch_uniq == -1)) {

                SPR_setAnim(goldorakSprite, 0);
                goldorak_launch_count_type = 0;
            }

            setArmeLeft(goldorak_launch_count_type, 1);
        }
    } else {
        keyXPressed = 0;
    }


    if ( (joy1 & BUTTON_A) && (goldorak_launch_count == 0)) {

        // XGM_startPlayPCM(SFX_ROLL, 1, SOUND_PCM_CH2);
        XGM_startPlayPCM(SFX_LASER, 1, SOUND_PCM_CH2);

        if (goldorak_launch_count_type == 0) {
            goldorak_launch_count = 20;
        }

        if ((goldorak_launch_count_type == 1) & (goldorak_launch_uniq == -1)) {
            goldorak_launch_count = 20;  // -- 100ms * 2  (50 fps = 1; frame = 20ms)
        }

        if ((goldorak_launch_count_type == 2) & (goldorak_launch_uniq == -1)) {
            goldorak_launch_count = 20; //  -- 100ms * 2  (50 fps = 1; frame = 20ms)
        }

    }

} /* cursor */


bool testCollision(int x0, int y0, int width0, int height0, int x1, int y1, int monster1)
{
    if (monster1 == MONSTER_EXPLOSION_TYPE) {
        return FALSE;
    }

    // -- print("Testcollision: " .. monster1);

    x1 = x1 + monster_hitx[monster1 - 1];
    y1 = y1 + monster_hity[monster1 - 1];

    int width1 = monster_hitwidht[monster1 - 1];
    int height1 = monster_hitheight[monster1 - 1];

    if ((x0 > x1 + width1) || (x1 > x0 + width0)) {
        return FALSE;
    }

    if ((y0 > y1 + height1) ||  (y1 > y0 + height0)) {
        return FALSE;
    }

    return TRUE;
} /* testCollision */

void update_handleGame(void)
{
    if (frame > FRAME_PER_ANIM * 20) {
        if (lostLevel == 0) {
            cursor();
            SPR_setPosition(goldorakSprite, goldo_x, goldo_y);
        }
    }

} /* update_handleGame */

void boum(int m)
{
    monster_def[m].h = monster_def[m].h - 1;
    if (monster_def[m].h <= 0) {

        if (monster_def[m].m == 1) {
            monsterx[m] = monsterx[m] + 3;
            monstery[m] = monstery[m] + 3;
        } else if (monster_def[m].m == 2) {
            monsterx[m] = monsterx[m] + 9;
            monstery[m] = monstery[m] + 24;
        }

        monster_def[m].m = MONSTER_EXPLOSION_TYPE;
        monster_def[m].p = 200;
        monster_step[m] = 1;

        SPR_releaseSprite(monster_def[m].sprite);
        monster_def[m].sprite = SPR_addSprite(&explosion, 0, 0, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));

        SPR_setPosition(monster_def[m].sprite, monsterx[m], monstery[m]);

        XGM_startPlayPCM(SFX_EXPLODE3, 1, SOUND_PCM_CH2);



    } else {
        monster_def[m].blink = 50;
    }
    score = score + 10;

} /* boum */

bool testMonsterfireCollision(int x0, int y0, int width0, int height0, int n)
{
    int x1 = firemonster[n].x;
    int y1 = firemonster[n].y;

    int width1 = 4;
    int height1 = 7;

    if ((x0 > x1 + width1) || (x1 > x0 + width0)) {
        return FALSE;
    }

    if ((y0 > y1 + height1) || (y1 > y0 + height0)) {
        return FALSE;
    }

    return TRUE;
}

void goldorakExplode(void)
{
    lostLevel = 1;


    XGM_startPlayPCM(SFX_GOLDO_EXPLODE, 1, SOUND_PCM_CH2);

    SPR_releaseSprite(goldorakSprite);
    goldorakSprite = SPR_addSprite(&explosion, goldo_x + 16, goldo_y + 6, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));

}

void detectMonsterFireCollision(void)
{
    int n;
    char disable_fire[16];

    for (n = 0; n < 16; n++) {
        disable_fire[n] = 0;
    }

    for (n = 0; n < 16; n++) {
        if (firemonster_step[n] != 0) {
            if (testMonsterfireCollision(goldo_x + 16, goldo_y, 32, 33, n)) {
                goldorakExplode();
                disable_fire[n] = 1;
            }
        }
    }

    for (n = 0; n < 16; n++) {
        if (disable_fire[n] == 1) {
            firemonster_step[n] = 0;
            SPR_releaseSprite(firemonster[n].sprite);
        }
    }

} /* detectMonsterFireCollision */


void detectCollision(void)
{
    int disable_fire[16];
    int m, n;

    for (n = 0; n < 16; n++) {
        disable_fire[n] = 0;
    }


    for (m = 0; m < 16; m++) {
        if ((monster_step[m] != 0) && (monster_def[m].m != MONSTER_EXPLOSION_TYPE)) {

            // -- test with fire

            for (n = 0; n < 16; n++) {
                if (fire_step[n] != 0) {

                    if (fire_type[n] == 0) {
                        if (testCollision(firex[n], firey[n], 2, 6, monsterx[m], monstery[m], monster_def[m].m)) {
                            boum(m);
                            disable_fire[n] = 1;
                        }
                        if (testCollision(firex[n] + 50, firey[n], 2, 6, monsterx[m], monstery[m], monster_def[m].m)) {
                            boum(m);
                            disable_fire[n] = 1;
                        }
                    }

                    if (fire_type[n] == 1) {
                        if (testCollision(firex[n], firey[n], 16, 8, monsterx[m], monstery[m], monster_def[m].m)) {
                            boum(m);
                        }
                    }

                    if (fire_type[n] == 2) {
                        if (testCollision(firex[n], firey[n], 14, 29, monsterx[m], monstery[m], monster_def[m].m)) {
                            boum(m);
                        }
                    }

                }
            }

            // -- test with goldorak

            if (testCollision(goldo_x + 16, goldo_y, 32, 33, monsterx[m], monstery[m], monster_def[m].m)) {

                goldorakExplode();
            }


        }
    }

    for (n = 0; n < 16; n++) {
        if (disable_fire[n] == 1) {
            fire_step[n] = 0;
            SPR_releaseSprite(fire_sprite[n]);
        }
    }
} /* detectCollision */

void fire(u16 gx, u16 gy, int type)
{
    u16 n;

    for (n = 0; n < 16; n++) {
        if (fire_step[n] == 0) {
            firex[n] = gx;
            firey[n] = gy;
            fire_type[n] = type;
            fire_step[n] = 1;
            goldorak_launch_uniq = n;

            if (type == 0) {
                fire_sprite[n] = SPR_addSprite(&fire_gamma, gx, gy, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            } else if (type == 1) {
                fire_sprite[n] = SPR_addSprite(&fire001, gx, gy, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            } else if (type == 2) {
                fire_sprite[n] = SPR_addSprite(&cornofulgure, gx, gy, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            }
            return;
        }
    }
} /* fire */

void waveAndCollision()
{
    waveframe = waveframe + 1;

    nextWave = FALSE;

    if ((wavedef[wavepos].w != 0) && (waveframe == wavedef[wavepos].w )) {
        nextWave = TRUE;
    }

    if ((wavedef[wavepos].w1 != 0) && (waveframe > 20) && (displayedMonster < wavedef[wavepos].w1)) {
        nextWave = TRUE;
    }

    // char temp[32];
    // sprintf(temp, "%d", displayedMonster);
    // VDP_drawText(temp, 10, 10);

    if (nextWave) {
        if (wavedef[wavepos].e != 0) {
            level = level + 1;
            if (level == NBLEVEL + 1) level = 1;
            initLevel(level);
            return;
        }

        monster(&wavedef[wavepos]);
        wavepos = wavepos + 1;
        waveframe = 0;
    }

    displayFire();
    displayMonster();

// handle fire_type 0
    if (goldorak_launch_count_type == 0) {

        if (goldorak_launch_count == 20) {
            fire(goldo_x + 6, goldo_y + 8, goldorak_launch_count_type);
        }

        if (goldorak_launch_count > 0) {
            goldorak_launch_count = goldorak_launch_count - 1;
        }

        //    mydraw(goldorak_launch_001_00, x,y)
    }

//    -- handle fire_type 1
    if (goldorak_launch_count_type == 1) {

        if (goldorak_launch_uniq == -1) {

            if (goldorak_launch_count > 10) {
                //    mydraw(goldorak_launch_001_01, x,y)
                goldorak_launch_count = goldorak_launch_count - 1;

            } else if (goldorak_launch_count > 0) {
                //    mydraw(goldorak_launch_001_02, x,y)

                goldorak_launch_count = goldorak_launch_count - 1;
                if (goldorak_launch_count == 0) {
                    fire(goldo_x + 24, goldo_y - 4, goldorak_launch_count_type);
                }

            } else { // -- Waiting for fire
                //    mydraw(goldorak_launch_001_00, x,y);
            }
        } else {


            if (goldorak_launch_count > 0) {
                //    mydraw(goldorak_launch_001_01, x,y);

                goldorak_launch_count = goldorak_launch_count - 1;
                if (goldorak_launch_count == 0) {
                    goldorak_launch_uniq = -1; //  -- ready to launch another
                }

            } else {
                //    mydraw(goldorak_launch_001_02, x,y)
            }

        }

    } /* waveAndCollision */

//  -- handle fire_type 2
    if (goldorak_launch_count_type == 2) {

        if (goldorak_launch_count == 20) {
            fire(goldo_x + 25, goldo_y - 28, goldorak_launch_count_type);
        }

        if (goldorak_launch_count > 0) {
            goldorak_launch_count = goldorak_launch_count - 1;
        }

//     mydraw(goldorak_launch_001_00, x,y)
    }


    detectCollision();

} /* waveAndCollision */


void game_loop(void)
{
    frame++;

    update_handleGame();


    MAP_scrollToEx(levelMap, 0, offset, FALSE);

    offset--;
    if (offset == 0) {
        offset = 1537;  // loop between all the tile (Except the last 6)
    }


    // char off[32];
    // sprintf(off, "%5d", offset);
    // VDP_drawText(off, 0, 0);


    if (frame > FRAME_PER_ANIM * 20) {
        if (lostLevel == 0) {
            waveAndCollision();

        } else {
            displayFire();
            displayMonster();

            lostLevel = lostLevel + 1;
            if (lostLevel >= 100) {

                // VDP_drawText("W", 10, 10);

                if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                    XGM_startPlayPCM(SFX_SOUND_LIFE, 1, SOUND_PCM_CH2);

                    life = life - 1;
                    if (life == 0) {
                        // drawScene = draw_handleGameover
                        // updateScene = update_handleGameover
                    }
                    initLevel(level);
                    return;
                }
            }

            // love.graphics.printf("Boum",font,0,100,320,"center")
        }

        monsterFireAndCollision();
    } else {
        goldoAnim();
    }

    SPR_update();

    SYS_doVBlankProcess();

} /* game_loop */