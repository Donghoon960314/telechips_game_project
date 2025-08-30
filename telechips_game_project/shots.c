#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

const int PLAYER_SHOT_W[] = { 40, 70 };
const int PLAYER_SHOT_H[] = { 50, 50 };

int PLAYER_SHOT_WIDTH;
int PLAYER_SHOT_HEIGHT;

const int ENEMY_SHOT_W[] = { 15, 50 };
const int ENEMY_SHOT_H[] = { 20, 15 };

int ENEMY_SHOT_WIDTH;
int ENEMY_SHOT_HEIGHT;
int sw, sh;


void shots_init() {
    for (int i = 0; i < SHOTS_N; i++)
        shots[i].used = false;
}

bool shots_add(bool player, bool straight, int x, int y, DIRECTION dir, int power, ATTACK_TYPE attack_type)
{
    // 강한 공격(플레이어)
    if (player && power >= 4)
    {   
        PLAYER_SHOT_WIDTH = PLAYER_SHOT_2_W;
        PLAYER_SHOT_HEIGHT = PLAYER_SHOT_2_H;
        

        al_play_sample(
            sample_strong_shot,
            0.8,
            0,
            0.6,
            ALLEGRO_PLAYMODE_ONCE,
            NULL
        );
    }
    else if (power >= 8)
    {   
        //적 보스 공격
        ENEMY_SHOT_WIDTH = ENEMY_SHOT_2_W;
        ENEMY_SHOT_HEIGHT = ENEMY_SHOT_2_H;

        al_play_sample(
            sample_strong_shot,
            0.8,
            0,
            0.6,
            ALLEGRO_PLAYMODE_ONCE,
            NULL
        );
    }
    else
    {
        // 일반 공격(플레이어/적)
        PLAYER_SHOT_WIDTH = PLAYER_SHOT_1_W;
        PLAYER_SHOT_HEIGHT = PLAYER_SHOT_1_H;
        
        al_play_sample(
            sample_normal_shot,
            0.3,
            0,
            player ? 1.0 : between_f(1.5, 1.6),
            ALLEGRO_PLAYMODE_ONCE,
            NULL
        );
    }

    for (int i = 0; i < SHOTS_N; i++)
    {
        // 빈 슬롯 찾기
        if (shots[i].used) continue;

        // 누가 쏜 총알인지 저장
        shots[i].player = player;
        shots[i].attack_type = attack_type; // 공격 타입 저장
        shots[i].power = power;

        // 플레이어 총알이면 위치 설정
        if (player)
        {
            shots[i].x = x + (PLAYER_W / 2) - (PLAYER_SHOT_WIDTH / 2);
            shots[i].y = y + (PLAYER_H / 2) + (PLAYER_SHOT_HEIGHT / 2);

            // 플레이어 방향 저장
            switch (dir)
            {
            //case DIR_UP:    shots[i].dir = SHOT_UP; break;
            case DIR_DOWN:  shots[i].dir = SHOT_DOWN; break;
            //case DIR_LEFT:  shots[i].dir = SHOT_LEFT; break;
            case DIR_RIGHT: shots[i].dir = SHOT_RIGHT; break;
            }
        }
        // 적 총알이면
        else
        {
            if (shots[i].power > 5) {
                int boss_center_x = x + BOSS_1_W / 2;
                int boss_center_y = y + BOSS_1_H / 2;
                ENEMY_SHOT_WIDTH = ENEMY_SHOT_2_W;
                ENEMY_SHOT_HEIGHT = ENEMY_2_H;
                shots[i].x = boss_center_x - (ENEMY_SHOT_WIDTH / 2);
                shots[i].y = boss_center_y - (ENEMY_SHOT_HEIGHT / 2);
            }
            else {
                ENEMY_SHOT_WIDTH = ENEMY_SHOT_1_W;
                ENEMY_SHOT_HEIGHT = ENEMY_1_H;
                shots[i].x = x - (ENEMY_SHOT_WIDTH / 2);
                shots[i].y = y - (ENEMY_SHOT_HEIGHT / 2);
            }
            // 직선 이동
            if (straight)
            {
                // dir 기준으로 좌/우 직선 사격
                int spd = 2;
                switch (dir)
                {
                case DIR_LEFT:
                    shots[i].dx = -spd;
                    shots[i].dy = 0;
                    shots[i].dir = (SHOT_DIR)SHOT_LEFT;
                    break;
                case DIR_RIGHT:
                    shots[i].dx = spd;
                    shots[i].dy = 0;
                    shots[i].dir = (SHOT_DIR)SHOT_RIGHT;
                    break;
                    /*
                case DIR_UP:
                    shots[i].dx = 0;
                    shots[i].dy = -spd;
                    shots[i].dir = (SHOT_DIR)SHOT_UP;
                    break;
                case DIR_DOWN:
                    shots[i].dx = 0;
                    shots[i].dy = spd;
                    shots[i].dir = (SHOT_DIR)SHOT_DOWN;
                    break;
                    */
                }
            }
            // 랜덤 이동
            /*else
            {
                shots[i].dx = between(-2, 2);
                shots[i].dy = between(-2, 2);
            }*/

            // 속도가 없으면 바로 종료
            if (!shots[i].dx && !shots[i].dy)
                return true;

            shots[i].frame = 0;
        }
        shots[i].frame = 0;
        shots[i].used = true;
        shots[i].power = power;

        return true;
    }
    return false;
}

