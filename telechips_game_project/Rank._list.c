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

//분 단위로는 오름차순->분이 같을때 초단위로 오름차순
int cmp_rank(const void* a, const void* b) 
{
    const _RANK* x = (const _RANK*)a; //Rank구조체 가르키는 포인터 x와 y
    const _RANK* y = (const _RANK*)b;

    if (x->minutes != y->minutes) return (x->minutes - y->minutes);
    return (x->seconds - y->seconds);
}

void print_ranking_table(const char* player_name, int player_min, int player_sec)
{
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");
    must_init(al_init_image_addon(), "image addon");

    ALLEGRO_FONT* name_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);
    ALLEGRO_FONT* title_font = al_load_ttf_font("BebasNeue-Regular.ttf",80, 0);

    must_init(name_font, "score_font");
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));

    //rank.txt 읽어옴
    FILE* f = fopen("rank.txt", "r");
    _RANK entries[128];
    int count = 0;
    if (f) 
    {
        while (fscanf(f, "%31s %d %d", entries[count].name, &entries[count].minutes, &entries[count].seconds) == 3) 
        {
            count++;
            if (count >= 128) break;
        }
        fclose(f);
    }
    qsort(entries, count, sizeof(_RANK), cmp_rank);

    bool done = false;
    ALLEGRO_EVENT event;

    al_start_timer(timer);

    while (!done)
    {
        while (al_get_next_event(queue, &event)) 
        {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                done = true;
            if (event.type == ALLEGRO_EVENT_KEY_DOWN &&
                event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                done = true;
        }

        disp_pre_draw();
        al_clear_to_color(al_map_rgb(0, 0, 0));

        // 제목
        al_draw_text(title_font, al_map_rgb(255, 255, 255), DISP_W / 2, 20, ALLEGRO_ALIGN_CENTER, "SCOREBOARD - TOP 10");

        // 상위 10개 출력
        for (int i = 0; i < count && i < 10; i++) 
        {
            char buf[128];
            //sprint로 버퍼에 우선 저장 나중에 al_draw로 출력할거임
            sprintf(buf, "%2d. %-10s  %02d:%02d", i + 1, entries[i].name, entries[i].minutes, entries[i].seconds); 

            //sprintf s %2 -> 정수출력 최소 10칸 확보
            // -10s은 최소 10칸에서 -는 왼쪽정렬
            //02d는 항상 두개의숫자로 빈칸을 0으로 채움

            int y = 120 + i * 80;
            //이름이랑 기록 비교해서 마냥 입력한 값이랑 같으면 하이라이트 줌 
            if (strcmp(entries[i].name, player_name) == 0 && entries[i].minutes == player_min && entries[i].seconds == player_sec)
            {
                al_draw_filled_rectangle(DISP_W / 2 - 300, y - 10, DISP_W / 2 + 300, y + 60, al_map_rgb(50, 50, 150)); // 파란 박스
            }
            // 텍스트 출력
            al_draw_text(name_font, al_map_rgb(200, 200, 0), DISP_W / 2, y, ALLEGRO_ALIGN_CENTER, buf);
        }

        disp_post_draw();
           
            //al_draw_text(name_font, al_map_rgb(200, 200, 0),DISP_W / 2, 120 + i * 80, ALLEGRO_ALIGN_CENTER, buf);
       

        //disp_post_draw();
    }
    al_destroy_event_queue(queue);
}

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
long frames;
long score;

void must_init(bool test, const char* description)
{
    if (test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

// --- display ---

#define BUFFER_W 320 //display 크기(가로) //비율
#define BUFFER_H 240//display 크기(세로)

#define DISP_SCALE 3 //크기
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;


void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
}


void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display(); //flip해서 찍는 함수

}
// --- sprites ---

#define SHIP_W 12
#define SHIP_H 13
//적군 크기

#define SHIP_SHOT_W 2
#define SHIP_SHOT_H 9
//적군 공격 크기

#define LIFE_W 6
#define LIFE_H 6
//남은 목숨 크기

const int ALIEN_W[] = { 14, 13, 45, 45 };
const int ALIEN_H[] = { 9, 10, 27, 45 };

#define ENEMY_LEVEL1_W ALIEN_W[0] //14
#define ENEMY_LEVEL1_H ALIEN_H[0] //9
#define ENEMY_LEVEL2_W ALIEN_W[1] //13
#define ENEMY_LEVEL2_H ALIEN_H[1] //10
#define ENEMY_BOSS1_W ALIEN_W[2] //45
#define ENEMY_BOSS1_H ALIEN_H[2] //27
#define ENEMY_BOSS2_W ALIEN_W[3] //45
#define ENEMY_BOSS2_H ALIEN_H[3]//27

