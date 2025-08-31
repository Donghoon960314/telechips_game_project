#pragma once
//======================================================
//                       MAIN
//======================================================
int time_limit;
int time_left;



//======================================================
//                      GENERAL
//======================================================
long frames;
long score;
extern stage_num_for;
extern stage_num;
extern delay;
extern bool restarted;

typedef enum {
    DIFF_EASY = 1, // 하 (×1)
    DIFF_NORMAL = 2, // 중 (×2)
    DIFF_HARD = 3 // 상 (×3)
} DIFFICULTY;
DIFFICULTY game_difficulty;

void must_init(bool test, const char* description);
int between(int lo, int hi);
float between_f(float lo, float hi);
bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);

// display
#define BUFFER_W 1600
#define BUFFER_H 900

#define DISP_W BUFFER_W
#define DISP_H BUFFER_H

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

void disp_init();
void disp_deinit();
void disp_pre_draw();
void disp_post_draw();

//======================================================
//                     KEYBOARD
//======================================================
#define KEY_SEEN 1 // 현재 프레임에서 눌렸는지 확인, 다음 프레임에선 무조건 비트 해제 (0b00000001)
#define KEY_DOWN 2 // 키를 계속 누르고 있는지, 누르고 있다면 다음 프레임까지 비트 유지 (0b00000010)

unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init();
void keyboard_update(ALLEGRO_EVENT* event);

//======================================================
//                      SPRITES
//======================================================
#define PLAYER_W 80
#define PLAYER_H 80
/*
#define PLAYER_W 50
#define PLAYER_H 100

#define PLAYER_SHOT_W 90
#define PLAYER_SHOT_H 60

extern const int ENEMY_W[];
extern const int ENEMY_H[];

#define ENEMY_1_W ENEMY_W[0]
#define ENEMY_1_H ENEMY_H[0]
#define ENEMY_2_W ENEMY_W[1]
#define ENEMY_2_H ENEMY_H[1]
#define BOSS_1_W ENEMY_W[2]
#define BOSS_1_H ENEMY_H[2]
#define BOSS_2_W ENEMY_W[3]
#define BOSS_2_H ENEMY_H[3]

#define ENEMY_SHOT_W 4
#define ENEMY_SHOT_H 4
*/
extern const int PLAYER_SHOT_W[2];
extern const int PLAYER_SHOT_H[2];
extern int PLAYER_SHOT_WIDTH;
extern int PLAYER_SHOT_HEIGHT;

#define PLAYER_SHOT_1_W PLAYER_SHOT_W[0];
#define PLAYER_SHOT_1_H PLAYER_SHOT_H[0];
#define PLAYER_SHOT_2_W PLAYER_SHOT_W[1];
#define PLAYER_SHOT_2_H PLAYER_SHOT_H[1];


extern const int ENEMY_W[];
extern const int ENEMY_H[];

#define ENEMY_1_W ENEMY_W[0]
#define ENEMY_1_H ENEMY_H[0]
#define ENEMY_2_W ENEMY_W[1]
#define ENEMY_2_H ENEMY_H[1]
#define BOSS_1_W ENEMY_W[2]
#define BOSS_1_H ENEMY_H[2]
#define BOSS_2_W ENEMY_W[3]
#define BOSS_2_H ENEMY_H[3]

extern const int ENEMY_SHOT_W[];
extern const int ENEMY_SHOT_H[];
extern int ENEMY_SHOT_WIDTH;
extern int ENEMY_SHOT_HEIGHT;

#define ENEMY_SHOT_1_W ENEMY_SHOT_W[0]
#define ENEMY_SHOT_1_H ENEMY_SHOT_H[0]
#define ENEMY_SHOT_2_W ENEMY_SHOT_W[1]
#define ENEMY_SHOT_2_H ENEMY_SHOT_H[1]

typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;

    ALLEGRO_BITMAP* player1;
    ALLEGRO_BITMAP* player1_attack;
    ALLEGRO_BITMAP* player2;
    ALLEGRO_BITMAP* player2_attack;

    ALLEGRO_BITMAP* player_shot[2][2]; // [0: 직업1, 1: 직업2][0: 일반, 1: 스킬1]

    ALLEGRO_BITMAP* enemy[4];
    ALLEGRO_BITMAP* enemy_shot[3];
} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h);
ALLEGRO_BITMAP* subway_background;   // 지하철 배경 이미지
ALLEGRO_BITMAP* subway_floor;   // 지하철 바닥 이미지
ALLEGRO_BITMAP* background; // 배경 이미지

