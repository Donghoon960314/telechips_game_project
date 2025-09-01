//======================================================
//                    hud.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      hud.c
 @brief     게임의 hud 구성
 @author    김혁, 신동훈, 정명훈, 이재강
*/

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

    for (int i = 0; i < HEAL_TEXTS_N; i++) {
        heal_texts[i].active = false;
    }

    score_display = 0;
}

void hud_update() {
    for (int i = 0; i < HEAL_TEXTS_N; i++) {
        if (heal_texts[i].active) {
            heal_texts[i].y -= 1;      // 위로 이동
            heal_texts[i].timer--;
            if (heal_texts[i].timer <= 0) {
                heal_texts[i].active = false;
            }
        }
    }
}

void hud_draw()
{
    // 남은 시간 표시
    int minutes = time_left / 60;
    int seconds = time_left % 60;

    // 0.5초마다 깜박이는 효과
    bool blink = (frames / 30) % 2;
    char time_text[16];
    if (blink)
        sprintf(time_text, "%02d:%02d", minutes, seconds);
    else
        sprintf(time_text, "%02d %02d", minutes, seconds);

    // 색상 변화 (기본: 흰색 → 30초 이하: 주황 → 10초 이하: 빨강)
    ALLEGRO_COLOR col = al_map_rgb(255, 255, 255);
    if (time_left <= 10)       col = al_map_rgb(255, 0, 0);
    else if (time_left <= 30)  col = al_map_rgb(255, 165, 0);

    // 좌표
    int x = BUFFER_W / 2;
    int y = 20;

    // 반투명 박스 배경
    al_draw_filled_rectangle(
        x - 100, y,
        x + 100, y + 110,
        al_map_rgba(0, 0, 0, 150) // 반투명 검정
    );

    // 그림자 효과 (글자를 약간 오른쪽/아래로 그려줌)
    al_draw_text(font_medium, al_map_rgb(30, 30, 30),
        x + 3, y + 3, ALLEGRO_ALIGN_CENTER, time_text);

    // 본문 글자 (색상 변화 적용)
    al_draw_text(font_medium, col,
        x, y, ALLEGRO_ALIGN_CENTER, time_text);

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


    for (int i = 0; i < HEAL_TEXTS_N; i++) {
        if (heal_texts[i].active) {
            al_draw_text(
                font_small,
                al_map_rgb(0, 200, 0),   // 초록색
                heal_texts[i].x,
                heal_texts[i].y,
                ALLEGRO_ALIGN_CENTER,
                heal_texts[i].text
            );
        }
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