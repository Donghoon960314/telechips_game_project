#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

void items_init()
{
    for (int i = 0; i < ITEMS_N; i++)
        items[i].used = false;

    item_spawn_timer = 180; // 아이템 스폰 시간 설정
}

void items_update()
{
    if (--item_spawn_timer <= 0)
    {
        for (int i = 0; i < ITEMS_N; i++)
        {
            if (!items[i].used)
            {
                items[i].x = between(100, BUFFER_W - 100);
                items[i].y = between(PLAYER_MIN_Y + 100, PLAYER_MAX_Y - 100);

                items[i].type = (rand() % 2 == 0) ? ITEM_HEAL : ITEM_ATK_SPEED;
                items[i].used = true;
                items[i].lifetime = 300;
                break;
            }
        }

        item_spawn_timer = 180;
    }
    // 플레이어 <-> 아이템 충돌 판정
    for (int i = 0; i < ITEMS_N; i++)
    {
        if (!items[i].used) continue;

        if (--items[i].lifetime <= 0) {
            items[i].used = false;
            continue;
        }

        // =====================================
        //        플레이어 히트박스 계산
        // =====================================
        float t = (float)(player.y - PLAYER_MIN_Y) / (PLAYER_MAX_Y - PLAYER_MIN_Y);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        int scaled_w = PLAYER_W * scale;
        int scaled_h = PLAYER_H * scale;

        int hitbox_h = scaled_h / 3;
        int hitbox_y = player.y + scaled_h - hitbox_h;

        int player_hitbox_x1 = player.x;
        int player_hitbox_y1 = hitbox_y;
        int player_hitbox_x2 = player.x + scaled_w;
        int player_hitbox_y2 = hitbox_y + hitbox_h;

        // =====================================
        //         아이템 히트박스 계산
        // =====================================
        ALLEGRO_BITMAP* bmp = (items[i].type == ITEM_HEAL) ? hp_heal : attack_speed_up;
        int w = al_get_bitmap_width(bmp);
        int h = al_get_bitmap_height(bmp);

        // 플레이어와 같은 깊이 스케일 계산
        float t_item = (float)(items[i].y - PLAYER_MIN_Y) / (PLAYER_MAX_Y - PLAYER_MIN_Y);
        if (t_item < 0) t_item = 0;
        if (t_item > 1) t_item = 1;
        float item_scale = DEPTH_MIN_SCALE + t_item * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        // 스케일 적용된 실제 충돌 박스
        int item_x1 = items[i].x - (w * item_scale) / 2;
        int item_y1 = items[i].y - (h * item_scale) / 2;
        int item_x2 = items[i].x + (w * item_scale) / 2;
        int item_y2 = items[i].y + (h * item_scale) / 2;

        if (collide(
            player_hitbox_x1, player_hitbox_y1,
            player_hitbox_x2, player_hitbox_y2,
            item_x1, item_y1,
            item_x2, item_y2))
        {
            if (items[i].type == ITEM_HEAL) // 체력 회복 아이템
            {
                player.hp += 20;
                if (player.hp > player.max_hp)
                    player.hp = player.max_hp;

                for (int j = 0; j < HEAL_TEXTS_N; j++) {
                    if (!heal_texts[j].active) {
                        heal_texts[j].active = true;
                        heal_texts[j].x = player.x + PLAYER_W / 2;
                        heal_texts[j].y = player.y - 30;
                        heal_texts[j].timer = 60; // 1초 동안 표시 (60fps 기준)
                        sprintf(heal_texts[j].text, "+20");
                        break;
                    }
                }
            }

            else if (items[i].type == ITEM_ATK_SPEED) // 공격 속도 증가 아이템
            {
                player.atk_speed_buff = true;
                player.atk_speed_buff_timer = 300; // 버프 유지 시간

                player.normal_shot_cooldown /= 3;
                player.skill_1_cooldown /= 3;

                if (player.normal_shot_cooldown < 5)
                    player.normal_shot_cooldown = 5;
                if (player.skill_1_cooldown < 15)
                    player.skill_1_cooldown = 15;
            }

            items[i].used = false;
        }
    }

    // 공격 속도 복구
    if (player.atk_speed_buff)
    {
        if (--player.atk_speed_buff_timer <= 0)
        {
            player.atk_speed_buff = false;

            if (player.job == JOB_TYPE_1) {
                player.normal_shot_cooldown = 60;
                player.skill_1_cooldown = 120;
            }
            else if (player.job == JOB_TYPE_2) {
                player.normal_shot_cooldown = 15;
                player.skill_1_cooldown = 60;
            }
        }
    }
}

void items_draw()
{
    for (int i = 0; i < ITEMS_N; i++)
    {
        if (!items[i].used) continue;

        // 마지막 2초 동안 깜빡임
        if (items[i].lifetime <= 120) {
            if ((items[i].lifetime / 10) % 2 == 0) {
                continue;
            }
        }

        // 깊이에 따른 스케일 계산
        float t = (float)(items[i].y - PLAYER_MIN_Y) / (PLAYER_MAX_Y - PLAYER_MIN_Y);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        // 아이템 비트맵과 원본 크기
        ALLEGRO_BITMAP* bmp = (items[i].type == ITEM_HEAL) ? hp_heal : attack_speed_up;
        int w = al_get_bitmap_width(bmp);
        int h = al_get_bitmap_height(bmp);

        // 스케일 적용해서 중앙 기준으로 그리기
        al_draw_scaled_bitmap(
            bmp,
            0, 0, w, h,                                // 원본 전체
            items[i].x - (w * scale) / 2,              // 화면 X (중앙 정렬)
            items[i].y - (h * scale) / 2,              // 화면 Y (중앙 정렬)
            w * scale, h * scale,                      // 스케일 적용 크기
            0
        );
    }
}