ALLEGRO_BITMAP* hp_heal;
ALLEGRO_BITMAP* attack_speed_up;

void sprites_init();
void sprites_deinit();

//======================================================
//                       FIRST_UI
//======================================================
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

Button pos1;
Button pos2;
Button pos3;
Button pos4;
Button pos5;
Button pos6;
Button pos7;
Button pos8;
Button pos9;

Button* BUT_List[];
#define BTN_COUNT   sizeof(BUT_List) / sizeof(BUT_List[0])
Button* hit_button(float bx, float by);
void update_hover_all(float bx, float by);
float to_buffer_x(float mx);
float to_buffer_y(float my);
void Button_draw(const Button* pos, ALLEGRO_FONT* font);

/* --- FIRST Game UI State  --- */
typedef enum {
    STATE_MENU = 10, // 첫 시작 화면
    STATE_MODE = 11, // 난이도 화면
    STATE_CHOICE = 12,// 직업 선택
    STATE_PROLOGUE = 13,// 프롤로그
    STATE_RUNNING = 14,   // 게임 시작 누를 때 실행
    STATE_RANKING = 15 // 랭킹 화면
} GameState;


/* --- 직업에 대한 구조체 선언 --- */
typedef enum {
    JOB_None = 0,
    JOB_DANSO = 8,
    JOB_ZARUBAN = 9
}JOB;

JOB job;

void show_back_only(void);

void show_main_menu(void);

/* --- Prologue ---*/
#define PRO 14

// 알레그로 비트맵 사진 리스트
ALLEGRO_BITMAP* prologue_List[PRO];

// 프롤로그 상태 구조체 선언
typedef struct {
    int curr;   // 현재 위치
    int start;  // 시작 위치
    int end;    // 끝 위치
    bool blink; // 빈 화면의 유무
}PROLOGUE_STATE;

// 프롤로그 상태 초기값
PROLOGUE_STATE ps;

void set_pro_job(void);
void prologue_display(ALLEGRO_BITMAP* bitmap);
void load_slides(void);

void next_slide();



bool pt_in_rect(float px, float py, const Button* b);


//======================================================
//                       AUDIO
//======================================================
ALLEGRO_SAMPLE* sample_normal_shot; // 일반 공격
ALLEGRO_SAMPLE* sample_strong_shot; // 강한 공격
ALLEGRO_SAMPLE* sample_ENEMY1; //몬스터1 공격
ALLEGRO_SAMPLE* sample_BOSS1; 
ALLEGRO_SAMPLE* sample_BOSS2; //단소살인마 공격

ALLEGRO_AUDIO_STREAM* bgm_stream;


void audio_init();
void audio_deinit();

//======================================================
//                      COLLIDE
//======================================================
int enemies_collide(bool player, int x, int y, int w, int h);

//======================================================
//                      PLAYER
//======================================================
float DEPTH_MIN_SCALE;
float DEPTH_MAX_SCALE;

#define PLAYER_SPEED 10
#define PLAYER_MAX_X (BUFFER_W - PLAYER_W)
#define PLAYER_MIN_Y (BUFFER_H * 0.4)
#define PLAYER_MAX_Y (BUFFER_H - PLAYER_H) * 0.78

// 플레이어 이동 방향
typedef enum DIRECTION {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} DIRECTION;

typedef enum { 
    JOB_TYPE_1, 
    JOB_TYPE_2
} JOB_TYPE;
JOB_TYPE job_type;


typedef struct PLAYER
{
    int x, y; // 위치 좌표
    int hp; // HP
    int max_hp; // 최대 HP
    int speed; // 이동 속도
    int power_normal; // 일반 공격 공격력
    int power_skill_1; // 스킬1 공격력
    //int power_skill_2; // 스킬2 공격력

    int normal_shot_timer; // 일반 공격 타이머
    int normal_shot_cooldown; // 일반 공격 쿨타임 기준값
    int skill_1_timer; // 스킬 1 타이머
    int skill_1_cooldown; // 스킬 1 쿨타임 기준값
    //int skill_2_timer; // 스킬 2 쿨타임
    //int skill_2_cooldown; // 스킬 2 쿨타임 기준값

    int invincible_timer; // 무적 상태 시간
    int attack_anim_timer; // 공격 모션 유지 시간

    bool atk_speed_buff; // 공격속도 증가 버프 적용 여부
    int atk_speed_buff_timer; // 공격속도 증가 버프 타이머

    DIRECTION last_dir; // 마지막 이동 방향
    JOB_TYPE job; // 직업
} PLAYER;
PLAYER player;