#define ENEMY_SHOT_W 8
#define ENEMY_SHOT_H 8

#define EXPLOSION_FRAMES 4
#define SPARKS_FRAMES    3


typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;

} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("단소살인마 캐릭터.png");
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites._sheet);
}

void table(void)
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    disp_init();
    must_init(al_init_image_addon(), "image addon");  
    sprites_init();                                   


    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    frames = 0;
    score = 0;

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);

    while (1)
    {
        al_wait_for_event(queue, &event);

        
            disp_post_draw();
            redraw = false;
        
    }
    disp_deinit();
    return 0;
}
#endif

#if 0
// dnf_like_full.c
#define _CRT_SECURE_NO_WARNINGS
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// ---------------- Display ----------------
#define BUFFER_W 320
#define BUFFER_H 240
#define SCALE    3
#define DISP_W   (BUFFER_W * SCALE)
#define DISP_H   (BUFFER_H * SCALE)

static ALLEGRO_DISPLAY* disp = NULL;
static ALLEGRO_BITMAP* buffer = NULL;
static ALLEGRO_EVENT_QUEUE* queue = NULL;
static ALLEGRO_TIMER* timer = NULL;

static void must_init(bool ok, const char* what) {
    if (ok) return;
    fprintf(stderr, "couldn't initialize %s\n", what);
    exit(1);
}

static void disp_init(void) {
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");
    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "buffer");
}
static void disp_pre(void) { al_set_target_bitmap(buffer); }
static void disp_post(void) {
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
}

// ---------------- Input ----------------
#define KEY_SEEN 1
#define KEY_DOWN 2
static unsigned char key[ALLEGRO_KEY_MAX];

static void key_init(void) { memset(key, 0, sizeof(key)); }
static void key_update(ALLEGRO_EVENT* e) {
    switch (e->type) {
    case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) key[i] &= ~KEY_SEEN;
        break;
    case ALLEGRO_EVENT_KEY_DOWN:
        key[e->keyboard.keycode] = KEY_SEEN | KEY_DOWN;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        key[e->keyboard.keycode] &= ~KEY_DOWN;
        break;
    }
}
static bool pressed(int k) { return (key[k] & KEY_SEEN) != 0; }
static bool down(int k) { return (key[k] & KEY_DOWN) != 0; }

// ---------------- World / Physics ----------------
// 벨트스크롤 워킹 밴드와 점프용 바닥선
#define WALK_TOP     110.0f
#define WALK_BOTTOM  180.0f
#define GROUND_Y     180.0f

#define GRAV         0.35f
#define MOVE_ACC     0.60f
#define MOVE_SPEED_X 2.60f
#define MOVE_SPEED_Y 1.80f
#define JUMP_V0     -4.60f
#define FRICTION     0.85f

// ---------------- Player Sprite Sheet ----------------
// !!! 네 이미지에 맞게 수정 !!!
#define PLAYER_SHEET "test_image.png"
#define PLAYER_COLS  4      // 가로 프레임 수
#define PLAYER_ROWS  3      // 세로 프레임 수 (예: 0=기본/수평, 1=위로걷기, 2=아래로걷기)
#define FRAME_W      24     // 프레임 가로 px
#define FRAME_H      32     // 프레임 세로 px
#define ANIM_TICKS   6      // 작을수록 빠르게 프레임 전환

// 프레임 패딩/오프셋이 있으면 조정
#define PAD_X 0
#define PAD_Y 0
#define OFF_X 0
#define OFF_Y 0

static ALLEGRO_BITMAP* player_sheet = NULL;
static int anim_col = 0;     // 0..PLAYER_COLS-1
static int anim_row = 0;     // 0..PLAYER_ROWS-1
static int anim_tick = 0;

// ---------------- Fonts ----------------
static ALLEGRO_FONT* font = NULL;

// ---------------- Player ----------------
typedef struct PLAYER {
    float x, y;
    float vx, vy;
    int   w, h;
    int   dir;        // -1: left, +1: right
    bool  on_ground;
} PLAYER;
static PLAYER player;

static void player_init(void) {
    player.w = FRAME_W;       // 스프라이트 프레임 폭을 히트박스로 사용(원하면 따로 둬도 됨)
    player.h = FRAME_H;
    player.x = (BUFFER_W - player.w) / 2.0f;
    player.y = GROUND_Y - player.h;
    player.vx = player.vy = 0.0f;
    player.dir = +1;
    player.on_ground = true;
}

