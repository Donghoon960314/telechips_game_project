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

    item_spawn_timer = 180; // ������ ���� �ð� ����
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
    // �÷��̾� <-> ������ �浹 ����
    for (int i = 0; i < ITEMS_N; i++)
    {
        if (!items[i].used) continue;

        if (--items[i].lifetime <= 0) {
            items[i].used = false;
            continue;
        }

        // =====================================
        //        �÷��̾� ��Ʈ�ڽ� ���
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
        //         ������ ��Ʈ�ڽ� ���
        // =====================================
        ALLEGRO_BITMAP* bmp = (items[i].type == ITEM_HEAL) ? hp_heal : attack_speed_up;
        int w = al_get_bitmap_width(bmp);
        int h = al_get_bitmap_height(bmp);

        // �÷��̾�� ���� ���� ������ ���
        float t_item = (float)(items[i].y - PLAYER_MIN_Y) / (PLAYER_MAX_Y - PLAYER_MIN_Y);
        if (t_item < 0) t_item = 0;
        if (t_item > 1) t_item = 1;
        float item_scale = DEPTH_MIN_SCALE + t_item * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        // ������ ����� ���� �浹 �ڽ�
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
            if (items[i].type == ITEM_HEAL) // ü�� ȸ�� ������
            {
                player.hp += 20;
                if (player.hp > player.max_hp)
                    player.hp = player.max_hp;

                for (int j = 0; j < HEAL_TEXTS_N; j++) {
                    if (!heal_texts[j].active) {
                        heal_texts[j].active = true;
                        heal_texts[j].x = player.x + PLAYER_W / 2;
                        heal_texts[j].y = player.y - 30;
                        heal_texts[j].timer = 60; // 1�� ���� ǥ�� (60fps ����)
                        sprintf(heal_texts[j].text, "+20");
                        break;
                    }
                }
            }

            else if (items[i].type == ITEM_ATK_SPEED) // ���� �ӵ� ���� ������
            {
                player.atk_speed_buff = true;
                player.atk_speed_buff_timer = 300; // ���� ���� �ð�

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

    // ���� �ӵ� ����
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

        // ������ 2�� ���� ������
        if (items[i].lifetime <= 120) {
            if ((items[i].lifetime / 10) % 2 == 0) {
                continue;
            }
        }

        // ���̿� ���� ������ ���
        float t = (float)(items[i].y - PLAYER_MIN_Y) / (PLAYER_MAX_Y - PLAYER_MIN_Y);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        float scale = DEPTH_MIN_SCALE + t * (DEPTH_MAX_SCALE - DEPTH_MIN_SCALE);

        // ������ ��Ʈ�ʰ� ���� ũ��
        ALLEGRO_BITMAP* bmp = (items[i].type == ITEM_HEAL) ? hp_heal : attack_speed_up;
        int w = al_get_bitmap_width(bmp);
        int h = al_get_bitmap_height(bmp);

        // ������ �����ؼ� �߾� �������� �׸���
        al_draw_scaled_bitmap(
            bmp,
            0, 0, w, h,                                // ���� ��ü
            items[i].x - (w * scale) / 2,              // ȭ�� X (�߾� ����)
            items[i].y - (h * scale) / 2,              // ȭ�� Y (�߾� ����)
            w * scale, h * scale,                      // ������ ���� ũ��
            0
        );
    }
}