void player_init();
void player_update();
void player_draw();

//======================================================
//                      ENEMY
//======================================================
typedef enum ENEMY_TYPE
{
    ENEMY_TYPE_1 = 0, // 일반 몹(1)
    ENEMY_TYPE_2, // 일반 몹(2)
    BOSS_TYPE_1, // 보스 몹(1)
    BOSS_TYPE_2,
    ENEMY_TYPE_N // 몹 종류 개수
} ENEMY_TYPE;

typedef enum {
    BOSS_IDLE, // 멈춰있는 상태
    BOSS_CHASE // 플레이어를 추적하는 상태
} BOSS_STATE;

typedef struct ENEMY
{
    int x, y; // 몹의 현재 위치
    int vx, vy; // 몹의 속도
    ENEMY_TYPE type; // 몹의 종류
    int shot_timer; // 다음 공격 시점까지 남은 프레임
    int blink; // 총알 맞은 후 깜빡인 효과용 타이머
    int hp; // 체력
    bool used; // 사용 중인지, 화면에 존재하는지 여부
   
    // 보스용 추가
    BOSS_STATE state;
    int state_timer;
} ENEMY;

#define ENEMIES_N 5
ENEMY enemies[ENEMIES_N];

bool spawn_enabled; // 리스폰 여부
bool boss_spawned; // 보스 등장 여부
int boss_spawn_timer; // 보스 등장 대기 타이머


void enemies_init();
void enemies_update();
void enemies_draw();

//======================================================
//                        SHOT
//======================================================
// 플레이어 총알 방향
typedef enum SHOT_DIR {
    SHOT_UP,
    SHOT_DOWN,
    SHOT_LEFT,
    SHOT_RIGHT
} SHOT_DIR;

typedef enum {
    ATTACK_NORMAL, // 플레이어 일반 공격
    ATTACK_SKILL_1, // 플레이어 스킬1

    ATTACK_ENEMY, // 일반몹 공격
    ATTACK_BOSS1, // 보스몹 공격
    ATTACK_BOSS2

} ATTACK_TYPE;

typedef struct SHOT
{
    int x, y, dx, dy; // 총알 위치, 이동 속도
    int frame; // 애니메이션 프레임
    bool player; // 플레이어 총알(true), 적 총알(false) 구분
    bool used; // 사용 여부(활성화된 총알인지)
    SHOT_DIR dir; // 총알 방향
    int power; // 총알 공격력
    ATTACK_TYPE attack_type;
} SHOT;

#define SHOTS_N 128
SHOT shots[SHOTS_N];

void shots_init();
bool shots_add(bool player, bool straight, int x, int y, DIRECTION dir, int power, ATTACK_TYPE attack_type);
void shots_update();
int shots_collide(bool player, int x, int y, int w, int h);
void shots_draw();

//======================================================
//                      ITEM
//======================================================
typedef enum {
    ITEM_HEAL, // 체력 회복
    ITEM_ATK_SPEED // 공격 속도 증가
} ITEM_TYPE;

typedef struct ITEM {
    int x, y; // 아이템 위치
    ITEM_TYPE type; // 아이템 종류
    bool used; // 현재 화면에 존재하는지
    int lifetime; // 아이템 남은 시간
} ITEM;

#define ITEMS_N 3
ITEM items[ITEMS_N];

int item_spawn_timer;

void items_init();
void items_update();
void items_draw();

//======================================================
//                      HUD
//======================================================
ALLEGRO_FONT* font; // HUD용 폰트
long score_display; // 화면에 표시할 점수

void hud_init();
void hud_draw();
void hud_deinit();

//======================================================
//                   BACKGROUND
//======================================================
float shake_y;
int shake_timer;

void update_shake();
void draw_background();
void draw_subway_background();
void draw_floor();
void draw_horizon_lines();
void draw_vertical_lines();

//======================================================
//                   RANK LIST
//======================================================
typedef struct RANK {
    char name[32];
    int minutes;
    int seconds;
} _RANK;

int cmp_rank(const void* a, const void* b);
void print_ranking_table(const char* player_name, int player_min, int player_sec);

//======================================================
//                RANKING INPUT
//======================================================
void rank_name_open(int time, char* rank_name, int* rank_min, int* rank_sec);

