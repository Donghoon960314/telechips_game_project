#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

float shake_y = 0.0f; 
int shake_timer = 0;

void update_shake() {
    if (shake_timer <= 0) {
        shake_y = between(-10, 10); // 흔들림 정도
        shake_timer = between(10, 15); // 흔들림 주기
    }
    else {
        shake_timer--;
    }
}

// 창문 밖 배경
void draw_background() {
    if (!background) return;

    int bg_w = al_get_bitmap_width(background);
    int bg_h = al_get_bitmap_height(background);

    int dest_h = (int)(BUFFER_H * 0.40f);

    float scale = (float)dest_h / bg_h;
    int scaled_w = (int)(bg_w * scale);

    // 스크롤
    static float background_offset = 0.0f;
    float scroll_speed = 4.0f;
    background_offset += scroll_speed;
    if (background_offset >= scaled_w) {
        background_offset -= scaled_w;
    }

    for (int x = -background_offset; x < BUFFER_W; x += scaled_w) {
        al_draw_scaled_bitmap(
            background,
            0, 0, bg_w, bg_h,
            x, (int)shake_y, scaled_w, dest_h,
            0
        );
    }
}

// 창문 프레임
void draw_subway_background() {
    if (!subway_background) return;

    int src_w = al_get_bitmap_width(subway_background);
    int src_h = al_get_bitmap_height(subway_background);

    al_draw_scaled_bitmap(
        subway_background,
        0, 0, src_w, src_h,
        0, (int)(shake_y * 0.4f), BUFFER_W, (int)(BUFFER_H * 0.50f),
        0
    );
}

// 바닥 (🚉 작게 흔들림)
void draw_floor() {
    if (!subway_floor) return;

    int src_w = al_get_bitmap_width(subway_floor);
    int src_h = al_get_bitmap_height(subway_floor);

    al_draw_scaled_bitmap(
        subway_floor,
        0, 0, src_w, src_h,
        0, (int)(BUFFER_H * 0.50f + shake_y * 0.4f),
        BUFFER_W, BUFFER_H - (int)(BUFFER_H * 0.50f),
        0
    );
}

// 바닥 가로선 
void draw_horizon_lines() {
    float min_gap = 50; // 위쪽 간격 (좁음)
    float max_gap = 150; // 아래쪽 간격 (넓음)
    float min_thick = 2; // 위쪽 라인 두께 (얇음)
    float max_thick = 20; // 아래쪽 라인 두께 (굵음)

    float start_y = BUFFER_H * 0.5f + shake_y * 0.4f;
    float end_y = BUFFER_H + shake_y * 0.4f;          
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

// 바닥 세로선
void draw_vertical_lines() {
    float start_y = 110 + shake_y * 0.4f;
    float end_y = BUFFER_H + shake_y * 0.4f;
    float vanishing_x = BUFFER_W / 2;
    float vanishing_y = -100 + shake_y * 0.4f;

    // 그릴 세로선 개수
    int lines = 4;

    for (int i = 0; i <= lines; i++) {
        float t = (float)i / lines;
        float bottom_x = t * BUFFER_W;
        float top_x = vanishing_x;

        float min_thick = 1.0f;
        float max_thick = 5.0f;
        float thick = min_thick + t * (max_thick - min_thick);

        al_draw_line(
            top_x, vanishing_y,
            bottom_x, end_y,
            al_map_rgb(200, 200, 200),
            thick
        );
    }
}