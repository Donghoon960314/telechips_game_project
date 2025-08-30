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

    sprites.player = al_load_bitmap("player.png");
    sprites.player_attack = al_load_bitmap("player_attack.png");

    sprites.player_shot[0] = al_load_bitmap("shot1.png");
    sprites.player_shot[1] = al_load_bitmap("shot2.png");

    sprites.enemy[0] = al_load_bitmap("monster01.png");
    sprites.enemy[1] = al_load_bitmap("monster02.png");
    sprites.enemy[2] = al_load_bitmap("boss01.png");
    sprites.enemy[3] = al_load_bitmap("boss02.png");


    sprites.enemy_shot = sprite_grab(13, 10, ENEMY_SHOT_W, ENEMY_SHOT_H);

    // 배경 이미지
    subway_background = al_load_bitmap("subway_background.jpg"); 
    subway_floor = al_load_bitmap("subway_floor.jpg");
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.player);
    al_destroy_bitmap(sprites.player_attack);

    al_destroy_bitmap(sprites.player_shot[0]);
    al_destroy_bitmap(sprites.player_shot[1]);

    al_destroy_bitmap(sprites.enemy[0]);
    al_destroy_bitmap(sprites.enemy[1]);

    al_destroy_bitmap(sprites.enemy_shot);

    al_destroy_bitmap(sprites._sheet);

    al_destroy_bitmap(subway_background);
    al_destroy_bitmap(subway_floor);
}