void shots_update() {
    for (int i = 0; i < SHOTS_N; i++) {
        // 사용 안 하는 슬롯 건너뛰기
        if (!shots[i].used) continue;

        // 플레이어 총알 처리
        if (shots[i].player) {
            // 방향에 따라 이동
            switch (shots[i].dir) {
            //case SHOT_UP:    shots[i].y -= 5; break;
            //case SHOT_DOWN:  shots[i].y += 5; break;
            case SHOT_LEFT:  shots[i].x -= 5; break;
            case SHOT_RIGHT: shots[i].x += 5; break;
            }

            // 화면 밖이면 제거
            if (shots[i].x < -PLAYER_SHOT_WIDTH || shots[i].x > BUFFER_W ||
                shots[i].y < -PLAYER_SHOT_HEIGHT || shots[i].y > BUFFER_H) {
                shots[i].used = false;
            }
        }
        // 적 총알 처리
        else
        {
            shots[i].x += shots[i].dx;
            shots[i].y += shots[i].dy;

            /*if (shots[i].dir == SHOT_UP) {
                if (shots[i].y < 0 || shots[i].y <= 110) {
                    shots[i].used = false;
                    continue;
                }
            }*/

            //else {
                if ((shots[i].x < -ENEMY_SHOT_WIDTH)
                    || (shots[i].x > BUFFER_W)
                    || (shots[i].y < -ENEMY_SHOT_HEIGHT)
                    || (shots[i].y > BUFFER_H)
                    ) {
                    shots[i].used = false;
                    continue;
                }
            //}
        }

        shots[i].frame++;
    }
}

int shots_collide(bool is_player, int x, int y, int w, int h) {
    for (int i = 0; i < SHOTS_N; i++) {
        // 총알 사용 여부 확인
        if (!shots[i].used) continue;

        // 자기 자신의 총알은 제외
        if (shots[i].player == is_player) continue;

        // 총알 크기 설정
        int sw, sh;
        if(shots[i].player)
        {
            // 플레이어 총알 크기 = 스프라이트 크기 × 깊이 스케일
            ALLEGRO_BITMAP* bmp = NULL;
            JOB_TYPE job = player.job;
            if (shots[i].attack_type == ATTACK_NORMAL) {
                bmp = sprites.player_shot[job][0];
            }
            else if (shots[i].attack_type == ATTACK_SKILL_1) {
                bmp = sprites.player_shot[job][1];
            }

            if (bmp) {
                // 깊이 스케일 계산
                float t = (float)(shots[i].y - 110) / (PLAYER_MAX_Y - 110);
                if (t < 0) t = 0; if (t > 1) t = 1;
                float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

                sw = (int)(al_get_bitmap_width(bmp) * scale);
                sh = (int)(al_get_bitmap_height(bmp) * scale);
            }
            else {
                sw = PLAYER_SHOT_WIDTH;
                sh = PLAYER_SHOT_HEIGHT;
            }
        }
        else {
            // 적 총알 크기 = 스프라이트 크기 × 깊이 스케일
            ALLEGRO_BITMAP* bmp = NULL;
            if (shots[i].attack_type == ATTACK_BOSS) {
                bmp = sprites.enemy_shot[1];
            }
            else {
                bmp = sprites.enemy_shot[0];
            }

            if (bmp) {
                float t = (float)(shots[i].y - 110) / (PLAYER_MAX_Y - 110);
                if (t < 0) t = 0; if (t > 1) t = 1;
                float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

                sw = (int)(al_get_bitmap_width(bmp) * scale);
                sh = (int)(al_get_bitmap_height(bmp) * scale);
            }
            else {
                sw = ENEMY_SHOT_WIDTH;
                sh = ENEMY_SHOT_HEIGHT;
            }
        }

        int hitbox_h = sh / 1.5;                         
        int hitbox_y = shots[i].y + sh - hitbox_h; 

        // 충돌 체크
        if (collide(x, y, x + w, y + h,
            shots[i].x, hitbox_y,
            shots[i].x + sw, hitbox_y + hitbox_h))
        {
            int damage = shots[i].power;
            shots[i].used = false;
            return damage;
        }
    }
    return 0;
}

