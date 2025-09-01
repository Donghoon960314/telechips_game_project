//======================================================
//                    stage.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      stage.c
 @brief     스테이지 별 몬스터 초기화, 보스 구분 출현
 @author    김혁, 신동훈, 정명훈, 이재강
*/

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include "common.h"

stage_num_for = 0;  //bitmap에 저장된 stage font위한 파라미터용
stage_num = 1; //실제 스테이지 변수
delay = 0; //스테이지 변경 시 딜레이를 위한 초기화

bool monster_all_die = false; //몬스터가 모두 죽었는지 확인하는 변수

IMAGES images;

void stage_init() {
    stage_num = 1;        // 스테이지를 무조건 1부터
    stage_num_for = 0;    // 배너 인덱스도 0부터
    spawn_enabled = true; // 스폰 활성화
    boss_spawned = false; // 보스 안 나온 상태
    boss_spawn_timer = -1;// 보스 타이머 리셋
    delay = 0;            // 전환 카운터 리셋
}

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
        if (enemies[i].used && enemies[i].type == BOSS_TYPE_1) { //추후에 보스 타입 추가 되면 여기 수정해야함
            return true;
        }
    }
    return false;
}

bool check_monster_die(void) // 몬스터 5마리가 전부 죽었는지 체크하는 함수
//2단계 3단계 stage에서 보스몬스터를 출현시키기 위한 로직
{
    int check_num = 0;

    for (int i = 0; i < ENEMIES_N; ++i)
        if (enemies[i].used == false) check_num += 1;
    if (check_num == 5)
    {
        shots_init(); //5명 모두 죽으면 남아있는 총알 초기화 
        if (stage_num == 1) {
            return true;
        } //1단계일때는 stage_reset()에 무조건 true넘겨줘서 보스몹 출현없이 
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

int stage_reset(void)
{
    if (delay > 0) {
        delay--;
        if (delay == 0) {

            boss_spawn_timer = -1;
            boss_spawned = false;

            stage_player_var();   // 여기서 stage_num_for += 1
            stage_font(stage_num_for);  // 현재 stage 배너 출력

            enemies_init();
            shots_init();
            items_init();
            stage_num += 1;
        }
    }
    else if (check_monster_die() == true) { //특정 프레임에서 몬스터가 다 죽으면 바로 delay = 60으로 설정하고 매 프레
        delay = 60;
    }

    return stage_num;
}
