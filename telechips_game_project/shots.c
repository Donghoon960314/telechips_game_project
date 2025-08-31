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
    // ���� ����(�÷��̾�)
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
        //�� ���� ����
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
        // �Ϲ� ����(�÷��̾�/��)
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
        // �� ���� ã��
        if (shots[i].used) continue;

        // ���� �� �Ѿ����� ����
        shots[i].player = player;
        shots[i].attack_type = attack_type; // ���� Ÿ�� ����
        shots[i].power = power;

        // �÷��̾� �Ѿ��̸� ��ġ ����
        if (player)
        {
            shots[i].x = x + (PLAYER_W / 2) - (PLAYER_SHOT_WIDTH / 2);
            shots[i].y = y + (PLAYER_H / 2) + (PLAYER_SHOT_HEIGHT / 2);

            // �÷��̾� ���� ����
            switch (dir)
            {
            case DIR_DOWN:  shots[i].dir = SHOT_DOWN; break;
            case DIR_RIGHT: shots[i].dir = SHOT_RIGHT; break;
            }
        }
        // �� �Ѿ��̸�
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
            // ���� �̵�
            if (straight)
            {
                // dir �������� ��/�� ���� ���
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
                }
            }
            // �ӵ��� ������ �ٷ� ����
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
        // ��� �� �ϴ� ���� �ǳʶٱ�
        if (!shots[i].used) continue;

        // �÷��̾� �Ѿ� ó��
        if (shots[i].player) {            
            if (shots[i].dir == SHOT_LEFT)  shots[i].x -= 5;
            else                            shots[i].x += 5;
            // ȭ�� ���̸� ����
            if (shots[i].x < -PLAYER_SHOT_WIDTH || shots[i].x > BUFFER_W ||
                shots[i].y < -PLAYER_SHOT_HEIGHT || shots[i].y > BUFFER_H) {
                shots[i].used = false;
            }
        }
        // �� �Ѿ� ó��
        else
        {
            shots[i].x += shots[i].dx;
            
            
            if ((shots[i].x < -ENEMY_SHOT_WIDTH)
                || (shots[i].x > BUFFER_W)       
            ) {
                shots[i].used = false;
                continue;
            }
        }
        shots[i].frame++;
    }
}

int shots_collide(bool is_player, int x, int y, int w, int h) {
    for (int i = 0; i < SHOTS_N; i++) {
        // �Ѿ� ��� ���� Ȯ��
        if (!shots[i].used) continue;

        // �ڱ� �ڽ��� �Ѿ��� ����
        if (shots[i].player == is_player) continue;

        // �Ѿ� ũ�� ����
        int sw, sh;
        if(shots[i].player)
        {
            // �÷��̾� �Ѿ� ũ�� = ��������Ʈ ũ�� �� ���� ������
            ALLEGRO_BITMAP* bmp = NULL;
            JOB_TYPE job = player.job;
            if (shots[i].attack_type == ATTACK_NORMAL) {
                bmp = sprites.player_shot[job][0];
            }
            else if (shots[i].attack_type == ATTACK_SKILL_1) {
                bmp = sprites.player_shot[job][1];
            }

            if (bmp) {
                // ���� ������ ���
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
            // �� �Ѿ� ũ�� = ��������Ʈ ũ�� �� ���� ������
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
        int hitbox_x = shots[i].x;
        int hitbox_h = sh * 0.8f;                         
        int hitbox_y = shots[i].y + sh - hitbox_h; 

        // �浹 üũ
        if (collide(x, y, x + w, y + h,
            hitbox_x, hitbox_y,
            hitbox_x + sw, hitbox_y + hitbox_h))
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
        // ��� ���� �Ѿ˸� �׸���
        if (!shots[i].used) continue;

        int frame_display = (shots[i].frame / 2) % 2;

        // �÷��̾� �Ѿ�
        if (shots[i].player) {
            ALLEGRO_BITMAP* bmp = (shots[i].attack_type == ATTACK_NORMAL)
                ? sprites.player_shot[player.job][0]
                : sprites.player_shot[player.job][1];

            int w = al_get_bitmap_width(bmp);
            int h = al_get_bitmap_height(bmp);

            float scale = (float)(DEPTH_MIN_SCALE + ((shots[i].y - 110) / (PLAYER_MAX_Y - 110.0f)) *
                (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE));

            float final_w = w * scale;
            float final_h = h * scale;

            if (shots[i].dir == SHOT_LEFT) {
                al_draw_scaled_bitmap(
                    bmp,
                    0, 0, w, h,
                    shots[i].x + final_w, shots[i].y,
                    -final_w, final_h,
                    0
                );
            }
            else {
                al_draw_scaled_bitmap(
                    bmp,
                    0, 0, w, h,
                    shots[i].x, shots[i].y,
                    final_w, final_h,
                    0
                );
            }
            int sw = (int)final_w;
            int sh = (int)final_h;
            int hitbox_x = shots[i].x;
            int hitbox_h = sh * 0.8f;
            int hitbox_y = shots[i].y + sh - hitbox_h;
            al_draw_rectangle(
                hitbox_x, hitbox_y,
                hitbox_x + sw, hitbox_y + hitbox_h,
                al_map_rgb(255, 0, 0), 1
            );
        }
        else
        {            
            ALLEGRO_BITMAP* bmp = (shots[i].power >= 10)
                ? sprites.enemy_shot[1]
                : sprites.enemy_shot[0];

            int w = al_get_bitmap_width(bmp);
            int h = al_get_bitmap_height(bmp);

            float scale = (float)(DEPTH_MIN_SCALE + ((shots[i].y - 110) / (PLAYER_MAX_Y - 110.0f)) *
                (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE));

            float final_w = w * scale;
            float final_h = h * scale;

            if (shots[i].dir == SHOT_LEFT) {
                al_draw_scaled_bitmap(
                    bmp,
                    0, 0, w, h,
                    shots[i].x + final_w, shots[i].y,
                    -final_w, final_h,
                    0
                );
            }
            else {
                al_draw_scaled_bitmap(
                    bmp,
                    0, 0, w, h,
                    shots[i].x, shots[i].y,
                    final_w, final_h,
                    0
                );
            }
            int sw = (int)final_w;
            int sh = (int)final_h;
            int hitbox_x = shots[i].x;
            int hitbox_h = sh *0.8f;
            int hitbox_y = shots[i].y + sh - hitbox_h;
            al_draw_rectangle(
                hitbox_x, hitbox_y,
                hitbox_x + sw, hitbox_y + hitbox_h,
                al_map_rgb(255, 0, 0), 1
            );

        }
    }
}
