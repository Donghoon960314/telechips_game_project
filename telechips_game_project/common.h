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
    DIFF_EASY = 1, // �� (��1)
    DIFF_NORMAL = 2, // �� (��2)
    DIFF_HARD = 3 // �� (��3)
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
#define KEY_SEEN 1 // ���� �����ӿ��� ���ȴ��� Ȯ��, ���� �����ӿ��� ������ ��Ʈ ���� (0b00000001)
#define KEY_DOWN 2 // Ű�� ��� ������ �ִ���, ������ �ִٸ� ���� �����ӱ��� ��Ʈ ���� (0b00000010)

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

    ALLEGRO_BITMAP* player_shot[2][2]; // [0: ����1, 1: ����2][0: �Ϲ�, 1: ��ų1]

    ALLEGRO_BITMAP* enemy[4];
    ALLEGRO_BITMAP* enemy_shot[3];
} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h);
ALLEGRO_BITMAP* subway_background;   // ����ö ��� �̹���
ALLEGRO_BITMAP* subway_floor;   // ����ö �ٴ� �̹���
ALLEGRO_BITMAP* background; // ��� �̹���

ALLEGRO_BITMAP* hp_heal;
ALLEGRO_BITMAP* attack_speed_up;

void sprites_init();
void sprites_deinit();

//======================================================
//                       FIRST_UI
//======================================================
/* --- Button --- */
typedef struct {
    float x;    // ���� X
    float y;    // ���� Y
    float w;    // ��
    float h;    // ����

    char label1[20];
    char label2[20]; // (unused)
    bool hover;
    bool active;    // ���� ȭ�� Ȱ��ȭ ����
} Button;

/* ��ư 4�� �ʱ�ȭ
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
    STATE_MENU = 10, // ù ���� ȭ��
    STATE_MODE = 11, // ���̵� ȭ��
    STATE_CHOICE = 12,// ���� ����
    STATE_PROLOGUE = 13,// ���ѷα�
    STATE_RUNNING = 14,   // ���� ���� ���� �� ����
    STATE_RANKING = 15 // ��ŷ ȭ��
} GameState;


/* --- ������ ���� ����ü ���� --- */
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

// �˷��׷� ��Ʈ�� ���� ����Ʈ
ALLEGRO_BITMAP* prologue_List[PRO];

// ���ѷα� ���� ����ü ����
typedef struct {
    int curr;   // ���� ��ġ
    int start;  // ���� ��ġ
    int end;    // �� ��ġ
    bool blink; // �� ȭ���� ����
}PROLOGUE_STATE;

// ���ѷα� ���� �ʱⰪ
PROLOGUE_STATE ps;

void set_pro_job(void);
void prologue_display(ALLEGRO_BITMAP* bitmap);
void load_slides(void);

void next_slide();



bool pt_in_rect(float px, float py, const Button* b);


//======================================================
//                       AUDIO
//======================================================
ALLEGRO_SAMPLE* sample_normal_shot; // �Ϲ� ����
ALLEGRO_SAMPLE* sample_strong_shot; // ���� ����
ALLEGRO_SAMPLE* sample_ENEMY1; //����1 ����
ALLEGRO_SAMPLE* sample_BOSS1; 
ALLEGRO_SAMPLE* sample_BOSS2; //�ܼһ��θ� ����

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

// �÷��̾� �̵� ����
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
    int x, y; // ��ġ ��ǥ
    int hp; // HP
    int max_hp; // �ִ� HP
    int speed; // �̵� �ӵ�
    int power_normal; // �Ϲ� ���� ���ݷ�
    int power_skill_1; // ��ų1 ���ݷ�
    //int power_skill_2; // ��ų2 ���ݷ�

    int normal_shot_timer; // �Ϲ� ���� Ÿ�̸�
    int normal_shot_cooldown; // �Ϲ� ���� ��Ÿ�� ���ذ�
    int skill_1_timer; // ��ų 1 Ÿ�̸�
    int skill_1_cooldown; // ��ų 1 ��Ÿ�� ���ذ�
    //int skill_2_timer; // ��ų 2 ��Ÿ��
    //int skill_2_cooldown; // ��ų 2 ��Ÿ�� ���ذ�

    int invincible_timer; // ���� ���� �ð�
    int attack_anim_timer; // ���� ��� ���� �ð�

    bool atk_speed_buff; // ���ݼӵ� ���� ���� ���� ����
    int atk_speed_buff_timer; // ���ݼӵ� ���� ���� Ÿ�̸�

    DIRECTION last_dir; // ������ �̵� ����
    JOB_TYPE job; // ����
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
    ENEMY_TYPE_1 = 0, // �Ϲ� ��(1)
    ENEMY_TYPE_2, // �Ϲ� ��(2)
    BOSS_TYPE_1, // ���� ��(1)
    BOSS_TYPE_2,
    ENEMY_TYPE_N // �� ���� ����
} ENEMY_TYPE;

typedef enum {
    BOSS_IDLE, // �����ִ� ����
    BOSS_CHASE // �÷��̾ �����ϴ� ����
} BOSS_STATE;

typedef struct ENEMY
{
    int x, y; // ���� ���� ��ġ
    int vx, vy; // ���� �ӵ�
    ENEMY_TYPE type; // ���� ����
    int shot_timer; // ���� ���� �������� ���� ������
    int blink; // �Ѿ� ���� �� ������ ȿ���� Ÿ�̸�
    int hp; // ü��
    bool used; // ��� ������, ȭ�鿡 �����ϴ��� ����
   
    // ������ �߰�
    BOSS_STATE state;
    int state_timer;
} ENEMY;

#define ENEMIES_N 5
ENEMY enemies[ENEMIES_N];

bool spawn_enabled; // ������ ����
bool boss_spawned; // ���� ���� ����
int boss_spawn_timer; // ���� ���� ��� Ÿ�̸�


void enemies_init();
void enemies_update();
void enemies_draw();

//======================================================
//                        SHOT
//======================================================
// �÷��̾� �Ѿ� ����
typedef enum SHOT_DIR {
    SHOT_UP,
    SHOT_DOWN,
    SHOT_LEFT,
    SHOT_RIGHT
} SHOT_DIR;

typedef enum {
    ATTACK_NORMAL, // �÷��̾� �Ϲ� ����
    ATTACK_SKILL_1, // �÷��̾� ��ų1

    ATTACK_ENEMY, // �Ϲݸ� ����
    ATTACK_BOSS1, // ������ ����
    ATTACK_BOSS2

} ATTACK_TYPE;

typedef struct SHOT
{
    int x, y, dx, dy; // �Ѿ� ��ġ, �̵� �ӵ�
    int frame; // �ִϸ��̼� ������
    bool player; // �÷��̾� �Ѿ�(true), �� �Ѿ�(false) ����
    bool used; // ��� ����(Ȱ��ȭ�� �Ѿ�����)
    SHOT_DIR dir; // �Ѿ� ����
    int power; // �Ѿ� ���ݷ�
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
    ITEM_HEAL, // ü�� ȸ��
    ITEM_ATK_SPEED // ���� �ӵ� ����
} ITEM_TYPE;

typedef struct ITEM {
    int x, y; // ������ ��ġ
    ITEM_TYPE type; // ������ ����
    bool used; // ���� ȭ�鿡 �����ϴ���
    int lifetime; // ������ ���� �ð�
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
ALLEGRO_FONT* font; // HUD�� ��Ʈ
long score_display; // ȭ�鿡 ǥ���� ����

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

