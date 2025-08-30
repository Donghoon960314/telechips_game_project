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

typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;

    ALLEGRO_BITMAP* player1;
    ALLEGRO_BITMAP* player1_attack;
    ALLEGRO_BITMAP* player2;
    ALLEGRO_BITMAP* player2_attack;

    ALLEGRO_BITMAP* player_shot[2][2]; // [0: ����1, 1: ����2][0: �Ϲ�, 1: ��ų1]

    ALLEGRO_BITMAP* enemy[4];
    ALLEGRO_BITMAP* enemy_shot[2];
} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h);
ALLEGRO_BITMAP* subway_background;   // ����ö ��� �̹���
ALLEGRO_BITMAP* subway_floor;   // ����ö �ٴ� �̹���

void sprites_init();
void sprites_deinit();

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
void draw_background();
void draw_floor();
void draw_horizon_lines();
void draw_vertical_lines();
