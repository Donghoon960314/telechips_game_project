/*
- 게임 시작 화면 버튼 3개
- 난이도에 따른 버튼 3개 구현 +
- 배경 화면 넣기
-
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
//#include "common.h"

#if 01

/* --- general --- */

long frames;
long score;

// 알레그로 초기화 함수 작업 -> 해당 작업 변수가 없을 시 경고문
static void must_init(bool test, const char* description)
{
    if (test) return;
    printf("couldn't initialize %s\n", description);
    exit(1);
}

static int between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

static float between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}

static bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if (ax1 > bx2) return false;
    if (ax2 < bx1) return false;
    if (ay1 > by2) return false;
    if (ay2 < by1) return false;
    return true;
}

/* --- display --- */

#define BUFFER_W 1600
#define BUFFER_H 900

#define DISP_SCALE 1
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

// 디스플레이 화면 초기화 함수
void disp_init(void)
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
}

void disp_deinit(void)
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
}

void disp_pre_draw(void)
{
    al_set_target_bitmap(buffer);
}


void disp_post_draw(void)
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H,
        0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
}

/*---  --- */

/* --- Button --- */
typedef struct {
    float x;    // 왼쪽 X
    float y;    // 위쪽 Y
    float w;    // 폭
    float h;    // 높이

    char label1[20];
    char label2[20]; // (unused)
    bool hover;
    bool active;    // 메인 화면 활성화 여부
} Button;

/* 버튼 4개 초기화
---  init  ---
pos1 : START
pos2 : BACK
pos3 : GUIDE
pos4 : RANKING
--- mode ---
pos5 : easy
pos6 : Normal
pos7 : Hard

--- job ---
pos8 : Danso
pos9 : Zaruban
*/

Button pos1 = { BUFFER_W / 2, (BUFFER_H / 2) - 60 , 240, 100, "START", "", false, true };
Button pos2 = { BUFFER_W / 2, (BUFFER_H / 2) + 200, 240, 100, "BACK" , "", false, false };
Button pos3 = { BUFFER_W / 2, (BUFFER_H / 2) + 60, 240, 100, "GUIDE", "", false, true };
Button pos4 = { BUFFER_W / 2, (BUFFER_H / 2) + 180, 240, 100, "RANKING", "", false, true };
Button pos5 = { BUFFER_W / 2, (BUFFER_H / 2) - 60, 240, 100, "Easy", "", false, true };
Button pos6 = { BUFFER_W / 2, (BUFFER_H / 2) + 60, 240, 100, "Normal", "", false, true };
Button pos7 = { BUFFER_W / 2, (BUFFER_H / 2) + 250, 240, 100, "Hard", "", false, true };
Button pos8 = { BUFFER_W / 2, (BUFFER_H / 2) , 240, 100, "DANSO", "", false, true };
Button pos9 = { BUFFER_W / 2, (BUFFER_H / 2) + 120 , 240, 100, "ZARUBAN", "", false, true };


Button* BUT_List[] = { &pos1, &pos2, &pos3, &pos4 ,&pos5, &pos6, &pos7, &pos8, &pos9 };
#define BTN_COUNT   sizeof(BUT_List) / sizeof(BUT_List[0])

/* 히트 테스트는 항상 '버퍼 좌표계'로 비교해야 정확 */
bool pt_in_rect(float px, float py, const Button* b) {
    return b->active &&
        (px >= b->x && px <= b->x + b->w &&
            py >= b->y && py <= b->y + b->h);
}

Button* hit_button(float bx, float by)
{
    for (int i = 0; i < BTN_COUNT; ++i) {
        if (pt_in_rect(bx, by, BUT_List[i]))
            return BUT_List[i];
    }
    return NULL;
}

void update_hover_all(float bx, float by)
{
    for (int i = 0; i < BTN_COUNT; ++i) {
        BUT_List[i]->hover = pt_in_rect(bx, by, BUT_List[i]);
    }
}