static void player_anim_update(bool is_moving, int face_dir, int vertical_dir) {
    // row 매핑: 0=수평/기본, 1=위로 걷기, 2=아래로 걷기 (원하면 바꿔도 됨)
    if (vertical_dir < 0)      anim_row = 1;
    else if (vertical_dir > 0) anim_row = 2;
    else                       anim_row = 0;

    if (is_moving) {
        anim_tick++;
        if (anim_tick >= ANIM_TICKS) {
            anim_tick = 0;
            anim_col = (anim_col + 1) % PLAYER_COLS;
        }
    }
    else {
        anim_col = 0;
        anim_tick = 0;
    }
}

static void player_update(void) {
    // 좌우 가속
    float ax = 0.0f;
    if (down(ALLEGRO_KEY_LEFT)) { ax -= MOVE_ACC; player.dir = -1; }
    if (down(ALLEGRO_KEY_RIGHT)) { ax += MOVE_ACC; player.dir = +1; }

    if (player.on_ground && ax == 0.0f) player.vx *= FRICTION;

    player.vx += ax;
    if (player.vx > MOVE_SPEED_X) player.vx = MOVE_SPEED_X;
    if (player.vx < -MOVE_SPEED_X) player.vx = -MOVE_SPEED_X;

    // 점프 (C 키)
    if (player.on_ground && pressed(ALLEGRO_KEY_C)) {
        player.vy = JUMP_V0;
        player.on_ground = false;
    }

    // 상/하 걷기(벨트스크롤)
    if (player.on_ground) {
        if (down(ALLEGRO_KEY_UP))    player.y -= MOVE_SPEED_Y;
        if (down(ALLEGRO_KEY_DOWN))  player.y += MOVE_SPEED_Y;
    }
    else {
        // 공중에서 미세 제어 (옵션)
        if (down(ALLEGRO_KEY_UP))    player.y -= 0.4f;
        if (down(ALLEGRO_KEY_DOWN))  player.y += 0.4f;
    }

    // 중력
    player.vy += GRAV;

    // 이동 적용
    player.x += player.vx;
    player.y += player.vy;

    // 바닥 충돌(착지)
    float floor_y = GROUND_Y - player.h;
    if (player.y > floor_y) {
        player.y = floor_y;
        player.vy = 0.0f;
        player.on_ground = true;
    }

    // 워킹 밴드 클램프 (지상에서만)
    if (player.on_ground) {
        float minY = WALK_TOP;
        float maxY = WALK_BOTTOM - player.h;
        if (player.y < minY) player.y = minY;
        if (player.y > maxY) player.y = maxY;
    }

    // 화면 경계 x
    if (player.x < 0) player.x = 0;
    if (player.x + player.w > BUFFER_W) player.x = BUFFER_W - player.w;
}

static void player_draw_sprite(float x, float y, int face_dir) {
    // 시트 내 소스 좌표
    float sx = OFF_X + anim_col * (FRAME_W + PAD_X);
    float sy = OFF_Y + anim_row * (FRAME_H + PAD_Y);

    int flags = 0;
    if (face_dir < 0) flags |= ALLEGRO_FLIP_HORIZONTAL;

    al_draw_bitmap_region(player_sheet, sx, sy, FRAME_W, FRAME_H, x, y, flags);
}

static void draw_guides(void) {
    // 워킹 밴드 가이드
    al_draw_filled_rectangle(0, WALK_TOP - 1, BUFFER_W, WALK_TOP + 1, al_map_rgb(60, 60, 60));
    al_draw_filled_rectangle(0, WALK_BOTTOM + 1, BUFFER_W, WALK_BOTTOM + 3, al_map_rgb(60, 60, 60));
    // 점프용 바닥선
    al_draw_filled_rectangle(0, GROUND_Y, BUFFER_W, GROUND_Y + 2, al_map_rgb(80, 80, 80));
}

