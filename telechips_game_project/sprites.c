#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("spritesheet.png");
    must_init(sprites._sheet, "spritesheet");

    sprites.player1 = al_load_bitmap("player.png");
    sprites.player1_attack = al_load_bitmap("player_attack.png");
    sprites.player2 = al_load_bitmap("player2.png");
    sprites.player2_attack = al_load_bitmap("player2_attack.png");

    // ���߿� ������/��ų�� ���� ������ �̹��� �ٸ��� �����ؾ� ��
    sprites.player_shot[JOB_TYPE_1][0] = al_load_bitmap("shot1.png");
    sprites.player_shot[JOB_TYPE_1][1] = al_load_bitmap("shot2.png");
    sprites.player_shot[JOB_TYPE_2][0] = al_load_bitmap("player2_shot1.png");
    sprites.player_shot[JOB_TYPE_2][1] = al_load_bitmap("player2_shot2.png");

    sprites.enemy[0] = al_load_bitmap("monster01.png");
    sprites.enemy[1] = al_load_bitmap("monster02.png");
    sprites.enemy[2] = al_load_bitmap("stage1.png");
    sprites.enemy[3] = al_load_bitmap("boss01.png"); //������ �Ѹ����� 

    sprites.enemy_shot[0] = sprite_grab(13, 10, ENEMY_SHOT_W, ENEMY_SHOT_H); // �Ϲ� ��
    sprites.enemy_shot[1] = sprite_grab(13, 10, ENEMY_SHOT_W, ENEMY_SHOT_H); // ���� ��

    // ��� �̹���
    subway_background = al_load_bitmap("subway_background.png"); 
    subway_floor = al_load_bitmap("subway_floor.jpg");
    background = al_load_bitmap("background.jpg");

    // ������ �̹���
    hp_heal = al_load_bitmap("item_hp_heal.png");
    attack_speed_up = al_load_bitmap("item_attack_speedup.png");
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.player1);
    al_destroy_bitmap(sprites.player1_attack);

    al_destroy_bitmap(sprites.player2);
    al_destroy_bitmap(sprites.player2_attack);

    al_destroy_bitmap(sprites.player_shot[JOB_TYPE_1][0]);
    al_destroy_bitmap(sprites.player_shot[JOB_TYPE_1][1]);
    al_destroy_bitmap(sprites.player_shot[JOB_TYPE_2][0]);
    al_destroy_bitmap(sprites.player_shot[JOB_TYPE_2][1]);

    al_destroy_bitmap(sprites.enemy[0]);
    al_destroy_bitmap(sprites.enemy[1]);

    al_destroy_bitmap(sprites.enemy_shot[0]);
    al_destroy_bitmap(sprites.enemy_shot[1]);

    al_destroy_bitmap(sprites._sheet);

    al_destroy_bitmap(subway_background);
    al_destroy_bitmap(subway_floor);
    al_destroy_bitmap(background);

    al_destroy_bitmap(hp_heal);
    al_destroy_bitmap(attack_speed_up);
}