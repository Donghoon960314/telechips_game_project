#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

int main() {
    must_init(al_init(), "allegro"); // Allegro 라이브러리 초기화
    must_init(al_install_keyboard(), "keyboard"); // 키보드 입력 초기화

    // 60fps 타이머 생성
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    // 이벤트 큐 생성
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    disp_init(); // 화면 초기화
    audio_init(); // 오디오 초기화

    must_init(al_init_image_addon(), "image"); // 이미지 관련 추가 모듈 초기화
    sprites_init(); // 스프라이트(캐릭터, 아이템 등) 초기화

    hud_init(); // 점수, 목숨 표시 등 HUD 초기화

    must_init(al_init_primitives_addon(), "primitives"); // 도형 그리기 모듈 초기화

    // 이벤트 큐에 이벤트 소스 등록
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

    bool done = false; // 게임 종료 여부
    bool redraw = true; // 화면 다시 그릴지 여부
    ALLEGRO_EVENT event;

    // 타이머 시작 -> 매 1/60초마다 ALLEGRO_EVENT_TIMER 이벤트 발생
    al_start_timer(timer);

    stage_font(start_stage_image);
    while (1)
    {
        // 이벤트 대기
        // 이벤트가 큐에 차곡차곡 쌓이고, 루프에서 하나씩 처리됨
        // 타이머, 키보드, 창 닫기 이벤트가 섞여도 순서대로 안전하게 처리 가능
        al_wait_for_event(queue, &event);

        // 이벤트 처리
        switch (event.type)
        {

        case ALLEGRO_EVENT_TIMER:  // 타이머 이벤트 -> 게임 업데이트
            //background_update(); // 배경 업데이트
            shots_update();     // 총알 업데이트
            player_update();      // 플레이어 업데이트
            enemies_update();    // 적 업데이트
            
            // 매 프레임마다 시간 감소
            if (frames % 60 == 0 && time_left > 0) {
                time_left--;
            }
            // 시간 지나면 게임 오버 
            if (time_left <= 0) {
                player.hp = 0;
            }

            //stage_reset();

            if (stage_reset() > 3)
            {
                rank_name_open(al_get_timer_count(timer));
                done = true;

            }

            if (frames > 120) {   // 첫 120프레임 동안만 소환 허용
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

                spawn_enabled = true;
                boss_spawned = false;
                boss_spawn_timer = 0;

                shots_init();
                player_init();
                enemies_init();

                // 다시 스테이지 1로 시작
                start_stage_image = 0;
                stage_font(start_stage_image);
            }

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

    sprites_deinit();
    hud_deinit();
    audio_deinit();
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    stage_image_pop_deinit();

    return 0;
}
