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
                items[i].x = between(50, BUFFER_W - 50);
                items[i].y = between(PLAYER_MIN_Y, PLAYER_MAX_Y - 50);

                items[i].type = (rand() % 2 == 0) ? ITEM_HEAL : ITEM_ATK_SPEED;
                items[i].used = true;
                break;
            }
        }

        item_spawn_timer = 60;
    }
     // �÷��̾� <-> ������ �浹 ����
    for (int i = 0; i < ITEMS_N; i++)
    {
        if (!items[i].used) continue;

        if (collide(player.x, player.y,
            player.x + PLAYER_W, player.y + PLAYER_H,
            items[i].x, items[i].y,
            items[i].x + 32, items[i].y + 32))
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
            al_draw_filled_circle(items[i].x, items[i].y, 15, al_map_rgb(255, 0, 0)); // HP ȸ�� �� ����
        else if (items[i].type == ITEM_ATK_SPEED)
            al_draw_filled_circle(items[i].x, items[i].y, 15, al_map_rgb(0, 0, 255)); // ���� ���� �� �Ķ�

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

