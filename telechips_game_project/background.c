#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

void draw_background() {
    if (!subway_background) return;

    int src_w = al_get_bitmap_width(subway_background);
    int src_h = al_get_bitmap_height(subway_background);

    al_draw_scaled_bitmap(
        subway_background,
        0, 0, src_w, src_h,
        0, 0, BUFFER_W, (int)(BUFFER_H * 0.50f),
        0
    );
}

void draw_floor() {
    if (!subway_floor) return;

    int src_w = al_get_bitmap_width(subway_floor);
    int src_h = al_get_bitmap_height(subway_floor);

    al_draw_scaled_bitmap(
        subway_floor,
        0, 0, src_w, src_h,
        0, (int)(BUFFER_H * 0.50f), BUFFER_W, BUFFER_H - (int)(BUFFER_H * 0.50f),
        0
    );
}

void draw_horizon_lines() {
    float min_gap = 50; // 위쪽 간격 (좁음)
    float max_gap = 150; // 아래쪽 간격 (넓음)
    float min_thick = 2; // 위쪽 라인 두께 (얇음)
    float max_thick = 20; // 아래쪽 라인 두께 (굵음)

    float start_y = BUFFER_H * 0.5f; // 지하철 바닥 배경부터 시작
    float end_y = BUFFER_H; // 화면 끝
    float y = start_y;

    while (y <= end_y) {
        float t = (y - start_y) / (end_y - start_y);

        float gap = min_gap + t * (max_gap - min_gap);
        float thick = min_thick + t * (max_thick - min_thick);

        al_draw_line(
            0, y, BUFFER_W, y,
            al_map_rgb(200, 200, 200),
            thick
        );

        y += gap;
    }
}

void draw_vertical_lines() {
    float start_y = 110;
    float end_y = BUFFER_H; // 바닥 끝
    float vanishing_x = BUFFER_W / 2; // 소실점 x 좌표
    float vanishing_y = -100;  // 소실점 y좌표

    // 그릴 세로선의 개수
    int lines = 4;

    for (int i = 0; i <= lines; i++) {
        // 0.0 ~ 1.0 사이 비율
        float t = (float)i / lines;
        // 아래쪽에서의 x 좌표
        float bottom_x = t * BUFFER_W;
        // 위쪽에서는 소실점으로 수렴
        float top_x = vanishing_x;

        // 아래쪽일수록 굵게
        float min_thick = 1.0f;
        float max_thick = 5.0f;
        float thick = min_thick + (end_y - start_y) / (end_y - vanishing_y) * (max_thick - min_thick);

        // 색상도 위쪽은 흐리고, 아래쪽은 진하게
        ALLEGRO_COLOR color = al_map_rgb(200, 200, 200);

        al_draw_line(
            top_x, vanishing_y,
            bottom_x, end_y,
            color,
            thick
        );
    }
}

