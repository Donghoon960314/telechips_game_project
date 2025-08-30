#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
//#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

int stage_num_for = 1;
int Monster_num = 2;
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

    images.stage[0] = al_load_bitmap("stage2_new_new.PNG");
    images.stage[1] = al_load_bitmap("stage1_new_new.PNG");
    images.stage[2] = al_load_bitmap("stage3_new_new.PNG");

}
void stage_image_pop_deinit() //나중에 저장된 비트맵을 없애줌
{
    al_destroy_bitmap(images.stage[0]);
    al_destroy_bitmap(images.stage[1]);
    al_destroy_bitmap(images.stage[2]);
}

bool check_monster_die(void) // 몬스터 5마리가 전부 죽었는지 체크하는 함수
{
    int check_num = 0;

    for (int i = 0; i < ENEMIES_N; ++i)
        if (enemies[i].used == false) check_num += 1;
    if (check_num == 5)
    {
        return true;
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

int stage_player_var(void) //스테이지가 바뀔때마다 변화하는 변수들 모음
{
    player.x = 10;
    player.y = 10;
    frames = 0;
    stage_num_for += 1;
    spawn_enabled = true;
}


int stage_reset(void)
{
    if (check_monster_die() == true)
    {
        printf("monster all die\n");
        stage_font(stage_num_for);
        stage_player_var();
        printf("start stage %d\n", stage_num_for);
        enemies_init();
    }
    return stage_num_for;
}