// ---------------- Main ----------------
int main(void) {
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_image_addon(), "image");
    font = al_create_builtin_font();
    must_init(font, "font");

    timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");
    queue = al_create_event_queue();
    must_init(queue, "queue");

    disp_init();

    // 이벤트 소스 등록
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    key_init();

    // 스프라이트 시트 로딩
    player_sheet = al_load_bitmap(PLAYER_SHEET);
    must_init(player_sheet, PLAYER_SHEET);

    // 플레이어 초기화
    player_init();

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT ev;

    al_start_timer(timer);

    while (!done) {
        al_wait_for_event(queue, &ev);

        switch (ev.type) {
        case ALLEGRO_EVENT_TIMER: {
            // 입력 상태로 애니메이션 파라미터 계산
            bool moveL = down(ALLEGRO_KEY_LEFT);
            bool moveR = down(ALLEGRO_KEY_RIGHT);
            bool moveU = down(ALLEGRO_KEY_UP);
            bool moveD = down(ALLEGRO_KEY_DOWN);
            bool is_moving = (moveL || moveR || moveU || moveD);

            static int face_dir = +1;
            if (moveL) face_dir = -1;
            if (moveR) face_dir = +1;

            int vertical_dir = 0;
            if (moveU) vertical_dir = -1;
            else if (moveD) vertical_dir = +1;

            // 업데이트
            player_update();
            player_anim_update(is_moving, face_dir, vertical_dir);

            if (down(ALLEGRO_KEY_ESCAPE)) done = true;

            redraw = true;
            break;
        }
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        key_update(&ev);

        if (redraw && al_is_event_queue_empty(queue)) {
            disp_pre();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            draw_guides();

            // 스프라이트 그리기
            player_draw_sprite(player.x, player.y, player.dir);

            // 디버그 텍스트(선택)
            al_draw_textf(font, al_map_rgb(255, 255, 255), 2, 2, 0,
                "x=%.1f y=%.1f vx=%.2f vy=%.2f ground=%d",
                player.x, player.y, player.vx, player.vy, player.on_ground);

            disp_post();
            redraw = false;
        }
    }

    // 정리
    al_destroy_bitmap(player_sheet);
    al_destroy_font(font);
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    return 0;
}

#endif
/////////////////////////제출코드 8.27 4:27pm//////////////////////////////////

#if 0 //ranking 화면에 출력

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#define SIZE(x) (sizeof(x)/sizeof(x[0]))
long frames;
long score;
void must_init(bool test, const char* description)
{
    if (test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

typedef struct Rank
{
    int score;
    char name[10];

}Rank_t;

int cmp_rank(const void* a, const void* b) {
    const Rank_t* x = (const Rank_t*)a;
    const Rank_t* y = (const Rank_t*)b;

    if (x->score != y->score) {
        // y - x의 부호만 반환 (오버플로 방지)
        return (y->score > x->score) - (y->score < x->score);
    }
    // 이름 오름차순
    return strcmp(x->name, y->name);
}

int Read_rank_Medium(Rank_t * rank, int CAP)
{
    FILE*fp = fopen("Medium rank.txt", "r");
    //(void)freopen("Medium rank.txt", "r", stdin);
    int n = 0;

    while (n < CAP && fscanf(fp,"%d %10s", &rank[n].score, rank[n].name) == 2) {
        ++n;  // 두 개 항목을 다 읽었을 때만 카운트
    }
    fclose(fp);
    qsort(rank, n, sizeof(rank[0]), cmp_rank);

    // 사용 예시
    /*
    for (int i = 0; i < n; ++i) {
        printf("%d %s\n", ranking[i].score, ranking[i].name);
    }
    */
    return n;
}
int Read_rank_Easy(Rank_t* rank, int CAP)
{
    FILE* fp = fopen("Easy rank.txt", "r");
    //(void)freopen("Medium rank.txt", "r", stdin);
    int n = 0;

    while (n < CAP && fscanf(fp, "%d %10s", &rank[n].score, rank[n].name) == 2) {
        ++n;  // 두 개 항목을 다 읽었을 때만 카운트
    }
    fclose(fp);
    qsort(rank, n, sizeof(rank[0]), cmp_rank);

    // 사용 예시
    /*
    for (int i = 0; i < n; ++i) {
        printf("%d %s\n", ranking[i].score, ranking[i].name);
    }
    */
    return n;
}
int Read_rank_Hard(Rank_t* rank, int CAP)
{
    FILE* fp = fopen("Hard rank.txt", "r");
    //(void)freopen("Medium rank.txt", "r", stdin);
    int n = 0;

    while (n < CAP && fscanf(fp, "%d %10s", &rank[n].score, rank[n].name) == 2) {
        ++n;  // 두 개 항목을 다 읽었을 때만 카운트
    }
    fclose(fp);
    qsort(rank, n, sizeof(rank[0]), cmp_rank);

    // 사용 예시
    /*
    for (int i = 0; i < n; ++i) {
        printf("%d %s\n", ranking[i].score, ranking[i].name);
    }
    */
    return n;
}
// --- display ---

#define BUFFER_W 400 //display 크기(가로) //비율
#define BUFFER_H 240//display 크기(세로)

#define DISP_SCALE 3 //크기
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
}

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display(); //flip해서 찍는 함수

}
// --- keyboard --
#define KEY_SEEN     1
#define KEY_DOWN     2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