/* 스케일된 디스플레이 좌표 -> 버퍼 좌표 변환 */
float to_buffer_x(float mx)
{
    return mx * (float)BUFFER_W / (float)DISP_W;
}

float to_buffer_y(float my)
{
    return my * (float)BUFFER_H / (float)DISP_H;
}





/* 버튼 설계 함수(직사각형 라이브러리 , 도형 색깔 채움, 폰트 하이라이트) */
void Button_draw(const Button* pos, ALLEGRO_FONT* font)
{
    if (!pos->active) return;

    // 버튼 보드 색깔 
    ALLEGRO_COLOR btn_col = pos->hover
        ? al_map_rgb(80, 170, 240)
        : al_map_rgb(60, 130, 200);
    ALLEGRO_COLOR btn_border = al_map_rgb(10, 10, 10);

    al_draw_filled_rectangle(pos->x, pos->y, pos->x + pos->w, pos->y + pos->h, btn_col);

    al_draw_rectangle(pos->x, pos->y, pos->x + pos->w, pos->y + pos->h, btn_border, 4.0f);

    int tw = al_get_text_width(font, pos->label1);
    int th = al_get_font_line_height(font);
    float tx = pos->x + (pos->w - tw) / 2.0f;
    float ty = pos->y + (pos->h - th) / 2.0f;

    al_draw_text(font, al_map_rgb(255, 255, 255), tx, ty, 0, pos->label1);
}



/* --- FIRST Game UI State  --- */
typedef enum {
    STATE_MENU = 10, // 첫 시작 화면
    STATE_MODE = 11, // 난이도 화면
    STATE_CHOICE = 12,// 직업 선택
    STATE_RUNNING = 13   // 게임 시작 누를 때 실행
} GameState;

/* --- 게임 난이도에 따른 구조체 선언 ---*/

typedef enum {
    DIFF_EASY = 1,   // 하 (×1)
    DIFF_NORMAL = 2, // 중 (×2)
    DIFF_HARD = 3 // 상 (×3)
}DIFFICULTY;
DIFFICULTY game_difficulty;

// 난이도에 따른 초기값 구현
// 


/* --- 직업에 대한 구조체 선언 --- */
typedef enum {
    JOB_None = 0,
    JOB_DANSO = 8,
    JOB_ZARUBAN = 9
}JOB;

JOB job = JOB_None;


/* --- display menu ---  */
void show_back_only(void)
{
    pos1.active = false;
    pos3.active = false;
    pos4.active = false;
    pos5.active = false;
    pos6.active = false;
    pos7.active = false;
    pos8.active = false;
    pos9.active = false;
    // Back 버튼 활성화
    pos2.active = true;
}

void show_main_menu(void)
{
    pos1.active = true;
    pos3.active = true;
    pos4.active = true;
    pos2.active = false;    // 백 버튼 비활성화
}


/* --- Prologue ---*/
#define PRO 14

// 알레그로 비트맵 사진 리스트
ALLEGRO_BITMAP* prologue_List[PRO] = { 0 };

// 프롤로그 상태 구조체 선언
typedef struct {
    int curr;   // 현재 위치
    int start;  // 시작 위치
    int end;    // 끝 위치
    bool blink; // 빈 화면의 유무
}PROLOGUE_STATE;


// 프롤로그 상태 초기값
PROLOGUE_STATE ps = { 0,0,0,false };

// 직업에 따라 달라지는 프롤로그 설정 함수
static void set_pro_job(void)
{
    switch (job)
    {
    case JOB_DANSO:
        ps.start = 0;
        ps.end = 6;
        break;
    case JOB_ZARUBAN:
        ps.start = 7;
        ps.end = 13;
    }

    ps.curr = ps.start;
    ps.blink = false;

}

// 프롤로그 화면 함수
void prologue_display(ALLEGRO_BITMAP* bitmap)
{
    // 배경화면 사진을 버퍼 크기에 맞추기
    float w = al_get_bitmap_width(bitmap);
    float h = al_get_bitmap_height(bitmap);
    float dw = BUFFER_W;
    float dh = BUFFER_H;
    al_draw_scaled_bitmap(bitmap,
        0, 0, w, h,
        0, 0, dw, dh,
        0);

}

