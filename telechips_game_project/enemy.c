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


const int ENEMY_W[] = { 9, 12, 20};
const int ENEMY_H[] = { 14, 15, 15 };

int hp_mult[] = { 1, 2, 3 };   // 하 = 1, 중 = 2, 상 = 3
float speed_mult[] = { 1.0f, 1.25f, 1.5f }; // 속도 배율
float fire_mult[] = { 1.5f, 1.0f, 0.5f }; // 쿨타임 배율 (난이도 높을수록 더 자주 공격)

bool spawn_enabled = true; // 리스폰 여부
bool boss_spawned = false; // 보스 등장 여부
int boss_spawn_timer = -1; // 보스 등장 대기 타이머

// 몬스터 능력치 설정
void enemy_set_stats(ENEMY* e) {
    switch (e->type) {
    case ENEMY_TYPE_1:
        e->hp = 2 * hp_mult[game_difficulty - 1];
        e->vx = 1 * speed_mult[game_difficulty - 1];
        e->vy = 0;
        e->shot_timer = (int)(between(60, 150) * fire_mult[game_difficulty - 1]);
        break;

    case ENEMY_TYPE_2:
        e->hp = 4 * hp_mult[game_difficulty - 1];
        e->vx = 1 * speed_mult[game_difficulty - 1];
        e->vy = 2 * speed_mult[game_difficulty - 1];
        e->shot_timer = (int)(between(60, 150) * fire_mult[game_difficulty - 1]);
        break;

    case BOSS_TYPE_1:
        e->hp = 20 * hp_mult[game_difficulty - 1];
        e->vx = 1 * speed_mult[game_difficulty - 1];
        e->vy = 1 * speed_mult[game_difficulty - 1];
        e->shot_timer = (int)(60 * fire_mult[game_difficulty - 1]);
        e->state = BOSS_IDLE;
        e->state_timer = 30;
        break;
    }
}

// 몬스터 초기화
void enemies_init()
{
    for (int i = 0; i < ENEMIES_N; i++) {
        enemies[i].used = false;
    }
}


