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
    // ���� �ð� ǥ��
    int minutes = time_left / 60;
    int seconds = time_left % 60;

    char time_text[16];
    sprintf(time_text, "%02d:%02d", minutes, seconds);

    al_draw_text(
        font,
        al_map_rgb(0, 0, 0),
        BUFFER_W / 2, 5, // ȭ�� ��� �߾�
        ALLEGRO_ALIGN_CENTER,
        time_text
    );

    // ���� ��Ÿ�� ǥ��
    float normal_cd = player.normal_shot_timer / 60.0f;
    float strong_cd = player.skill_1_timer/ 60.0f;

    char normal_text[32];
    char strong_text[32];

    sprintf(normal_text, "X: %.1fs", normal_cd);
    sprintf(strong_text, "Z: %.1fs", strong_cd);

    // �Ϲ� ���� ��Ÿ��
    al_draw_text(
        font,
        al_map_rgb(0, 0, 0),
        5, BUFFER_H - 35,
        ALLEGRO_ALIGN_LEFT,
        normal_text
    );

    // ���� ���� ��Ÿ��
    al_draw_text(
        font,
        al_map_rgb(0, 0, 0),
        5, BUFFER_H - 20,
        ALLEGRO_ALIGN_LEFT,
        strong_text
    );

    // ���� ���� ǥ��
    if (player.hp <= 0)
    {
        al_draw_text(
            font,
            al_map_rgb_f(0, 0, 0),
            BUFFER_W / 2, BUFFER_H / 2, // ȭ�� �߾�
            ALLEGRO_ALIGN_CENTER, // ��� ����
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