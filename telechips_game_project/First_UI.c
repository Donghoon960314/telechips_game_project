//======================================================
//                    FIRST_UI.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      FIRST_UI.c
 @brief     게임 시작부 UI 구성
 @author    김혁, 신동훈, 정명훈, 이재강
*/

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"


// 게임 시작 화면 버튼 3개
// 난이도에 따른 버튼 3개 구
// 배경 화면 넣기

Button pos1 = { BUFFER_W / 2 - 120 , (BUFFER_H / 2) - 180 , 240, 100, "START", "", false, true };
Button pos2 = { BUFFER_W /2 + 440 , (BUFFER_H / 2) - 440, 240, 100, "BACK" , "", false, false };
Button pos3 = { BUFFER_W / 2 - 120, (BUFFER_H / 2)  , 240, 100, "TUTORIAL", "", false, true };
Button pos4 = { BUFFER_W / 2 - 120, (BUFFER_H / 2) + 180, 240, 100, "RANKING", "", false, true };
Button pos5 = { BUFFER_W / 2 - 120, (BUFFER_H / 2) - 180, 240, 100, "Easy", "", false, true };
Button pos6 = { BUFFER_W / 2 - 120, (BUFFER_H / 2) , 240, 100, "Normal", "", false, true };
Button pos7 = { BUFFER_W / 2 - 120, (BUFFER_H / 2) + 180, 240, 100, "Hard", "", false, true };
Button pos8 = { BUFFER_W / 2 - 300, (BUFFER_H / 2)  , 240, 100, "Tanjiro", "", false, true };
Button pos9 = { BUFFER_W / 2 + 100, (BUFFER_H / 2)  , 240, 100, "Rengoku", "", false, true };

Button* BUT_List[] = { &pos1, &pos2, &pos3, &pos4 ,&pos5, &pos6, &pos7, &pos8, &pos9 };

/* 히트 테스트는 항상 '버퍼 좌표계'로 비교해야 정확 */
bool pt_in_rect(float px, float py, const Button* b) {
    return b->active &&
        (px >= b->x && px <= b->x + b->w &&
            py >= b->y && py <= b->y + b->h);
}

Button* hit_button(float bx, float by)
{
    for (int i = 0; i < BTN_COUNT; ++i) {
        if (pt_in_rect(bx, by, BUT_List[i]))
            return BUT_List[i];
    }
    return NULL;
}

void update_hover_all(float bx, float by)
{
    for (int i = 0; i < BTN_COUNT; ++i) {
        BUT_List[i]->hover = pt_in_rect(bx, by, BUT_List[i]);
    }
}


/* 스케일된 디스플레이 좌표 -> 버퍼 좌표 변환 */
float to_buffer_x(float mx)
{
    return mx * (float)BUFFER_W / (float)DISP_W;
}

float to_buffer_y(float my)
{
    return my * (float)BUFFER_H / (float)DISP_H;
}


/* 버튼 설계 함수(직사각형 라이브러리 , 도형 색깔 채움, 폰트 하이라이트) */
void Button_draw(const Button* pos, ALLEGRO_FONT* font)
{
    if (!pos->active) return;

    // 버튼 보드 색깔 
    ALLEGRO_COLOR btn_col = pos->hover
        ? al_map_rgb(80, 170, 240)
        : al_map_rgb(100, 100, 100);
    ALLEGRO_COLOR btn_border = al_map_rgb(10, 10, 10);

    al_draw_filled_rectangle(pos->x, pos->y, pos->x + pos->w, pos->y + pos->h, btn_col);

    al_draw_rectangle(pos->x, pos->y, pos->x + pos->w, pos->y + pos->h, btn_border, 4.0f);
    
    int tw = al_get_text_width(font, pos->label1);
    int th = al_get_font_line_height(font);
    float tx = pos->x + (pos->w - tw) / 2.0f;
    float ty = pos->y + (pos->h - th) / 2.0f;

    al_draw_text(font, al_map_rgb(255, 255, 255), tx, ty, 0, pos->label1);
}

job = JOB_None;

/* --- display menu ---  */
void show_back_only(void)
{
    pos1.active = false;
    pos3.active = false;
    pos4.active = false;
    pos5.active = false;
    pos6.active = false;
    pos7.active = false;
    pos8.active = false;
    pos9.active = false;

    // Back 버튼 활성화
    pos2.active = true;
}

void show_main_menu(void)
{
    pos1.active = true;
    pos3.active = true;
    pos4.active = true;
    pos2.active = false;    //  Back 버튼 비활성화
}

ALLEGRO_BITMAP* prologue_List[PRO] = { 0 };

PROLOGUE_STATE ps = { 0,0,0,false };

// 직업에 따라 달라지는 프롤로그 설정 함수
void set_pro_job(void)
{
    switch (job_type)
    {

    case JOB_TYPE_1: // Tanjiro

        ps.start = 0;
        ps.end = 6;
        break;
    case JOB_TYPE_2: // Rengoku
        ps.start = 7;
        ps.end = 13;
    }

    ps.curr = ps.start;
    ps.blink = false;

}

// 프롤로그 화면 함수
void prologue_display(ALLEGRO_BITMAP* bitmap)
{
    // 배경화면 사진을 버퍼 크기에 맞추기
    float w = al_get_bitmap_width(bitmap);
    float h = al_get_bitmap_height(bitmap);
    float dw = BUFFER_W;
    float dh = BUFFER_H;
    al_draw_scaled_bitmap(bitmap,
        0, 0, w, h,
        0, 0, dw, dh,
        0);

}

// 튜토리얼 화면
void Tutorial_display(ALLEGRO_BITMAP* bitmap)
{
    // 배경화면 사진을 버퍼 크기에 맞추기
    float w = al_get_bitmap_width(bitmap);
    float h = al_get_bitmap_height(bitmap);
    float dw = BUFFER_W;
    float dh = BUFFER_H;
    al_draw_scaled_bitmap(bitmap,
        0, 0, w, h,
        400, 90, w, h,
        0);

}

// 프롤로그 비트맵 로드 함수
void load_slides(void)
{

    char* files[PRO] = {
        "prologue_dialog_01.png",
        "prologue_dialog_02.png",
        "prologue_dialog_03.png",
        "prologue_dialog_04.png",
        "prologue_dialog_05.png",
        "prologue_dialog_06.png",
        "prologue_dialog_07.png",
        "prologue_dialog_08.png",
        "prologue_dialog_09.png",
        "prologue_dialog_10.png",
        "prologue_dialog_11.png",
        "prologue_dialog_12.png",
        "prologue_dialog_13.png",
        "prologue_dialog_14.png"
    };

    for (int i = 0; i < PRO; ++i)
    {
        prologue_List[i] = al_load_bitmap(files[i]);
        must_init(prologue_List[i], files[i]);
    }

}


// 프롤로그 화면 순차적으로 넘기는 함수
void next_slide()
{
    // 공백일 때
    if (ps.blink)
        return;

    // 슬라이드의 수만큼 슬라이드 인덱스 추가
    if (ps.curr < ps.end)
        ps.curr++;
    // 마지막 화면을 지날 시에 블랙 화면으로 고정
    else {
        ps.blink = true;
    }
}