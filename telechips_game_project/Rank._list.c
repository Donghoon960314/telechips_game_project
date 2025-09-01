//======================================================
//                    Rank._list.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      Rank._list.c
 @brief     난이도 별 랭킹 정렬 및 출력
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

//분 단위로는 오름차순->분이 같을때 초단위로 오름차순
int cmp_rank(const void* a, const void* b) 
{
    const _RANK* x = (const _RANK*)a; //Rank구조체 가르키는 포인터 x와 y
    const _RANK* y = (const _RANK*)b;

    if (x->minutes != y->minutes) return (x->minutes - y->minutes);
    return (x->seconds - y->seconds);
}

void print_ranking_table(const char* player_name, int player_min, int player_sec, RankMode mode)
{
    _RANK entries[128];
    int count = 0;
    //ALLEGRO_FONT* name_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);
    //ALLEGRO_FONT* title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 80, 0);

    // rank.txt 읽기
    if (mode == END_TO_RANK)
    {
        FILE* f = NULL;

        if (game_difficulty == DIFF_EASY)
        {
            f = fopen("Rank_Easy.txt", "r");
        }
        else if (game_difficulty == DIFF_NORMAL)
        {
            f = fopen("Rank_Normal.txt", "r");
        }
        else if (game_difficulty == DIFF_HARD)
        {
            f = fopen("Rank_Hard.txt", "r");
        }
        else
        {
            f = fopen("Rank_Hard.txt", "r");
        }

        if (f)
        {
            while (fscanf(f, "%31s %d %d", entries[count].name, &entries[count].minutes, &entries[count].seconds) == 3)
            {
                count++;
                if (count >= 128) break;
            }
            fclose(f);
        }
        qsort(entries, count, sizeof(_RANK), cmp_rank);

        // 제목
        if (game_difficulty == DIFF_EASY)
        {
            al_draw_text(title_font, al_map_rgb(255, 255, 255), DISP_W / 2, 20, ALLEGRO_ALIGN_CENTER, "EASY_MODE - TOP 10");
            al_draw_text(name_font, al_map_rgb(200, 200, 0), BUFFER_W - 10, BUFFER_H - al_get_font_line_height(name_font) - 10, ALLEGRO_ALIGN_RIGHT, "Press ESC Back");

        }
        else if (game_difficulty == DIFF_NORMAL)
        {
            al_draw_text(title_font, al_map_rgb(255, 255, 255), DISP_W / 2, 20, ALLEGRO_ALIGN_CENTER, "NORMAL_MODE - TOP 10");
            al_draw_text(name_font, al_map_rgb(0, 0, 200), BUFFER_W - 10, BUFFER_H - al_get_font_line_height(name_font) - 10, ALLEGRO_ALIGN_RIGHT, "Press ESC to return to menu");

        }
        else if (game_difficulty == DIFF_HARD)
        {
            al_draw_text(title_font, al_map_rgb(255, 255, 255), DISP_W / 2, 20, ALLEGRO_ALIGN_CENTER, "HARD_MODE - TOP 10");
            al_draw_text(name_font, al_map_rgb(0, 0, 200), BUFFER_W - 10, BUFFER_H - al_get_font_line_height(name_font) - 10, ALLEGRO_ALIGN_RIGHT, "Press ESC to return to menu");


        }
        else
        {
            al_draw_text(title_font, al_map_rgb(255, 255, 255), DISP_W / 2, 20, ALLEGRO_ALIGN_CENTER, "HARD_MODE - TOP 10");
            al_draw_text(name_font, al_map_rgb(0, 0, 200), BUFFER_W - 10, BUFFER_H - al_get_font_line_height(name_font) - 10, ALLEGRO_ALIGN_RIGHT, "Press ESC to return to menu");

        }


        // 상위 10개 출력
        for (int i = 0; i < count && i < 10; i++)
        {

            char buf[128];
            sprintf(buf, "%2d. %-10s  %02d:%02d", i + 1, entries[i].name, entries[i].minutes, entries[i].seconds);

            int y = 120 + i * 80;
            if (strcmp(entries[i].name, player_name) == 0 && entries[i].minutes == player_min && entries[i].seconds == player_sec)
            {
                al_draw_filled_rectangle(DISP_W / 2 - 300, y - 10, DISP_W / 2 + 300, y + 60, al_map_rgb(50, 50, 150));
            }
            al_draw_text(name_font, al_map_rgb(200, 200, 0), DISP_W / 2, y, ALLEGRO_ALIGN_CENTER, buf);

        }
    }
    else if (mode == BUTTON_TO_RANK)
    {
        const char* files[3] = { "Rank_Easy.txt","Rank_Normal.txt","Rank_Hard.txt" };
        const char* titles[3] = { "EASY_MODE - TOP 10","NORMAL_MODE - TOP 10","HARD_MODE - TOP 10" };

        for (int d = 0; d < 3; d++) {
            count = 0;
            FILE* f = fopen(files[d], "r");
            if (f) {
                while (fscanf(f, "%31s %d %d",
                    entries[count].name,
                    &entries[count].minutes,
                    &entries[count].seconds) == 3)
                {
                    count++;
                    if (count >= 128) break;
                }
                fclose(f);
            }
            qsort(entries, count, sizeof(_RANK), cmp_rank);

            int base_x = (DISP_W / 6) + (d * DISP_W / 3);
            int base_y = 50;

            al_draw_text(button_to_rank_title_font, al_map_rgb(255, 255, 255), base_x, base_y, ALLEGRO_ALIGN_CENTER, titles[d]);
            al_draw_text(name_font, al_map_rgb(0, 0, 200), BUFFER_W - 10, BUFFER_H - al_get_font_line_height(name_font) - 10, ALLEGRO_ALIGN_RIGHT, "Press ESC to return to menu");


            // 상위 10 출력
            for (int i = 0; i < count && i < 10; i++) {
                char buf[128];
                sprintf(buf, "%2d. %-10s  %02d:%02d", i + 1, entries[i].name, entries[i].minutes, entries[i].seconds);

                int y = base_y + 80 + i * 60; // 제목 밑 60px부터, 한 줄 30px 간격
                al_draw_text(name_font, al_map_rgb(200, 200, 0), base_x, y, ALLEGRO_ALIGN_CENTER, buf);
            }
        }
    }
}