// --- sprites ---
typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;
    ALLEGRO_BITMAP* background;

} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites.background = al_load_bitmap("C:/Users/user/Downloads/test_image.png");
    must_init(sprites.background, "spritesheet");

}
void sprites_deinit()
{
    al_destroy_bitmap(sprites.background);
}

// --- hud ---

ALLEGRO_FONT* font;

long score_display;

void hud_init()
{
    font = al_create_builtin_font();
    must_init(font, "font");

    score_display = 0;
}

void hud_deinit()
{
    al_destroy_font(font);
}

void hud_update()
{
    if (frames % 2)
        return;

    for (long i = 5; i > 0; i--)
    {
        long diff = 1 << i;
        if (score_display <= (score - diff))
            score_display += diff;
    }
}

void hud_draw(Rank_t* rank, int n)
{ 
    al_draw_text(font, al_map_rgb(255, 240, 180), 16, 12, 0, "RANKING");

    // 행 좌표/간격
    const int start_x = 20;
    const int start_y = 48;
    const int line_h = al_get_font_line_height(font) + 4;

    int shown = (n < 10) ? n : 10; // 상위 20개만 표시(원하면 조절)

    for (int i = 0; i < shown; ++i) {
        int y = start_y + i * line_h;
        ALLEGRO_COLOR c = al_map_rgb(255, 240, 180);
        al_draw_textf(font, c, start_x, y, 0, "%2d. %-9s  %6d", i + 1, rank[i].name, rank[i].score
        );
    }
}

void hud_draw_Rank(Rank_t* Easy, int nE,
                   Rank_t* Medium, int nM,
                   Rank_t* Hard, int nH)
{
   
    const int xE = 0;  
    const int xM = 120;  
    const int xH = 240; 

    const int name_pad = 40; 
    const int y0 = 30;       
    const int lh = al_get_font_line_height(font) + 4; 
    const int shown = 10;  

    // 타이틀
    al_draw_text(font, al_map_rgb(255, 240, 180), xE, 12, 0, "EASY");
    al_draw_text(font, al_map_rgb(255, 240, 180), xM, 12, 0, "MED");
    al_draw_text(font, al_map_rgb(255, 240, 180), xH, 12, 0, "HARD");

    for (int i = 0; i < shown; ++i) {
        int y = y0 + i * lh;

        if (i < nE) {
            // Easy: 이름과 점수를 따로 출력
            al_draw_textf(font, al_map_rgb(235, 235, 235), xE, y, 0, "%2d. %s", i + 1, Easy[i].name);
            al_draw_textf(font, al_map_rgb(235, 235, 235), xE + name_pad, y, 0, "%6d", Easy[i].score);
        }

        if (i < nM) {
            al_draw_textf(font, al_map_rgb(235, 235, 235), xM, y, 0, "%2d. %s", i + 1, Medium[i].name);
            al_draw_textf(font, al_map_rgb(235, 235, 235), xM + name_pad, y, 0, "%6d", Medium[i].score);
        }

        if (i < nH) {
            al_draw_textf(font, al_map_rgb(235, 235, 235), xH, y, 0, "%2d. %s", i + 1, Hard[i].name);
            al_draw_textf(font, al_map_rgb(235, 235, 235), xH + name_pad, y, 0, "%6d", Hard[i].score);
        }
    }
}

/*
void hud_draw_Hard(Rank_t* rank, int n)
{
    al_draw_text(font, al_map_rgb(255, 240, 180), 16, 12, 0, "RANKING");

    // 행 좌표/간격
    const int start_x = 60;
    const int start_y = 48;
    const int line_h = al_get_font_line_height(font) + 4;

    int shown = (n < 10) ? n : 10; // 상위 20개만 표시(원하면 조절)

    for (int i = 0; i < shown; ++i) {
        int y = start_y + i * line_h;
        ALLEGRO_COLOR c = al_map_rgb(255, 240, 180);
        al_draw_textf(font, c, start_x, y, 0, "%2d. %-9s  %6d", i + 1, rank[i].name, rank[i].score
        );
    }
}
*/

