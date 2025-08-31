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
                break;
            }
        }

        item_spawn_timer = 180;
    }
    // �÷��̾� <-> ������ �浹 ����
    for (int i = 0; i < ITEMS_N; i++)
    {
        if (!items[i].used) continue;

        // =====================================
        //  �÷��̾� ��Ʈ�ڽ� ���(���߿� ����)
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

        // ==============================================================
        // ������ ��Ʈ�ڽ� (32x32 ����, ���߿� �̹��� ũ�⿡ ���缭 ����)
        // ==============================================================
        int item_x1 = items[i].x - 32;
        int item_y1 = items[i].y - 32;
        int item_x2 = items[i].x;
        int item_y2 = items[i].y;

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

        if (items[i].type == ITEM_HEAL)
            al_draw_filled_circle(
                items[i].x, items[i].y, 15, al_map_rgb(255, 0, 0)); // HP ȸ�� �� ����
        else if (items[i].type == ITEM_ATK_SPEED)
            al_draw_filled_circle(
                items[i].x, items[i].y, 15, al_map_rgb(0, 0, 255)); // ���� ���� �� �Ķ�

        // ==============================
        //         ��Ʈ�ڽ� ǥ��
        // ==============================
        al_draw_rectangle(
            items[i].x, items[i].y,                 // �»��
            items[i].x + 32, items[i].y + 32,       // ���ϴ�
            al_map_rgb(255, 0, 0), 2                // ������, �β� 2
        );
        // ==============================
        //         ��Ʈ�ڽ� ǥ��
        // ==============================
    }
}

