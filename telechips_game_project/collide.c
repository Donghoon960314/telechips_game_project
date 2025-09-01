//======================================================
//                    collide.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      collide.c
 @brief     충돌 판정 정의
 @author    김혁, 신동훈, 정명훈, 이재강
*/

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

// 플레이어 <-> 적 충돌 판정
int enemies_collide(bool player, int x, int y, int w, int h)
{
    for (int i = 0; i < ENEMIES_N; i++) {
        if (!enemies[i].used) continue;

        int ew = ENEMY_W[enemies[i].type];
        int eh = ENEMY_H[enemies[i].type];

        // 스케일 계산
        float t = (float)(enemies[i].y - 110) / (PLAYER_MAX_Y - 110);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        // 스케일 적용
        int scaled_w = (int)(ew * scale);
        int scaled_h = (int)(eh * scale);

        if (collide(x, y, x + w, y + h,
            enemies[i].x, enemies[i].y,
            enemies[i].x + scaled_w, enemies[i].y + scaled_h)) {
            return i;
        }
    }
    return -1;
}