void shots_draw() {
    for (int i = 0; i < SHOTS_N; i++) {
        // 사용 중인 총알만 그리기
        if (!shots[i].used) continue;

        int frame_display = (shots[i].frame / 2) % 2;

        // 플레이어 총알
        if (shots[i].player) {
            float t = (float)(shots[i].y - 110) / (PLAYER_MAX_Y - 110);
            if (t < 0) t = 0;
            if (t > 1) t = 1;
            float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);
            
            ALLEGRO_BITMAP* bmp = NULL;
            JOB_TYPE job = player.job;  // 현재 플레이어 직업
            if (shots[i].attack_type == ATTACK_NORMAL) 
            {
                bmp = sprites.player_shot[job][0]; // 직업별 일반공격
            }
            else if (shots[i].attack_type == ATTACK_SKILL_1)
            {
                bmp = sprites.player_shot[job][1]; // 직업별 스킬1
            }

            float scale_x = scale;
            float scale_y = scale;
            float angle = 0.0f;

            //히트박스 그리기용
            if (bmp) {
                float t = (float)(shots[i].y - 110) / (PLAYER_MAX_Y - 110);
                if (t < 0) t = 0; if (t > 1) t = 1;
                float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

                sw = (int)(al_get_bitmap_width(bmp) * scale);
                sh = (int)(al_get_bitmap_height(bmp) * scale);
            }
            else {
                sw = PLAYER_SHOT_WIDTH;
                sh = PLAYER_SHOT_HEIGHT;
            }
            //여기까지
            switch (shots[i].dir)
            {
            //case DIR_UP:    angle = 0.0f;               break;
            //case DIR_DOWN:  angle = ALLEGRO_PI;         break;
            case DIR_LEFT:  scale_x = -scale; angle = 0.0f;    break;
            //case DIR_RIGHT: angle = ALLEGRO_PI * 0.5f;  break;
            }

            float cx = PLAYER_SHOT_WIDTH / 2.0f; //al_get_bitmap_width(bmp) / 2.0f;
            float cy = PLAYER_SHOT_HEIGHT / 2.0f; //al_get_bitmap_height(bmp) / 2.0f;

            al_draw_scaled_rotated_bitmap(
                bmp,
                cx, cy, // 회전 기준 (중심)
                shots[i].x + cx,
                shots[i].y + cy,
                scale_x, scale_y,
                angle,
                0
            );
        }
        // 적 총알
        else
        {
            ALLEGRO_BITMAP* bmp = sprites.enemy_shot;

            // 적 타입마다 총알 구분
            if (shots[i].power >= 10) {
                bmp = sprites.enemy_shot[1]; // 보스 총알
            }
            else {
                bmp = sprites.enemy_shot[0]; // 일반 몹 총알
            }
            //히트박스 그리기용

            if (bmp) {
                float t = (float)(shots[i].y - 110) / (PLAYER_MAX_Y - 110);
                if (t < 0) t = 0; if (t > 1) t = 1;
                float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

                sw = (int)(al_get_bitmap_width(bmp) * scale);
                sh = (int)(al_get_bitmap_height(bmp) * scale);
            }
            else {
                sw = ENEMY_SHOT_WIDTH;
                sh = ENEMY_SHOT_HEIGHT;
            }
            // 깜빡임용 틴트
            //ALLEGRO_COLOR tint = frame_display
                //? al_map_rgb_f(1, 1, 1)
                //: al_map_rgb_f(0.5f, 0.5f, 0.5f);

            // 회전 각도
            float angle = 0.0f;
            switch (shots[i].dir) {
            //case DIR_UP:    angle = 0.0f;               break;
            //case DIR_DOWN:  angle = ALLEGRO_PI;        break;
            //case DIR_LEFT:  angle = -ALLEGRO_PI * 0.5f; break;
            case DIR_RIGHT: angle = ALLEGRO_PI * 0.5f; break;
            }

            // 깊이 스케일
            float t = (float)(shots[i].y - 110) / (PLAYER_MAX_Y - 110);
            if (t < 0) t = 0; else if (t > 1) t = 1;
            float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

            // 중심 좌표
            float cx = (float)al_get_bitmap_width(bmp);
            float cy = al_get_bitmap_height(bmp) / 2.0f;

            // 틴트 + 스케일 + 회전 그리기
            al_draw_tinted_scaled_rotated_bitmap(
                bmp,
                //tint,
                al_map_rgb_f(1, 1, 1),
                cx, cy, // 회전 기준(중심)
                shots[i].x, //+ cx, // 화면 X
                shots[i].y, //+ cy, // 화면 Y
                scale, scale, // 스케일
                angle, // 회전
                0 // 플래그
            );

        }
        int hitbox_h = sh / 1.5;
        int hitbox_y = shots[i].y + sh - hitbox_h;
        al_draw_rectangle(
            shots[i].x, hitbox_y,
            shots[i].x + sw, hitbox_y + hitbox_h,
            al_map_rgb(255, 0, 0), 1
        );
    }
}
