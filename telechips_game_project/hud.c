#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

ALLEGRO_FONT* font_small = NULL;   // 쿨타임 표시용
ALLEGRO_FONT* font_medium = NULL;  // 타이머 표시용
ALLEGRO_FONT* font_large = NULL;   // GAME OVER 표시용

void hud_init()
{
    font = al_create_builtin_font();
    must_init(font, "font");

    font_small = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);
    must_init(font_small, "font_small");

    font_medium = al_load_ttf_font("BebasNeue-Regular.ttf", 100, 0);
    must_init(font_medium, "font_medium");

    font_large = al_load_ttf_font("BebasNeue-Regular.ttf", 150, 0);
    must_init(font_large, "font_large");

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
        font_medium,
        al_map_rgb(0, 0, 0),
        BUFFER_W / 2, 0, // 화면 상단 중앙
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
        font_small,
        al_map_rgb(0, 0, 0),
        20, BUFFER_H - 120,
        ALLEGRO_ALIGN_LEFT,
        normal_text
    );

    // 강한 공격 쿨타임
    al_draw_text(
        font_small,
        al_map_rgb(0, 0, 0),
        20, BUFFER_H - 60,
        ALLEGRO_ALIGN_LEFT,
        strong_text
    );

    // 아이템 버프 남은 시간 표시
    if (player.atk_speed_buff) {
        float buff_time = player.atk_speed_buff_timer / 60.0f;
        char buff_text[32];
        sprintf(buff_text, "BUFF: %.1fs", buff_time);

        al_draw_text(
            font_small,
            al_map_rgb(0, 0, 255),     // 파란색 표시
            20, BUFFER_H - 180,        // 스킬 쿨타임 위쪽에 배치
            ALLEGRO_ALIGN_LEFT,
            buff_text
        );
    }

    // 게임 오버 표시
    if (player.hp <= 0)
    {
        al_draw_text(
            font_large,
            al_map_rgb_f(0, 0, 0),
            BUFFER_W / 2, BUFFER_H / 2 - 120, // 화면 중앙
            ALLEGRO_ALIGN_CENTER, // 가운데 정렬
            "G A M E  O V E R"
        );

        al_draw_text(
            font_medium,
            al_map_rgb_f(0, 0, 0),
            BUFFER_W / 2, (BUFFER_H / 2) + 80,
            ALLEGRO_ALIGN_CENTER,
            "PRESS 'R' TO RESTART"
        );
    }
}

void hud_deinit()
{
    al_destroy_font(font);
    al_destroy_font(font_small);
    al_destroy_font(font_medium);
    al_destroy_font(font_large);
}