// 프롤로그 비트맵 로드 함수
void load_slides(void)
{

    char* files[PRO] = {
        "prologue_dialog_01.png",
        "prologue_dialog_02.png",
        "prologue_dialog_03.png",
        "prologue_dialog_04.png",
        "prologue_dialog_05.png",
        "prologue_dialog_06.png",
        "prologue_dialog_07.png",
        "prologue_dialog_08.png",
        "prologue_dialog_09.png",
        "prologue_dialog_10.png",
        "prologue_dialog_11.png",
        "prologue_dialog_12.png",
        "prologue_dialog_13.png",
        "prologue_dialog_14.png"
    };

    for (int i = 0; i < PRO; ++i)
    {
        prologue_List[i] = al_load_bitmap(files[i]);
        must_init(prologue_List[i], files[i]);
    }

}

// 프롤로그 비트맵 화면 삭제 함수

void destroy_slides(void) {
    for (int i = 0; i < PRO; ++i)
    {
        if (prologue_List[i])
        {
            al_destroy_bitmap(prologue_List[i]);
            prologue_List[i] = NULL;
        }
    }
}


// 프롤로그 화면 순차적으로 넘기는 함수
void next_slide()
{
    // 공백일 때
    if (ps.blink)
        return;

    // 슬라이드의 수만큼 슬라이드 인덱스 추가
    if (ps.curr < ps.end)
        ps.curr++;
    // 마지막 화면을 지날 시에 블랙 화면으로 고정
    else {
        ps.blink = true;
    }
}



/* --- main --- */
int main(void)
{
    // 알레그로 라이브러리 초기화 함수 선언
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_font_addon(), "font");
    must_init(al_init_image_addon(), "image");


    /*알레그로 이벤트 멤버 변수인 이벤트, 타이머, 폰트, 이미지 초기화 작업  */
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "builtin font");

    ALLEGRO_BITMAP* bitmap = al_load_bitmap("start_display.png");
    must_init(bitmap, "bitmap");



    // 디스플레이 화면 초기화
    disp_init();

    // 프롤로그 슬라이드 생성하기
    load_slides();

    /* 이벤트 소스는 'disp'(스케일된 창)에서 받아야 함 */
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));


    bool running = true;
    bool redraw = true;
    GameState state = STATE_MENU;

    // 알레그로 라이브러리 타이머 시작 
    al_start_timer(timer);

    // 무한 반복
    while (running)
    {
        al_wait_for_event(queue, &event);

        // 이벤트에 따른 케이스 별로 나누기
        switch (event.type) {
            // 디스플레이 화면 종료에 관련된 이벤트
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

            // 키패드 관련 이벤트 발생 시
        case ALLEGRO_EVENT_KEY_DOWN:
            // ALLEGRO_KEY_ESCAPE의 ESC키를 누르면 화면 종료
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                running = false;

            // S키 버튼을 누를 시 
            else if (event.keyboard.keycode == ALLEGRO_KEY_S && state == STATE_RUNNING)
            {
                next_slide();
                redraw = true;
            }
            break;

            // 마우스 커서 좌표 관련 이벤트
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

            // 알레그로 타이머 이벤트 
        case ALLEGRO_EVENT_TIMER:
            frames++;
            redraw = true;
            break;
        }

        /* 메인 이벤트 큐에 공백 이벤트가 발생 되는 경우 */
        if (redraw && al_is_event_queue_empty(queue)) {
            /* 버퍼에 그리고 → 스케일해서 창에 복사 */
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(30, 30, 30));   /* 배경 */
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
            disp_post_draw();
            redraw = false;
        }
    }

    /* 함수 마무리 선언(타이머, 폰트, 디스플레이, 이벤트 큐)*/
    al_destroy_timer(timer);
    al_destroy_font(font);
    disp_deinit();
    al_destroy_bitmap(bitmap);
    al_destroy_event_queue(queue);

    return 0;
}
#endif