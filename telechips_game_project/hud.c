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

void hud_init()
{
    font = al_create_builtin_font();
    must_init(font, "font");

    score_display = 0;
}

void hud_draw()
{
    // 남은 시간 표시
    int minutes = time_left / 60;
    int seconds = time_left % 60;

    char time_text[16];
    sprintf(time_text, "%02d:%02d", minutes, seconds);

    al_draw_text(
        font,
        al_map_rgb(0, 0, 0),
        BUFFER_W / 2, 5, // 화면 상단 중앙
        ALLEGRO_ALIGN_CENTER,
        time_text
    );

    // 공격 쿨타임 표시
    float normal_cd = player.normal_shot_timer / 60.0f;
    float strong_cd = player.skill_1_timer/ 60.0f;

    char normal_text[32];
    char strong_text[32];

    sprintf(normal_text, "X: %.1fs", normal_cd);
    sprintf(strong_text, "Z: %.1fs", strong_cd);

    // 일반 공격 쿨타임
    al_draw_text(
        font,
        al_map_rgb(0, 0, 0),
        5, BUFFER_H - 35,
        ALLEGRO_ALIGN_LEFT,
        normal_text
    );

    // 강한 공격 쿨타임
    al_draw_text(
        font,
        al_map_rgb(0, 0, 0),
        5, BUFFER_H - 20,
        ALLEGRO_ALIGN_LEFT,
        strong_text
    );

    // 게임 오버 표시
    if (player.hp <= 0)
    {
        al_draw_text(
            font,
            al_map_rgb_f(0, 0, 0),
            BUFFER_W / 2, BUFFER_H / 2, // 화면 중앙
            ALLEGRO_ALIGN_CENTER, // 가운데 정렬
            "G A M E  O V E R"
        );

        al_draw_text(
            font,
            al_map_rgb_f(0, 0, 0),
            BUFFER_W / 2, (BUFFER_H / 2) + 20,
            ALLEGRO_ALIGN_CENTER,
            "PRESS 'R' TO RESTART"
        );
    }
}

void hud_deinit()
{
    al_destroy_font(font);
}