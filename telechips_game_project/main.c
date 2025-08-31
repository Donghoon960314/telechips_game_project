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
ALLEGRO_FONT* name_font = NULL;   // 전역 정의
ALLEGRO_FONT* title_font = NULL;  // 전역 정의
ALLEGRO_FONT* button_to_rank_title_font = NULL;


//======================================================
//                  MAIN GAME LOOP
//======================================================
int main() {
    must_init(al_init(), "allegro"); // Allegro 라이브러리 초기화
    must_init(al_install_keyboard(), "keyboard"); // 키보드 입력 초기화
    must_init(al_install_mouse(), "mouse");       // 마우스 입력 초기화

    // 60fps 타이머 생성
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    must_init(al_init_font_addon(), "font");      // 폰트 모듈 초기화
    must_init(al_init_ttf_addon(), "ttf");
    must_init(al_init_image_addon(), "image");    // 이미지 모듈 초기화
    must_init(al_init_primitives_addon(), "primitives"); // 도형 모듈 초기화

    
    
    al_init_font_addon();
    al_init_ttf_addon();


    // 이벤트 큐 생성
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    // UI용 폰트 / 비트맵 로드

    ALLEGRO_FONT* font = al_create_builtin_font();
    button_to_rank_title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 60, 0);
    name_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);
    title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 80, 0);

    printf("DEBUG: name_font=%p, title_font=%p\n", name_font, title_font);

    if (!name_font || !title_font) {
        fprintf(stderr, "폰트 로드 실패! 실행 경로에 BebasNeue-Regular.ttf가 있는지 확인하세요.\n");
        return -1; // 프로그램 바로 종료
    }

    must_init(font, "builtin font");
    ALLEGRO_BITMAP* bitmap = al_load_bitmap("start_display.png");
    must_init(bitmap, "bitmap");

    ALLEGRO_BITMAP* bitmap3 = al_load_bitmap("TUTORIAL.png");
    must_init(bitmap3, "bitmap");

    // 디스플레이 & 오디오 초기화
    disp_init(); // 화면 초기화
    audio_init(); // 오디오 초기화
    sprites_init(); // 스프라이트(캐릭터, 아이템 등) 초기화
    hud_init(); // HUD 초기화


    // 프롤로그 슬라이드 생성
    load_slides();

    // 이벤트 큐에 이벤트 소스 등록
    al_register_event_source(queue, al_get_mouse_event_source());    // 마우스
    al_register_event_source(queue, al_get_keyboard_event_source()); // 키보드
    al_register_event_source(queue, al_get_display_event_source(disp)); // 디스플레이
    al_register_event_source(queue, al_get_timer_event_source(timer));  // 타이머

    // 게임 객체 초기화
    keyboard_init(); // 키보드 상태 초기화
    shots_init();    // 총알 초기화
    enemies_init();  // 적 초기화
    items_init(); // 아이템 초기화
    stage_image_pop_init(); // 스테이지 배너 초기화

    //======================================================
    //                게임 상태 변수 초기화
    //======================================================
    frames = 0;          // 프레임 카운터
    score = 0;           // 점수 초기화
    time_limit = 60;    // 제한 시간
    time_left = time_limit;

    char player_name[32]; // 랭킹용 이름
    int min, sec;         // 랭킹용 시간
    bool game_end_ranking = false; //게임종료후 랭킹출력

    bool done = false;   // 게임 종료 여부
    bool redraw = true;  // 화면 다시 그릴지 여부
    GameState state = STATE_MENU; // 처음에는 UI 메뉴 상태
    ALLEGRO_EVENT event;

    // 타이머 시작
    al_start_timer(timer);
    srand((unsigned int)time(NULL));

    //======================================================
    //                  메인 루프 시작
    //======================================================
    while (1)
    {
        // 이벤트 대기
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
            //======================================================
            //                    UI 관련 처리
            //======================================================
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            /* 디스플레이 좌표 → 버퍼 좌표로 변환 후 hover 갱신 */
            float bx = to_buffer_x(event.mouse.x);
            float by = to_buffer_y(event.mouse.y);
            update_hover_all(bx, by);
            redraw = true;
        }
        break;

        // 마우스 클릭 관련 이벤트 발생 시 
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event.mouse.button == 1) {
                float bx = to_buffer_x(event.mouse.x);
                float by = to_buffer_y(event.mouse.y);

                // 마우스 위치가 버튼 위치
                Button* b = hit_button(bx, by);
                if (!b)  break;

                // 게임 시작
                if (b == &pos1)
                {
                    show_back_only();
                    state = STATE_CHOICE;
                    pos8.active = true;
                    pos9.active = true;
                }

                // 뒤로 가기
                else if (b == &pos2)
                {
                    show_main_menu();
                    state = STATE_MENU;
                }

                // 랭킹,게임 설명
                else if (b == &pos3)
                {
                    show_back_only();
                    state = STATE_TUTORIAL;
                }
                else if (b == &pos4)
                {
                    state = STATE_RANKING;
                }

                // 난이도: 쉬운 모드
                else if (b == &pos5)
                {
                    set_pro_job();
                    game_difficulty = DIFF_EASY;
                    state = STATE_PROLOGUE;

                }

                // 난이도: 중간 모드
                else if (b == &pos6)
                {
                    set_pro_job();
                    game_difficulty = DIFF_NORMAL;
                    state = STATE_PROLOGUE;
                }

                // 난이도: 어려운 모드
                else if (b == &pos7)
                {
                    set_pro_job();
                    game_difficulty = DIFF_HARD;
                    state = STATE_PROLOGUE;
                }

                // 직업 선택 1: Tanjiro
                else if (b == &pos8)
                {
                    state = STATE_MODE;
                    job_type = JOB_TYPE_1;
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;
                }

                // 직업 선택 2: Rengoku
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
                // 랭킹 화면일 때는 어떤 키를 눌러도 메뉴로 복귀
                state = STATE_MENU;
                game_end_ranking = false;   // 게임 끝나서 들어온 거니까 하이라이트 O
                stage_num = 1; //게임시작 -> 종료 -> 랭킹입력 -> esc -> 다시 게임시작 할때 필요함
                show_main_menu();   // 메뉴 버튼 다시 활성화
                redraw = true;
            }
            else if (state == STATE_PROLOGUE && event.keyboard.keycode == ALLEGRO_KEY_S) {
                next_slide(); // 프롤로그 넘기기
                redraw = true;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                done = true;
            }
            break;

            // 알레그로 타이머 이벤트 
        case ALLEGRO_EVENT_TIMER:
            if (state == STATE_RUNNING) {
                // 게임 로직 업데이트 (스테이지 안일 때만)
                shots_update();
                player_update();
                enemies_update();
                items_update();
                hud_update();
                update_shake();

                // 매 프레임마다 시간 감소
                if (frames % 60 == 0 && time_left > 0)
                    time_left--;

                // 시간 지나면 게임 오버
                if (time_left <= 0)
                    player.hp = 0;

                // 스테이지 클리어 체크
                if (stage_reset() > 3) {
                    rank_name_open(al_get_timer_count(timer), player_name, &min, &sec);
                    game_end_ranking = true;   // 게임 끝나서 들어온 거니까 하이라이트 O
                    //print_ranking_table(player_name, min, sec);
                    
                    al_flush_event_queue(queue);

                    //상태 변화후 ranking
                    state = STATE_RANKING;

                    disp_pre_draw();
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    
                    disp_post_draw();

                    redraw = false;

                    continue;
                }

                // 첫 120프레임 동안만 소환 허용
                if (frames > 120) {
                    spawn_enabled = false;
                }

                // ESC 키 -> 게임 종료
                if (key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                // R 키 -> 게임 리스타트 (플레이어 죽었을 때만)
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

                    // 다시 스테이지 1로 시작
                }
            }

            frames++;
            redraw = true;
            break;

            // 창 닫기 이벤트
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        // 종료 조건 확인
        if (done)
            break;


        // 키보드 입력 업데이트
        keyboard_update(&event);

        // 화면 그리기
        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw(); // 더블 버퍼 준비
            al_clear_to_color(al_map_rgb(0, 0, 0)); // 화면 클리어 (검정색)

            // UI 상태에 따른 화면 표시
            switch (state)
            {
            case STATE_MENU:
                prologue_display(bitmap);
                Button_draw(&pos1, font);
                Button_draw(&pos2, font);
                Button_draw(&pos3, font);
                Button_draw(&pos4, font);
                break;

                /*난이도 선택 화면*/
            case STATE_MODE:
                prologue_display(bitmap);
                Button_draw(&pos5, font);
                Button_draw(&pos6, font);
                Button_draw(&pos7, font);
                break;

                // 직업 선택 화면
            case STATE_CHOICE:
                prologue_display(bitmap);
                Button_draw(&pos8, font); //danso
                Button_draw(&pos9, font); //zaruban
                break;

                // 프롤로그 진입
            case STATE_PROLOGUE:
                if (!ps.blink)
                {
                    if (ps.curr <= ps.end)
                    {
                        ALLEGRO_BITMAP* bitmap2 = prologue_List[ps.curr];
                        prologue_display(bitmap2); // 프롤로그 슬라이드 표시
                    }
                }
                else
                {
                    state = STATE_RUNNING;  // 프롤로그 끝 → 게임 시작
                    stage_font(0);          // Stage1 배너 띄움

                    stage_init();
                    player_init();   // 플레이어 초기화
                }
                break;

                // 프롤로그가 끝났을 때 -> 본격적인 게임 화면
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

            case STATE_RANKING:
                if (game_end_ranking)
                {
                    print_ranking_table(player_name, min, sec, END_TO_RANK);
                }
                else
                {
                    print_ranking_table("", 0, 0, BUTTON_TO_RANK);
                }
                break;
            
            case STATE_TUTORIAL:
                prologue_display(bitmap);
                Tutorial_display(bitmap3);
                Button_draw(&pos2, font);
                break;
            }
            disp_post_draw(); // 더블 버퍼 화면에 출력
            redraw = false;
        }
    }

    /* 함수 마무리 선언(타이머, 폰트, 디스플레이, 이벤트 큐)*/
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
    