int main(void) {

    Rank_t easy[100], medium[100], hard[100];

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    disp_init();

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");
    must_init(al_init_image_addon(), "image");
    must_init(al_init_font_addon(), "font");

    hud_init();
    must_init(font, "font");

    sprites_init();
    keyboard_init();

    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_keyboard_event_source());

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    int ne = Read_rank_Easy(easy, SIZE(easy));
    int nm = Read_rank_Medium(medium, SIZE(medium));
    int nh = Read_rank_Hard(hard, SIZE(hard));

    disp_pre_draw();
    {
        int bw = al_get_bitmap_width(sprites.background);
        int bh = al_get_bitmap_height(sprites.background);
        al_draw_scaled_bitmap(sprites.background, 0, 0, bw, bh,
            0, 0, BUFFER_W, BUFFER_H, 0);

        hud_draw_Rank(easy, ne, medium, nm, hard, nh);
    }
    disp_post_draw();

    bool running = true;
    ALLEGRO_EVENT ev;
    while (running) {
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN &&
            ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            running = false;
        }
        // (원한다면 DISPLAY_RESIZE / EXPOSE 때 다시 그리기 추가 가능)
    }

    // 정리
    sprites_deinit();   // 여기서 al_destroy_bitmap(sprites.background);
    al_destroy_event_queue(queue);
    disp_deinit();      // buffer/display 파괴
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

#if 0
// ranking_view.c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#define NAME_LEN   10          // 이름 최대 9글자 + '\0'
#define MAX_RANKS  100         // 최대 레코드 수
#define WIN_W      800
#define WIN_H      600

typedef struct {
    int  score;
    char name[NAME_LEN];
} Rank;
// 점수 내림차순, 동점 시 이름 오름차순
static int cmp_rank(const void* a, const void* b) {
    const Rank* x = (const Rank*)a, * y = (const Rank*)b;
    if (x->score != y->score)
        return (y->score > x->score) - (y->score < x->score); // 내림차순
    return strcmp(x->name, y->name);                          // 오름차순
}

// 파일에서 순위 읽기(EOF까지) + 정렬. 반환: 읽은 개수
static int load_ranking(const char* path, Rank* out, int maxn) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        perror("fopen");
        return 0;
    }
    int n = 0;
    while (n < maxn && fscanf(fp, "%d %9s", &out[n].score, out[n].name) == 2) {
        ++n;
    }
    fclose(fp);

    qsort(out, n, sizeof(out[0]), cmp_rank);
    return n;
}

int main(void) {
    // 1) 데이터 읽기 + 정렬
    Rank ranks[MAX_RANKS] = { 0 };
    int N = load_ranking("Medium rank.txt", ranks, MAX_RANKS);

    // 2) Allegro 초기화
    if (!al_init()) return 1;
    al_install_keyboard();
    al_init_font_addon();                       // 기본 폰트 사용 (TTF 불필요)

    ALLEGRO_DISPLAY* disp = al_create_display(WIN_W, WIN_H);
    ALLEGRO_EVENT_QUEUE* q = al_create_event_queue();
    ALLEGRO_FONT* font = al_create_builtin_font();
    if (!disp || !q || !font) return 1;

    al_register_event_source(q, al_get_display_event_source(disp));
    al_register_event_source(q, al_get_keyboard_event_source());

    // 3) 1프레임 그려놓고
    al_clear_to_color(al_map_rgb(20, 24, 28));
    al_draw_text(font, al_map_rgb(255, 255, 255), 32, 24, 0, "RANKING - Medium");

    int shown = N < 20 ? N : 20;               // 화면엔 상위 20개만
    int line_h = al_get_font_line_height(font) + 6;
    int y0 = 64;

    for (int i = 0; i < shown; ++i) {
        al_draw_textf(
            font, al_map_rgb(230, 230, 230),
            40, y0 + i * line_h, 0,
            "%2d. %-9s  %6d", i + 1, ranks[i].name, ranks[i].score
        );
    }
    al_draw_text(font, al_map_rgb(160, 160, 160), 32, WIN_H - 32, 0,
        "Press ESC or close the window to exit.");
    al_flip_display();

    // 4) 이벤트 루프 (ESC/창닫기 종료)
    bool running = true;
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(q, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            running = false;
    }

    // 5) 정리
    al_destroy_font(font);
    al_destroy_event_queue(q);
    al_destroy_display(disp);
    return 0;
}
#endif