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

void stage_image_pop_init() //main�Լ� �ݺ��� �������� bitamp�� ����δ� �Լ�
{
    //stage_font(0);
    //stage_num_for++;

    images.stage[0] = al_load_bitmap("stage1.PNG");
    images.stage[1] = al_load_bitmap("stage2.PNG");
    images.stage[2] = al_load_bitmap("stage3.PNG");

}
void stage_image_pop_deinit() //���߿� ����� ��Ʈ���� ������
{
    al_destroy_bitmap(images.stage[0]);
    al_destroy_bitmap(images.stage[1]);
    al_destroy_bitmap(images.stage[2]);
}

//������ �׾����� üũ�ϴ� �Լ�
bool boss_check_live(void)
{
    for (int i = 0; i < ENEMIES_N; i++) {
        if (enemies[i].used && enemies[i].type == BOSS_TYPE_1) {
            return true;
        }
    }
    return false;
}

bool check_monster_die(void) // ���� 5������ ���� �׾����� üũ�ϴ� �Լ�
//2�ܰ� 3�ܰ� stage���� �������͸� ������Ű�� ���� ����
//stage 1������ true�� return�ϰ� stage2,3�ܰ迡 ���Ϳ� ������ ��� �׾����� üũ�Ѵ�.
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

void stage_font(int stage_num_for) //�� stage�� ���۵Ǳ����� stage ��Ʈ ����ִ� �Լ�
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

void stage_player_var(void) //���������� �ٲ𶧸��� ��ȭ�ϴ� ������ ����
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
        restarted = false;   // �� ���� ����
        delay = 0;
        boss_spawn_timer = -1;
        boss_spawned = false;

        // stage_num / stage_num_for�� �ʱ�ȭ�� �����̹Ƿ�
        printf("Restart detected, back to Stage %d\n", stage_num);

        return stage_num_for;
    }
    

    if (delay > 0) {       // ������ HP�� 0�̵Ǹ� ���Ͱ� ���� �׷���ä�� ������ ���� ���������� �Ѿ�ºκ���
                           // �ڿ������� �Ѿ�� ���� �ڵ���.
        delay--;
        if (delay == 0) {
            stage_font(stage_num_for);

            // ���������� delay�� �����ϰ� �ش� ������ ���� main���� while ������ ��� ���鼭 ���Ͱ� ��������
            //���� ���������� �Ѿư��� ���� ����� ���ð��� �ֱ����� �ڵ�
            //delay�� 0���� ���õǸ� ���� stage�� �Ѿ
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
        //���Ͱ� ��� ������ ���� stage�� �Ѿ�� ���� delay = 60���� ����
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
            stage_player_var();   // ���⼭ stage_num_for += 1
            stage_font(stage_num_for);  // ���� stage ��� ���

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
