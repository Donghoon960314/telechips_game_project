#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
//#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

int stage_num_for = 0;  
int stage_num = 1;
int delay = 0;

bool monster_all_die = false;

typedef struct IMAGES
{
    ALLEGRO_BITMAP* stage[3];

}IMAGES;
IMAGES images;

void stage_image_pop_init() //main함수 반복문 돌기전에 bitamp에 저장두는 함수
{
    //stage_font(0);
    //stage_num_for++;

    images.stage[0] = al_load_bitmap("stage1.PNG");
    images.stage[1] = al_load_bitmap("stage2.PNG");
    images.stage[2] = al_load_bitmap("stage3.PNG");

}
void stage_image_pop_deinit() //나중에 저장된 비트맵을 없애줌
{
    al_destroy_bitmap(images.stage[0]);
    al_destroy_bitmap(images.stage[1]);
    al_destroy_bitmap(images.stage[2]);
}

//보스가 죽었는지 체크하는 함수
bool boss_check_live(void)
{
    for (int i = 0; i < ENEMIES_N; i++) {
        if (enemies[i].used && enemies[i].type == BOSS_TYPE_1) {
            return true;
        }
    }
    return false;
}

bool check_monster_die(void) // 몬스터 5마리가 전부 죽었는지 체크하는 함수
//2단계 3단계 stage에만 보스몬스터를 출현시키기 위한 로직
//stage 1에서는 true를 return하고 stage2,3단계에 몬스터와 보스가 모두 죽었는지 체크한다.
{
    int check_num = 0;

    for (int i = 0; i < ENEMIES_N; ++i)
        if (enemies[i].used == false) check_num += 1;
    if (check_num == 5)
    {
        if (stage_num == 1)
            return true;
        else {
            return (boss_spawned && !boss_check_live());
        }
    }
    return false;
}

void stage_font(int stage_num_for) //각 stage가 시작되기전에 stage 폰트 띄워주는 함수
{

    if (stage_num_for < 3)
    {
        ALLEGRO_BITMAP* bmp = images.stage[stage_num_for];
        int w = al_get_bitmap_width(bmp);
        int h = al_get_bitmap_height(bmp);

        float scale = 0.5f;
        int new_w = w * scale;
        int new_h = h * scale;

        disp_pre_draw();
        al_clear_to_color(al_map_rgb(0, 0, 0));

        int dx = (BUFFER_W - new_w) / 2;
        int dy = (BUFFER_H - new_h) / 2;

        al_draw_scaled_bitmap(bmp,0, 0, w, h, dx, dy,new_w, new_h,0);
        disp_post_draw();

        al_rest(1.0);
    }
}

void stage_player_var(void) //스테이지가 바뀔때마다 변화하는 변수들 모음
{
    player.x = 10;
    player.y = 10;
    frames = 0;
    stage_num_for += 1;
    spawn_enabled = true;
}

/*
int stage_reset(void)
{
    
    if (restarted) {
        restarted = false;   // 한 번만 적용
        delay = 0;
        boss_spawn_timer = -1;
        boss_spawned = false;

        // stage_num / stage_num_for를 초기화한 상태이므로
        printf("Restart detected, back to Stage %d\n", stage_num);

        return stage_num_for;
    }
    

    if (delay > 0) {       // 몬스터의 HP가 0이되면 몬스터가 아직 그려진채로 빠르게 다음 스테이지로 넘어가는부분을
                           // 자연스럽게 넘어가기 위한 코드임.
        delay--;
        if (delay == 0) {
            stage_font(stage_num_for);

            // 전역변수로 delay를 설정하고 해당 딜레이 동안 main문의 while 루프를 계속 돌면서 몬스터가 없어지고
            //다음 스테이지로 넘아가기 위한 잠깐의 대기시간을 주기위한 코드
            //delay가 0으로 세팅되면 다음 stage로 넘어감
            boss_spawn_timer = -1;
            boss_spawned = false;

            printf("monster all die\n");
            //stage_font(stage_num_for);
            stage_player_var();

            printf("start stage %d\n", stage_num_for);

            enemies_init();
            stage_num += 1;
        }
    }
    else if (check_monster_die() == true) { 
        delay = 60;
        //몬스터가 모두 죽으면 다음 stage로 넘어가기 위해 delay = 60으로 세팅
    }

    return stage_num_for;
}
*/
int stage_reset(void)
{
    if (delay > 0) {
        delay--;
        if (delay == 0) {

            boss_spawn_timer = -1;
            boss_spawned = false;

            printf("monster all die\n");
            stage_player_var();   // 여기서 stage_num_for += 1
            stage_font(stage_num_for);  // 현재 stage 배너 출력

            printf("start stage %d\n", stage_num_for);

            enemies_init();
            stage_num += 1;
        }
    }
    else if (check_monster_die() == true) {
        delay = 60;
    }
    printf("stage:num : %d\n", stage_num);
    printf("stage_num_for : %d\n", stage_num_for);
    return stage_num;
}
