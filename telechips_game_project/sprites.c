//======================================================
//                    sprites.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      sprites.c
 @brief     ���ӿ� ���̴� �׸� ���ϵ� ȣ�� , �ʱ�ȭ �� ����
 @author    ����, �ŵ���, ������, ���簭
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

void sprites_init()
{
    sprites.player1 = al_load_bitmap("player.png");
    sprites.player1_attack1 = al_load_bitmap("player_attack1.png");
    sprites.player1_attack2 = al_load_bitmap("player_attack2.png");

    sprites.player2 = al_load_bitmap("player2.png");
    sprites.player2_attack1 = al_load_bitmap("player2_attack1.png");
    sprites.player2_attack2 = al_load_bitmap("player2_attack2.png");

    // ���߿� ������/��ų�� ���� ������ �̹��� �ٸ��� �����ؾ� ��
    sprites.player_shot[JOB_TYPE_1][0] = al_load_bitmap("player1_shot1.png");
    sprites.player_shot[JOB_TYPE_1][1] = al_load_bitmap("player1_shot2.png");
    sprites.player_shot[JOB_TYPE_2][0] = al_load_bitmap("player2_shot1.png");
    sprites.player_shot[JOB_TYPE_2][1] = al_load_bitmap("player2_shot2.png");

    sprites.enemy[0] = al_load_bitmap("monster01.png");
    sprites.enemy[1] = al_load_bitmap("monster02.png");
    sprites.enemy[2] = al_load_bitmap("boss01.png");
    sprites.enemy[3] = al_load_bitmap("boss02.png"); //������ �Ѹ����� 


    sprites.enemy_shot[0] = al_load_bitmap("enemy_attack.png"); // �Ϲ� ��
    sprites.enemy_shot[1] = al_load_bitmap("boss_shot1.png"); // ���� ��
    sprites.enemy_shot[2] = al_load_bitmap("boss_shot2.png"); // ���� ��

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
    al_destroy_bitmap(sprites.player1_attack1);
    al_destroy_bitmap(sprites.player1_attack2);

    al_destroy_bitmap(sprites.player2);
    al_destroy_bitmap(sprites.player2_attack1);
    al_destroy_bitmap(sprites.player2_attack2);

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