// 몬스터 업데이트 (이동, 충돌, 공격 처리)
void enemies_update()
{
    int new_quota = 5; // 이번 프레임에 새로 출현할 수 있는 몬스터 최대 수
    int start_x = BUFFER_W + 40; // 몬스터가 등장할 x좌표 시작점(화면 오른쪽 밖)
    int gap = 40; // 몬스터 간격


    for (int i = 0; i < ENEMIES_N; i++)
    {
        // 아직 사용되지 않은 슬롯이면 스폰 처리
        if (!enemies[i].used)
        {
            if (spawn_enabled && new_quota > 0)
            {
                // x좌표 배치 : 출현 quota 개수에 따라 오른쪽에서 간격을 두고 배치
                enemies[i].x = start_x - (new_quota - 1) * gap;

                // y좌표 배치 : 110 이상, 화면 높이의 75% 지점 이하에서 랜덤
                enemies[i].y = between(120, 0.75 * (BUFFER_H - ENEMY_H[0]));

                // 타입은 일반 몬스터 중에서만 선택
                enemies[i].type = between(ENEMY_TYPE_1, ENEMY_TYPE_2 + 1);
                enemies[i].blink = 0;
                enemies[i].used = true;

                // 초기 능력치 설정
                enemy_set_stats(&enemies[i]);

                new_quota--;
            }
            continue;
        }

        // --------------------
        // 타입별 이동 처리
        // --------------------
        switch (enemies[i].type)
        {
        case ENEMY_TYPE_1: // 일반 몬스터 1
            if (frames % 2) {
                enemies[i].x -= enemies[i].vx; // 왼쪽으로 이동

                // 화면 왼쪽/오른쪽 경계에서 방향 반전
                if (enemies[i].x < 0 || enemies[i].x > start_x)
                    enemies[i].vx *= -1;
            }
            break;

        case ENEMY_TYPE_2: // 일반 몬스터 2
            if (frames % 2) {
                enemies[i].x -= enemies[i].vx;
                enemies[i].y += enemies[i].vy;

                // y축 경계에서 반전
                if (enemies[i].y < 120 || enemies[i].y > 0.75 * (BUFFER_H - ENEMY_H[1]))
                    enemies[i].vy *= -1;

                // x축 경계에서 반전
                if (enemies[i].x < 0 || enemies[i].x > start_x)
                    enemies[i].vx *= -1;
            }
            break;

        case BOSS_TYPE_1: // 보스 몬스터
            if (frames % 2) // 속도 보정
            {
                // 상태 타이머 감소 → 일정 시간마다 상태 전환
                if (--enemies[i].state_timer <= 0) {
                    if (enemies[i].state == BOSS_IDLE) {
                        enemies[i].state = BOSS_CHASE;
                        enemies[i].state_timer = 30; // 추적
                    }
                    else {
                        enemies[i].state = BOSS_IDLE;
                        enemies[i].state_timer = 30; // 멈춤
                    }
                }

                // 추적 상태일 때만 플레이어 따라감
                if (enemies[i].state == BOSS_CHASE) {
                    if (player.x > enemies[i].x) enemies[i].x += enemies[i].vx;
                    else if (player.x < enemies[i].x) enemies[i].x -= enemies[i].vx;

                    if (player.y > enemies[i].y) enemies[i].y += enemies[i].vy;
                    else if (player.y < enemies[i].y) enemies[i].y -= enemies[i].vy;
                }

                // 경계 체크 (화면 밖으로 못 나가게 제한)
                int boss_w = ENEMY_W[BOSS_TYPE_1];
                int boss_h = ENEMY_H[BOSS_TYPE_1];

                if (enemies[i].x < 0) enemies[i].x = 0;
                if (enemies[i].x > BUFFER_W - boss_w) enemies[i].x = BUFFER_W - boss_w;
                if (enemies[i].y < 110) enemies[i].y = 110;
                if (enemies[i].y > BUFFER_H - boss_h) enemies[i].y = BUFFER_H - boss_h;
            }
            break;
        }

        // 맞았을 때 깜빡임 효과
        if (enemies[i].blink) enemies[i].blink--;

        // --------------------
        // 충돌 처리
        // --------------------
        int w = ENEMY_W[enemies[i].type];
        int h = ENEMY_H[enemies[i].type];

        // 깊이 스케일 계산 (2.5D 효과)
        float t = (float)(enemies[i].y - 110) / (PLAYER_MAX_Y - 110);
        if (t < 0) t = 0; if (t > 1) t = 1;
        float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        int scaled_w = w * scale;
        int scaled_h = h * scale;

        // 플레이어 총알 <-> 몬스터 충돌 체크
        int damage = shots_collide(false, enemies[i].x, enemies[i].y, scaled_w, scaled_h);
        if (damage > 0) {
            enemies[i].hp -= damage;
            enemies[i].blink = 4;
        }


        // 체력 0 이하 → 제거
        if (enemies[i].hp <= 0) {
            enemies[i].used = false;

            continue;
        }

        // --------------------
        // 공격 처리
        // --------------------
        if (enemies[i].shot_timer > 0)
            enemies[i].shot_timer--;

        if (enemies[i].shot_timer <= 0) {
            int cx = enemies[i].x + ENEMY_W[enemies[i].type] / 2;
            int cy = enemies[i].y + ENEMY_H[enemies[i].type] / 2;

            if (enemies[i].type == BOSS_TYPE_1) {
                // 보스: 상하좌우 4방향 발사
                shots_add(false, true, cx, cy, DIR_LEFT, 10, ATTACK_BOSS);
                shots_add(false, true, cx, cy, DIR_RIGHT, 10, ATTACK_BOSS);
                shots_add(false, true, cx, cy, DIR_UP, 10, ATTACK_BOSS);
                shots_add(false, true, cx, cy, DIR_DOWN, 10, ATTACK_BOSS);

                enemies[i].shot_timer = (int)(60 * fire_mult[game_difficulty - 1]);
            }
            else {
                // 일반 몬스터: 이동 방향 기준 발사
                DIRECTION fire_dir;
                if (enemies[i].vx > 0) fire_dir = DIR_LEFT;
                else if (enemies[i].vx < 0) fire_dir = DIR_RIGHT;
                else fire_dir = DIR_LEFT;

                shots_add(false, true, cx, cy, fire_dir, 5, ATTACK_ENEMY);

                switch (enemies[i].type) {
                case ENEMY_TYPE_1: enemies[i].shot_timer = (int)(between(60, 150) * fire_mult[game_difficulty - 1]); break;
                case ENEMY_TYPE_2: enemies[i].shot_timer = (int)(between(60, 150) * fire_mult[game_difficulty - 1]); break;
                }
            }
        }
    }

    // --------------------
    // 보스 소환 체크
    // --------------------
    bool any_normal_enemy_alive = false;
    for (int i = 0; i < ENEMIES_N; i++) {
        if (enemies[i].used && enemies[i].type != BOSS_TYPE_1) {
            any_normal_enemy_alive = true;
            break;
        }
    }

    // 모든 일반 몬스터가 사라졌을 때 보스 등장
    if (!boss_spawned && !any_normal_enemy_alive) {
        if (boss_spawn_timer == -1) {
            boss_spawn_timer = 60; // 1초 대기
        }
        else if (--boss_spawn_timer <= 0) {
            for (int i = 0; i < ENEMIES_N; i++) {
                if (!enemies[i].used) {
                    enemies[i].x = BUFFER_W / 2 - ENEMY_W[BOSS_TYPE_1] / 2;
                    enemies[i].y = 120; // 화면 위쪽 중앙
                    enemies[i].type = BOSS_TYPE_1;
                    enemies[i].used = true;

                    //　능력치 초기화
                    enemy_set_stats(&enemies[i]);

                    boss_spawned = true;
                    break;
                }
            }
        }
    }
}

// 몬스터 그리기
void enemies_draw()
{
    for (int i = 0; i < ENEMIES_N; i++) {
        if (!enemies[i].used) continue;

        // 적 HP 표시
        char hp_text[16];
        sprintf(hp_text, "%d", enemies[i].hp);
        al_draw_text(
            font,
            al_map_rgb(255, 255, 255),
            enemies[i].x + PLAYER_W / 2,
            enemies[i].y - 10,
            ALLEGRO_ALIGN_CENTER,
            hp_text
        );

        if (enemies[i].blink > 2) continue;

        // 깊이 스케일 계산 (2.5D 효과)
        float t = (float)(enemies[i].y - 110) / (PLAYER_MAX_Y - 110);
        if (t < 0) t = 0; if (t > 1) t = 1;
        float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        int w = ENEMY_W[enemies[i].type];
        int h = ENEMY_H[enemies[i].type];

        // 보스는 빨간 사각형으로
        if (enemies[i].type == BOSS_TYPE_1) {
            al_draw_filled_rectangle(
                enemies[i].x,
                enemies[i].y,
                enemies[i].x + w * scale,
                enemies[i].y + h * scale,
                al_map_rgb(255, 0, 0)
            );
        }
        else {
            // 일반 몬스터는 스프라이트
            al_draw_scaled_bitmap(
                sprites.enemy[enemies[i].type],
                0, 0, w, h,
                enemies[i].x, enemies[i].y,
                w * scale, h * scale,
                0
            );
        }
    }
}