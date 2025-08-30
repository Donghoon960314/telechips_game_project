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

void player_init()
{
    player.x = (BUFFER_W / 2) - (PLAYER_W / 2);
    player.y = (BUFFER_H / 2) - (PLAYER_H / 2);
    player.normal_shot_timer = 0;
    player.strong_shot_timer = 0;

    //player.hp = 60;

    player.hp = 40;
    player.invincible_timer = 120; // 무적 시간
    player.last_dir = DIR_RIGHT; // 초기 총알 방향
    player.attack_anim_timer = 0; // 공격 모션 타이머
}

void player_update()
{
    // HP가 0보다 작으면 아무 것도 하지 않음
    if (player.hp <= 0)
        return;

    // 키 입력에 따라 이동 처리 및 플레이어 이동 방향 저장
    if (key[ALLEGRO_KEY_LEFT]) {
        player.x -= PLAYER_SPEED;
        player.last_dir = DIR_LEFT;
    }
    if (key[ALLEGRO_KEY_RIGHT]) {
        player.x += PLAYER_SPEED;
        player.last_dir = DIR_RIGHT;
    }
    if (key[ALLEGRO_KEY_UP]) {
        player.y -= PLAYER_SPEED;
    }
    if (key[ALLEGRO_KEY_DOWN]) {
        player.y += PLAYER_SPEED;
    }

    // 화면 밖으로 나가지 않도록 제한
    if (player.x < 0)
        player.x = 0;
    if (player.x > PLAYER_MAX_X)
        player.x = PLAYER_MAX_X;

    if (player.y < PLAYER_MIN_Y)
        player.y = PLAYER_MIN_Y;
    if (player.y > PLAYER_MAX_Y)
        player.y = PLAYER_MAX_Y;

    // 깊이 스케일 계산
    float t = (float)(player.y - PLAYER_MIN_Y) / (PLAYER_MAX_Y - PLAYER_MIN_Y);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

    // 실제 충돌 크기
    int scaled_w = PLAYER_W * scale;
    int scaled_h = PLAYER_H * scale;

    // 무적 상태 처리
    if (player.invincible_timer > 0) {
        player.invincible_timer--;
    }
    // 무적 상태가 아닌 경우
    else {
        // 플레이어 <-> 몹 충돌
        int enemy_index = enemies_collide(true, player.x, player.y, scaled_w, scaled_h);
        if (enemy_index != -1) {
            if (enemies[enemy_index].type == BOSS_TYPE_1)
                player.hp -= 20; // 보스 충돌 → 20 깎음
            else
                player.hp -= 10; // 일반 몬스터 충돌 → 10 깎음

            if (player.hp < 0) player.hp = 0;
            player.invincible_timer = 120; // 2초 무적
        }
        // 플레이어 <-> 몹 총알 충돌
        int damage = shots_collide(true, player.x, player.y, scaled_w, scaled_h);
        if (damage > 0)
        {
            player.hp -= damage; // HP 감소
            if (player.hp < 0) player.hp = 0;
            player.invincible_timer = 120; // 2초 무적
        }
    }

    // 총알 발사 처리
    // 일반 공격 (X키)
    if (player.normal_shot_timer > 0) {
        player.normal_shot_timer--;
    }
    else if (key[ALLEGRO_KEY_X]) {
        if (shots_add(true, true, player.x, player.y, player.last_dir, 1)) {
            player.normal_shot_timer = 5; // 일반 공격 쿨타임
            player.attack_anim_timer = 18; // 공격 모션 유지 시간
        }
    }

    // 강한 공격 (Z키)
    if (player.strong_shot_timer > 0) {
        player.strong_shot_timer--;
    }
    else if (key[ALLEGRO_KEY_Z]) {
        if (shots_add(true, true, player.x, player.y, player.last_dir, 4)) {
            player.strong_shot_timer = 5; // 강공격 쿨타임
            player.attack_anim_timer = 18; // 공격 모션 유지 시간
        }
    }

    if (player.attack_anim_timer > 0) {
        player.attack_anim_timer--;
    }
}

void player_draw()
{
    if (player.hp <= 0)
        return;

    // 플레이어 HP 표시
    char hp_text[16];
    sprintf(hp_text, "%d", player.hp);
    al_draw_text(
        font,
        al_map_rgb(255, 255, 255),
        player.x + PLAYER_W / 2, // 플레이어 중앙
        player.y - 10,           // 위쪽
        ALLEGRO_ALIGN_CENTER,
        hp_text
    );

    // 무적 깜빡임 효과 처리
    if (player.invincible_timer > 0)
    {
        if (((player.invincible_timer / 2) % 3) == 1)
            return; // 이 프레임에는 그리지 않음
    }

    // 2.5D 구현
    DEPTH_MIN_SCALE = 1.0f; 
    DEPTH_MAX_SCALE = 2.5f;  

    // y = 110일 때 min_scale, y = PLAYER_MAX_Y일 때 max_scale
    float t = (float)(player.y - 110) / (PLAYER_MAX_Y - 110);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    float depth_scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);
    
    // 평상시/공격 이미지 변경
    ALLEGRO_BITMAP* bmp;
    if (player.attack_anim_timer > 0) 
    {
        bmp = sprites.player_attack;
    }
    else
    {
        bmp = sprites.player;
    }

    int w = PLAYER_W;
    int h = PLAYER_H;

    // 깊이감 스케일 적용
    float final_scale_x = w * depth_scale;
    float final_scale_y = h * depth_scale;

    // 좌/우 방향 반전
    if (player.last_dir == DIR_LEFT) {
        al_draw_scaled_bitmap(
            bmp,
            0, 0, w, h,
            player.x + final_scale_x, player.y,
            -final_scale_x, final_scale_y,
            0
        );
    }
    else {
        al_draw_scaled_bitmap(
            bmp,
            0, 0, w, h,
            player.x,
            player.y,
            final_scale_x, final_scale_y,
            0
        );
    }
}