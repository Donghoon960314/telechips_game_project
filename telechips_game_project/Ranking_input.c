#if 1
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
// 글자 크게 출력하는 헬퍼 함수

void rank_name_open(int time, char * rank_name, int * rank_min, int * rank_sec)//2분한 프레임 7200 굳이 long 쓸필요 x
{
    al_init_font_addon(); //ALLEGRO에서 font 사용 (기본 폰트)
    al_init_ttf_addon();  //기본 포트가 아닌 새로운 폰트 사용가능

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue(); //이벤트(마우스,키보드,디플,타이머)받을 큐 생성

    must_init(queue, "queue"); //잘 생성되었는지 확인
    al_register_event_source(queue, al_get_keyboard_event_source()); //키보드 입력만 받을 거라서 일단 키보드 소스만 등록

 
    char name[10] = ""; //이름 입력 받을 배열 생성 (최대 10 글자)
    int name_len = 0;
    bool user_input = false;

    long total_seconds = time / 60; // 프레임을 분과 초로 변환 
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    char time_text[50]; //시간 입력 받는 텍스트 생성
    int time_text_len = 0;

    time_text[time_text_len++] = 'T'; //배열의 처음부터 생성하기 시작
    time_text[time_text_len++] = 'i';
    time_text[time_text_len++] = 'm';
    time_text[time_text_len++] = 'e';
    time_text[time_text_len++] = ':';
    time_text[time_text_len++] = ' ';
  
    char minutes_str[10];
    int min_len = 0;
    if (minutes == 0) {
        minutes_str[min_len++] = '0';
    }
    else 
    {
        int temp = minutes;
        while (temp > 0) 
        {
            minutes_str[min_len++] = (temp % 10) + '0';
            temp /= 10;
        }
        for (int i = min_len - 1; i >= 0; i--) 
        {
            time_text[time_text_len++] = minutes_str[i];
        }
    }

    time_text[time_text_len++] = ' ';
    time_text[time_text_len++] = 'm';
    time_text[time_text_len++] = 'i';
    time_text[time_text_len++] = 'n';
    time_text[time_text_len++] = ' ';

    char seconds_str[10];
    int sec_len = 0;
    if (seconds == 0) {
        seconds_str[sec_len++] = '0';
    }
    else {
        int temp = seconds;
        while (temp > 0) {
            seconds_str[sec_len++] = (temp % 10) + '0'; //아스키 코드 고려하여 문자로 변환
                                                        //seconds_str에 우선적으로 거꾸로 저장
            temp /= 10;
        }
        for (int i = sec_len - 1; i >= 0; i--) {
            time_text[time_text_len++] = seconds_str[i]; //거꾸로 저장한거 다시 원상태로 다시 저장
        }
    }

    time_text[time_text_len++] = ' ';
    time_text[time_text_len++] = 's';
    time_text[time_text_len++] = 'e';
    time_text[time_text_len++] = 'c';
    time_text[time_text_len] = '\0';

    ALLEGRO_FONT* big_font = al_load_ttf_font("BebasNeue-Regular.ttf", 80, 0);  // 큰 폰트
    ALLEGRO_FONT* medium_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);  // 중간 폰트
    ALLEGRO_FONT* small_font = al_load_ttf_font("BebasNeue-Regular.ttf", 30, 0);  // 작은 폰트

    //여기서 부터는 키보드 이벤트 기다리는 로직 
    while (!user_input)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && name_len > 0) //글자를 지울떄 해당 위치에 null문자를 넣어서 지움
            {
                name[--name_len] = '\0';
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) //엔터입력시 while문 종료
            {
                user_input = true;
            }
            else if (name_len < 9 && event.keyboard.unichar >= 32 && event.keyboard.unichar <= 126) //받을 수 있는 키보드 자판
            {
                name[name_len++] = event.keyboard.unichar;
                name[name_len] = '\0';
            }
        }
     
        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_text(big_font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2 - 100, ALLEGRO_ALIGN_CENTER, "YOU WIN!!");
        al_draw_text(medium_font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2 - 20, ALLEGRO_ALIGN_CENTER, time_text);
        al_draw_text(small_font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2 + 60, ALLEGRO_ALIGN_CENTER, "Enter your name:");
        al_draw_text(medium_font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2 + 110, ALLEGRO_ALIGN_CENTER, name);

        al_flip_display(); //버퍼에 있는걸 화면에 출력 //더블버퍼를 사용해서 부드럽게
       

    }
    printf("Player Name: %s, Time: %d min %d sec\n", name, minutes, seconds);

    FILE* f = fopen("rank.txt", "a");
    if (f) {
        fprintf(f, "%s %d %d\n", name, minutes, seconds);
        fclose(f);
    }
    al_destroy_event_queue(queue);
    strcpy(rank_name, name);
    *rank_min = minutes;
    *rank_sec = seconds;
}


#endif