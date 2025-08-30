#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
bool restarted = false;


int main() {
    must_init(al_init(), "allegro"); // Allegro 라이브러리 초기화
    must_init(al_install_keyboard(), "keyboard"); // 키보드 입력 초기화
    must_init(al_install_mouse(), "mouse");
    // 60fps 타이머 생성
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");
    must_init(al_init_font_addon(), "font");
    // 이벤트 큐 생성
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");
    //ui
    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "builtin font");

    ALLEGRO_BITMAP* bitmap = al_load_bitmap("start_display.png");
    must_init(bitmap, "bitmap");

    disp_init(); // 화면 초기화
    audio_init(); // 오디오 초기화

    must_init(al_init_image_addon(), "image"); // 이미지 관련 추가 모듈 초기화
    sprites_init(); // 스프라이트(캐릭터, 아이템 등) 초기화

    hud_init(); // 점수, 목숨 표시 등 HUD 초기화
    // 프롤로그 슬라이드 생성하기
    load_slides();
    must_init(al_init_primitives_addon(), "primitives"); // 도형 그리기 모듈 초기화

    // 이벤트 큐에 이벤트 소스 등록
    al_register_event_source(queue, al_get_mouse_event_source()); //마우스
    al_register_event_source(queue, al_get_keyboard_event_source()); // 키보드
    al_register_event_source(queue, al_get_display_event_source(disp)); // 디스플레이
    al_register_event_source(queue, al_get_timer_event_source(timer)); // 타이머

    // 게임 객체 초기화
    keyboard_init(); // 키보드 상태 초기화
    shots_init(); // 총알 초기화
    player_init(); // 플레이어 초기화
    enemies_init(); // 적 초기화
    stage_image_pop_init();

    // 게임 상태 초기화
    frames = 0; // 프레임 카운터
    score = 0; // 점수 초기화
    int start_stage_image = 0;
    time_limit = 120; // 초기 시간
    time_left = time_limit; // 남은 시간

    //랭킹 입력 하이라이트 부분에서 참조할 변수
    char player_name[32];
    int min, sec;

    //bool running = true;
    bool done = false; // 게임 종료 여부
    bool redraw = true; // 화면 다시 그릴지 여부
    GameState state = STATE_MENU;
    ALLEGRO_EVENT event;

    // 타이머 시작 -> 매 1/60초마다 ALLEGRO_EVENT_TIMER 이벤트 발생
    al_start_timer(timer);

    stage_font(0);
    while (1)
    {
        // 이벤트 대기
        // 이벤트가 큐에 차곡차곡 쌓이고, 루프에서 하나씩 처리됨
        // 타이머, 키보드, 창 닫기 이벤트가 섞여도 순서대로 안전하게 처리 가능
        al_wait_for_event(queue, &event);

        // 이벤트 처리
        switch (event.type)
        {
            //명훈 ui
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
            if (event.mouse.button == 1) {
                float bx = to_buffer_x(event.mouse.x);
                float by = to_buffer_y(event.mouse.y);
                // 마우스 위치가 버튼 위치

                Button* b = hit_button(bx, by);
                if (!b)  break;

                // 게임 시작 버튼
                if (b == &pos1)
                {
                    show_back_only();
                    state = STATE_CHOICE;
                    pos8.active = true;
                    pos9.active = true;
                }

                // 뒤로 가기 선언
                else if (b == &pos2)
                {
                    show_main_menu();
                }

                // 랭킹 , 게임 설명 
                else if ((b == &pos3) || (b == &pos4))
                {
                    show_back_only();
                }

                // 난이도 : 쉬운 모드
                else if (b == &pos5)
                {
                    set_pro_job();

                    game_difficulty = DIFF_HARD;
                    printf("Very Easy~~~~~~~~~~~~ : %d\n", DIFF_EASY);    // 테스트
                    state = STATE_RUNNING;

                }
                // 난이도 : 중간 모드

                else if (b == &pos6)
                {
                    set_pro_job();

                    game_difficulty = DIFF_NORMAL;
                    printf("Normal~~~~~~~~~~~~ : %d\n", DIFF_NORMAL);    //
                    state = STATE_RUNNING;

                }
                // 난이도 : 어려운 모드
                else if (b == &pos7)
                {
                    set_pro_job();
                    game_difficulty = DIFF_HARD;
                    printf("Hard~~~~~~~~~~~~ : %d\n", DIFF_HARD);      // 테스트    
                    state = STATE_RUNNING;


                }
                // 직업 선택 1 : DANSO
                else if (b == &pos8)
                {
                    state = STATE_MODE;     // 모드 선택
                    job = JOB_DANSO;        // 
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;


                }

                // 직업 선택 2 : ZARUBAN
                else if (b == &pos9)
                {
                    state = STATE_MODE;
                    job = JOB_ZARUBAN;
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;
                }

                redraw = true;
            }
            break;
            //여기까지

            // 알레그로 타이머 이벤트 
        case ALLEGRO_EVENT_TIMER:
            //background_update(); // 배경 업데이트
            shots_update();     // 총알 업데이트
            player_update();      // 플레이어 업데이트
            enemies_update();    // 적 업데이트
            
            // 매 프레임마다 시간 감소
            if (frames % 60 == 0 && time_left > 0)
            {
                time_left--;
            }
            // 시간 지나면 게임 오버 
            if (time_left <= 0)
            {
                player.hp = 0;
            }

            //stage_reset();

            if (stage_reset() > 3)
            {
                //rank_name_open(al_get_timer_count(timer));
                //print_ranking_table(rank_name_open(al_get_timer_count(timer)));
                rank_name_open(al_get_timer_count(timer), player_name, &min, &sec);
                print_ranking_table(player_name, min, sec);
                done = true;
                    
            }

            if (frames > 120) {   // 첫 120프레임 동안만 소환 허용
              spawn_enabled = false;

            }

            // ESC 키 -> 게임 종료
            if (key[ALLEGRO_KEY_ESCAPE])
                done = true;

            // S키 버튼을 누를 시  명훈 ui
            else if (event.keyboard.keycode == ALLEGRO_KEY_S && state == STATE_RUNNING)
            {
                next_slide();
                redraw = true;
            }
            break;
            //여기까지

            // R 키 -> 게임 리스타트 (플레이어 죽었을 때만)
            if (player.hp <= 0 && key[ALLEGRO_KEY_R]) {
                frames = 0;
                score = 0;
                time_left = time_limit;

                spawn_enabled = true;
                boss_spawned = false;
                boss_spawn_timer = 0;

                boss_spawn_timer = -1;   // 대기 타이머도 리셋
                delay = 0;    // 스테이지 전환 대기 카운터 리셋

                stage_num = 1;    // 논리 스테이지 1로
                stage_num_for = 0;    // 배너/이미지 인덱스도 0(STAGE1)로

                al_stop_timer(timer);               
                al_set_timer_count(timer, 0);       
                al_start_timer(timer);              

                shots_init();
                player_init();
                enemies_init();

                stage_font(stage_num_for);

                // 다시 스테이지 1로 시작
            }
            //여기까지

            redraw = true; // 화면 다시 그리기 플래그
            frames++;      // 프레임 증가;
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
            
            //명훈 ui
            prologue_display(bitmap);

            switch (state) {

            case STATE_MENU:
                Button_draw(&pos1, font);
                Button_draw(&pos2, font);
                Button_draw(&pos3, font);
                Button_draw(&pos4, font);

                break;

                /*난이도 선택 화면*/
            case STATE_MODE:
                Button_draw(&pos5, font);
                Button_draw(&pos6, font);
                Button_draw(&pos7, font);

                break;

                // 직업 선택 화면
            case STATE_CHOICE:
                Button_draw(&pos8, font);   //danso
                Button_draw(&pos9, font);   //zaruban

                break;

                // 프롤로그 진입
            case STATE_RUNNING:
                // 빈 화면
            {
                if (ps.blink)
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                // 프롤로그 화면을 순차적으로 생성 (직업 순으로 나눔)
                else
                {
                    ALLEGRO_BITMAP* bitmap2 = prologue_List[ps.curr];
                    prologue_display(bitmap2);
                }
                break;
            }
            default:
                break;
            }
            //여기까지

            draw_floor(); // 바닥
            draw_vertical_lines(); // 세로선
            draw_horizon_lines(); // 가로선
            draw_background(); // 배경

            enemies_draw(); // 적
            shots_draw();   // 총알
            player_draw(); // 플레이어
            hud_draw(); // HUD
            
            disp_post_draw(); // 더블 버퍼 화면에 출력
            redraw = false;
        }
    }

    /* 함수 마무리 선언(타이머, 폰트, 디스플레이, 이벤트 큐)*/
    al_destroy_timer(timer);
    al_destroy_font(font);
    
    al_destroy_bitmap(bitmap);
    al_destroy_event_queue(queue);

    sprites_deinit();
    hud_deinit();
    audio_deinit();
    disp_deinit();
    
    
    stage_image_pop_deinit();

    return 0;
}
