#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "common.h"

bool restarted = false;
ALLEGRO_FONT* name_font = NULL;   // ?�역 ?�의
ALLEGRO_FONT* title_font = NULL;  // ?�역 ?�의
ALLEGRO_FONT* button_to_rank_title_font = NULL;


//======================================================
//                  MAIN GAME LOOP
//======================================================
int main() {
    must_init(al_init(), "allegro"); // Allegro ?�이브러�?초기??
    must_init(al_install_keyboard(), "keyboard"); // ?�보???�력 초기??
    must_init(al_install_mouse(), "mouse");       // 마우???�력 초기??

    // 60fps ?�?�머 ?�성
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    must_init(al_init_font_addon(), "font");      // ?�트 모듈 초기??
    must_init(al_init_ttf_addon(), "ttf");
    must_init(al_init_image_addon(), "image");    // ?��?지 모듈 초기??
    must_init(al_init_primitives_addon(), "primitives"); // ?�형 모듈 초기??

    
    
    al_init_font_addon();
    al_init_ttf_addon();


    // ?�벤?????�성
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    // UI???�트 / 비트�?로드

    //ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_FONT* font = al_load_ttf_font("OpenSans_Bold.ttf",40,0);
    button_to_rank_title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 60, 0);
    name_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);
    title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 80, 0);

    printf("DEBUG: name_font=%p, title_font=%p\n", name_font, title_font);

    if (!name_font || !title_font) {
        fprintf(stderr, "?�트 로드 ?�패! ?�행 경로??BebasNeue-Regular.ttf가 ?�는지 ?�인?�세??\n");
        return -1; // ?�로그램 바로 종료
    }

    must_init(font, "builtin font");
    ALLEGRO_BITMAP* bitmap = al_load_bitmap("start_display.png");
    must_init(bitmap, "bitmap");

    ALLEGRO_BITMAP* bitmap3 = al_load_bitmap("TUTORIAL.png");
    must_init(bitmap3, "bitmap");

    // ?�스?�레??& ?�디??초기??
    disp_init(); // ?�면 초기??
    audio_init(); // ?�디??초기??
    sprites_init(); // ?�프?�이??캐릭?? ?�이???? 초기??
    hud_init(); // HUD 초기??


    // ?�롤로그 ?�라?�드 ?�성
    load_slides();

    // ?�벤???�에 ?�벤???�스 ?�록
    al_register_event_source(queue, al_get_mouse_event_source());    // 마우??
    al_register_event_source(queue, al_get_keyboard_event_source()); // ?�보??
    al_register_event_source(queue, al_get_display_event_source(disp)); // ?�스?�레??
    al_register_event_source(queue, al_get_timer_event_source(timer));  // ?�?�머

    // 게임 객체 초기??
    keyboard_init(); // ?�보???�태 초기??
    shots_init();    // 총알 초기??
    enemies_init();  // ??초기??
    items_init(); // ?�이??초기??
    stage_image_pop_init(); // ?�테?��? 배너 초기??

    //======================================================
    //                게임 ?�태 변??초기??
    //======================================================
    frames = 0;          // ?�레??카운??
    score = 0;           // ?�수 초기??
    time_limit = 60;    // ?�한 ?�간
    time_left = time_limit;

    char player_name[32]; // ���������� �÷��̾� �̸�
    int min, sec;         // ��,��(���ӱ��)
    bool game_end_ranking = false; //��ŷâ���� �����ϴ°��� ���� ������ ����/ ��ʸ� Ŭ���ؼ� ���� Ȯ���� �� �ְԲ�.

    bool done = false;   // 게임 종료 ?��?
    bool redraw = true;  // ?�면 ?�시 그릴지 ?��?
    GameState state = STATE_MENU; // 처음?�는 UI 메뉴 ?�태
    ALLEGRO_EVENT event;

    // ?�?�머 ?�작
    al_start_timer(timer);
    srand((unsigned int)time(NULL));

    //======================================================
    //                  메인 루프 ?�작
    //======================================================
    while (1)
    {
        // ?�벤???��?
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
            //======================================================
            //                    UI 관??처리
            //======================================================
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            /* ?�스?�레??좌표 ??버퍼 좌표�?변????hover 갱신 */
            float bx = to_buffer_x(event.mouse.x);
            float by = to_buffer_y(event.mouse.y);
            update_hover_all(bx, by);
            redraw = true;
        }
        break;

        // 마우???�릭 관???�벤??발생 ??
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event.mouse.button == 1) {
                float bx = to_buffer_x(event.mouse.x);
                float by = to_buffer_y(event.mouse.y);

                // 마우???�치가 버튼 ?�치
                Button* b = hit_button(bx, by);
                if (!b)  break;

                // 게임 ?�작
                if (b == &pos1)
                {
                    show_back_only();
                    state = STATE_CHOICE;
                    pos8.active = true;
                    pos9.active = true;
                }

                // ?�로 가�?
                else if (b == &pos2)
                {
                    show_main_menu();
                    state = STATE_MENU;
                }

                // ??��,게임 ?�명
                else if (b == &pos3)
                {
                    show_back_only();
                    state = STATE_TUTORIAL;
                }
                else if (b == &pos4)
                {
                    state = STATE_RANKING;
                }

                // ?�이?? ?�운 모드
                else if (b == &pos5)
                {
                    set_pro_job();
                    game_difficulty = DIFF_EASY;
                    state = STATE_PROLOGUE;

                }

                // ?�이?? 중간 모드
                else if (b == &pos6)
                {
                    set_pro_job();
                    game_difficulty = DIFF_NORMAL;
                    state = STATE_PROLOGUE;
                }

                // ?�이?? ?�려??모드
                else if (b == &pos7)
                {
                    set_pro_job();
                    game_difficulty = DIFF_HARD;
                    state = STATE_PROLOGUE;
                }

                // 직업 ?�택 1: Tanjiro
                else if (b == &pos8)
                {
                    state = STATE_MODE;
                    job_type = JOB_TYPE_1;
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;
                }

                // 직업 ?�택 2: Rengoku
                else if (b == &pos9)
                {
                    state = STATE_MODE;
                    job_type = JOB_TYPE_2;
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;
                }
                redraw = true;
            }
        } break;

        case ALLEGRO_EVENT_KEY_DOWN:
            if (state == STATE_RANKING)
            {
                // ??�� ?�면???�는 ?�떤 ?��? ?�러??메뉴�?복�?
                state = STATE_MENU;
                game_end_ranking = false;   // 게임 ?�나???�어??거니�??�이?�이??O
                stage_num = 1; //게임?�작 -> 종료 -> ??��?�력 -> esc -> ?�시 게임?�작 ?�때 ?�요??
                show_main_menu();   // 메뉴 버튼 ?�시 ?�성??
                redraw = true;
            }
            else if (state == STATE_PROLOGUE && event.keyboard.keycode == ALLEGRO_KEY_S) {
                next_slide(); // ?�롤로그 ?�기�?
                redraw = true;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                done = true;
            }
            break;

            // ?�레그로 ?�?�머 ?�벤??
        case ALLEGRO_EVENT_TIMER:
            if (state == STATE_RUNNING) {
                // 게임 로직 ?�데?�트 (?�테?��? ?�일 ?�만)
                shots_update();
                player_update();
                enemies_update();
                items_update();
                hud_update();
                update_shake();

                // �??�레?�마???�간 감소
                if (frames % 60 == 0 && time_left > 0)
                    time_left--;

                // ?�간 지?�면 게임 ?�버
                if (time_left <= 0)
                    player.hp = 0;

                // �� �����Ӹ��� stage_reset()�� ��� ȣ���ϸ鼭 �������� ���� Ȯ��
                if (stage_reset() > 3) { //stage_reset�� return �� == stage_num
                    rank_name_open(al_get_timer_count(timer), player_name, &min, &sec); //�������� ������ ��ŷ â���� �̸� �Է�
                    game_end_ranking = true;   //game_end_ranking������ true�� �ٲٲ�  RANKING_STATE�� �Ѿ������ RANKȭ���� ���.
                    
                    al_flush_event_queue(queue); //��� �̺�Ʈ�� ���� ���� �������� �غ�

                    state = STATE_RANKING; //RANKIHG STATE ����

                    disp_pre_draw();
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    
                    disp_post_draw();

                    redraw = false;

                    continue;
                }

                // �?120?�레???�안�??�환 ?�용
                if (frames > 120) {
                    spawn_enabled = false;
                }

                // ESC ??-> 게임 종료
                if (key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                // R ??-> 게임 리스?�??(?�레?�어 죽었???�만)
                if (player.hp <= 0 && key[ALLEGRO_KEY_R]) {
                    frames = 0;
                    score = 0;
                    time_left = time_limit;

                    stage_init();

                    al_stop_timer(timer);
                    al_set_timer_count(timer, 0);
                    al_start_timer(timer);

                    shots_init();
                    player_init();
                    enemies_init();
                    items_init();

                    stage_font(stage_num_for);

                    // ?�시 ?�테?��? 1�??�작
                }
            }

            frames++;
            redraw = true;
            break;

            // �??�기 ?�벤??
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        // 종료 조건 ?�인
        if (done)
            break;


        // ?�보???�력 ?�데?�트
        keyboard_update(&event);

        // ?�면 그리�?
        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw(); // ?�블 버퍼 준�?
            al_clear_to_color(al_map_rgb(0, 0, 0)); // ?�면 ?�리??(검?�색)

            // UI ?�태???�른 ?�면 ?�시
            switch (state)
            {
            case STATE_MENU:
                prologue_display(bitmap);
                Button_draw(&pos1, font);
                Button_draw(&pos2, font);
                Button_draw(&pos3, font);
                Button_draw(&pos4, font);
                break;

                /*?�이???�택 ?�면*/
            case STATE_MODE:
                prologue_display(bitmap);
                Button_draw(&pos5, font);
                Button_draw(&pos6, font);
                Button_draw(&pos7, font);
                break;

                // 직업 ?�택 ?�면
            case STATE_CHOICE:
                prologue_display(bitmap);
                Button_draw(&pos8, font); //danso
                Button_draw(&pos9, font); //zaruban
                break;

                // ?�롤로그 진입
            case STATE_PROLOGUE:
                if (!ps.blink)
                {
                    if (ps.curr <= ps.end)
                    {
                        ALLEGRO_BITMAP* bitmap2 = prologue_List[ps.curr];
                        prologue_display(bitmap2); // ?�롤로그 ?�라?�드 ?�시
                    }
                }
                else
                {
                    state = STATE_RUNNING;  // ?�롤로그 ????게임 ?�작
                    frames = 0; //8.31 ?�정
                    time_left = time_limit;     //8.31 ?�정
                    keyboard_init(); //지맘�?�??�직이?�거 초기??8.31
                    stage_font(0);          // Stage1 배너 ?��?
                    
                    al_stop_timer(timer); //게임 종료???�시???�간 ?�류 ?�정//
                    al_set_timer_count(timer, 0);
                    al_start_timer(timer);

                    stage_init();
                    player_init();   // ?�레?�어 초기??
                }
                break;

                // ?�롤로그가 ?�났????-> 본격?�인 게임 ?�면
            case STATE_RUNNING:
                draw_floor();
                draw_vertical_lines();
                draw_horizon_lines();
                draw_background();
                draw_subway_background();
                items_draw();
                enemies_draw();
                shots_draw();
                player_draw();
                hud_draw();
                break;

            case STATE_RANKING: //��ŷ ȭ�� ���Խ� ��ŷ����ϴ� �Լ� ȣ��(���� �Լ�, ���ڸ� �ٸ���)
                if (game_end_ranking)
                {
                    print_ranking_table(player_name, min, sec, END_TO_RANK); //������ ������ ��ŷȭ�� ���Խ� 
                }
                else
                {
                    print_ranking_table("", 0, 0, BUTTON_TO_RANK); //��ư������ ��ŷȭ�� ���� 
                }
                break;
            
            case STATE_TUTORIAL:
                prologue_display(bitmap);
                Tutorial_display(bitmap3);
                Button_draw(&pos2, font);
                break;
            }
            disp_post_draw(); // ?�블 버퍼 ?�면??출력
            redraw = false;
        }
    }

    /* ?�수 마무�??�언(?�?�머, ?�트, ?�스?�레?? ?�벤????*/
    /*al_destroy_timer(timer);
    al_destroy_font(font);
    disp_deinit();
    al_destroy_bitmap(bitmap);
    al_destroy_event_queue(queue);*/

    sprites_deinit();
    hud_deinit();
    audio_deinit();
    disp_deinit();
    al_destroy_font(font);
    al_destroy_bitmap(bitmap);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    stage_image_pop_deinit();
    
    return 0;
        
}
    