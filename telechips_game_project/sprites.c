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

    // 나중에 직업별/스킬별 공격 나가는 이미지 다르게 설정해야 함
    sprites.player_shot[JOB_TYPE_1][0] = al_load_bitmap("shot1.png");
    sprites.player_shot[JOB_TYPE_1][1] = al_load_bitmap("shot2.png");
    sprites.player_shot[JOB_TYPE_2][0] = al_load_bitmap("player2_shot1.png");
    sprites.player_shot[JOB_TYPE_2][1] = al_load_bitmap("player2_shot2.png");

    sprites.enemy[0] = al_load_bitmap("monster01.png");
    sprites.enemy[1] = al_load_bitmap("monster02.png");
    sprites.enemy[2] = al_load_bitmap("boss01.png");
    sprites.enemy[3] = al_load_bitmap("boss02.png");


    sprites.enemy_shot[0] = al_load_bitmap("enemy_attack.png"); // 일반 몹
    sprites.enemy_shot[1] = al_load_bitmap("boss_shot1.png"); // 보스 몹

    // 배경 이미지
    subway_background = al_load_bitmap("subway_background.jpg"); 
    subway_floor = al_load_bitmap("subway_floor.jpg");
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
}