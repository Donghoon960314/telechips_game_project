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
// ���� ũ�� ����ϴ� ���� �Լ�

void rank_name_open(int time, char * rank_name, int * rank_min, int * rank_sec)//2���� ������ 7200 ���� long ���ʿ� x
{
    al_init_font_addon(); //ALLEGRO���� font ��� (�⺻ ��Ʈ)
    al_init_ttf_addon();  //�⺻ ��Ʈ�� �ƴ� ���ο� ��Ʈ ��밡��

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue(); //�̺�Ʈ(���콺,Ű����,����,Ÿ�̸�)���� ť ����

    must_init(queue, "queue"); //�� �����Ǿ����� Ȯ��
    al_register_event_source(queue, al_get_keyboard_event_source()); //Ű���� �Է¸� ���� �Ŷ� �ϴ� Ű���� �ҽ��� ���

 
    char name[10] = ""; //�̸� �Է� ���� �迭 ���� (�ִ� 10 ����)
    int name_len = 0;
    bool user_input = false;

    long total_seconds = time / 60; // �������� �а� �ʷ� ��ȯ 
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    char time_text[50]; //�ð� �Է� �޴� �ؽ�Ʈ ����
    int time_text_len = 0;

    time_text[time_text_len++] = 'T'; //�迭�� ó������ �����ϱ� ����
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
            seconds_str[sec_len++] = (temp % 10) + '0'; //�ƽ�Ű �ڵ� ����Ͽ� ���ڷ� ��ȯ
                                                        //seconds_str�� �켱������ �Ųٷ� ����
            temp /= 10;
        }
        for (int i = sec_len - 1; i >= 0; i--) {
            time_text[time_text_len++] = seconds_str[i]; //�Ųٷ� �����Ѱ� �ٽ� �����·� �ٽ� ����
        }
    }

    time_text[time_text_len++] = ' ';
    time_text[time_text_len++] = 's';
    time_text[time_text_len++] = 'e';
    time_text[time_text_len++] = 'c';
    time_text[time_text_len] = '\0';

    ALLEGRO_FONT* big_font = al_load_ttf_font("BebasNeue-Regular.ttf", 80, 0);  // ū ��Ʈ
    ALLEGRO_FONT* medium_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);  // �߰� ��Ʈ
    ALLEGRO_FONT* small_font = al_load_ttf_font("BebasNeue-Regular.ttf", 30, 0);  // ���� ��Ʈ

    //���⼭ ���ʹ� Ű���� �̺�Ʈ ��ٸ��� ���� 
    while (!user_input)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && name_len > 0) //���ڸ� ��� �ش� ��ġ�� null���ڸ� �־ ����
            {
                name[--name_len] = '\0';
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) //�����Է½� while�� ����
            {
                user_input = true;
            }
            else if (name_len < 9 && event.keyboard.unichar >= 32 && event.keyboard.unichar <= 126) //���� �� �ִ� Ű���� ����
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

        al_flip_display(); //���ۿ� �ִ°� ȭ�鿡 ��� //������۸� ����ؼ� �ε巴��
       

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