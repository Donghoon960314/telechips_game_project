/*
- ���� ���� ȭ�� ��ư 3��
- ���̵��� ���� ��ư 3�� ���� +
- ��� ȭ�� �ֱ�
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

// �˷��׷� �ʱ�ȭ �Լ� �۾� -> �ش� �۾� ������ ���� �� ���
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

// ���÷��� ȭ�� �ʱ�ȭ �Լ�
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

/* ��Ʈ �׽�Ʈ�� �׻� '���� ��ǥ��'�� ���ؾ� ��Ȯ */
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


/* �����ϵ� ���÷��� ��ǥ -> ���� ��ǥ ��ȯ */
float to_buffer_x(float mx)
{
    return mx * (float)BUFFER_W / (float)DISP_W;
}

float to_buffer_y(float my)
{
    return my * (float)BUFFER_H / (float)DISP_H;
}





/* ��ư ���� �Լ�(���簢�� ���̺귯�� , ���� ���� ä��, ��Ʈ ���̶���Ʈ) */
void Button_draw(const Button* pos, ALLEGRO_FONT* font)
{
    if (!pos->active) return;

    // ��ư ���� ���� 
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
    STATE_MENU = 10, // ù ���� ȭ��
    STATE_MODE = 11, // ���̵� ȭ��
    STATE_CHOICE = 12,// ���� ����
    STATE_RUNNING = 13   // ���� ���� ���� �� ����
} GameState;

/* --- ���� ���̵��� ���� ����ü ���� ---*/

typedef enum {
    DIFF_EASY = 1,   // �� (��1)
    DIFF_NORMAL = 2, // �� (��2)
    DIFF_HARD = 3 // �� (��3)
}DIFFICULTY;
DIFFICULTY game_difficulty;

// ���̵��� ���� �ʱⰪ ����
// 


/* --- ������ ���� ����ü ���� --- */
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
    // Back ��ư Ȱ��ȭ
    pos2.active = true;
}

void show_main_menu(void)
{
    pos1.active = true;
    pos3.active = true;
    pos4.active = true;
    pos2.active = false;    // �� ��ư ��Ȱ��ȭ
}


/* --- Prologue ---*/
#define PRO 14

// �˷��׷� ��Ʈ�� ���� ����Ʈ
ALLEGRO_BITMAP* prologue_List[PRO] = { 0 };

// ���ѷα� ���� ����ü ����
typedef struct {
    int curr;   // ���� ��ġ
    int start;  // ���� ��ġ
    int end;    // �� ��ġ
    bool blink; // �� ȭ���� ����
}PROLOGUE_STATE;


// ���ѷα� ���� �ʱⰪ
PROLOGUE_STATE ps = { 0,0,0,false };

// ������ ���� �޶����� ���ѷα� ���� �Լ�
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

// ���ѷα� ȭ�� �Լ�
void prologue_display(ALLEGRO_BITMAP* bitmap)
{
    // ���ȭ�� ������ ���� ũ�⿡ ���߱�
    float w = al_get_bitmap_width(bitmap);
    float h = al_get_bitmap_height(bitmap);
    float dw = BUFFER_W;
    float dh = BUFFER_H;
    al_draw_scaled_bitmap(bitmap,
        0, 0, w, h,
        0, 0, dw, dh,
        0);

}

// ���ѷα� ��Ʈ�� �ε� �Լ�
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

// ���ѷα� ��Ʈ�� ȭ�� ���� �Լ�

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


// ���ѷα� ȭ�� ���������� �ѱ�� �Լ�
void next_slide()
{
    // ������ ��
    if (ps.blink)
        return;

    // �����̵��� ����ŭ �����̵� �ε��� �߰�
    if (ps.curr < ps.end)
        ps.curr++;
    // ������ ȭ���� ���� �ÿ� �� ȭ������ ����
    else {
        ps.blink = true;
    }
}



/* --- main --- */
int main(void)
{
    // �˷��׷� ���̺귯�� �ʱ�ȭ �Լ� ����
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_font_addon(), "font");
    must_init(al_init_image_addon(), "image");


    /*�˷��׷� �̺�Ʈ ��� ������ �̺�Ʈ, Ÿ�̸�, ��Ʈ, �̹��� �ʱ�ȭ �۾�  */
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "builtin font");

    ALLEGRO_BITMAP* bitmap = al_load_bitmap("start_display.png");
    must_init(bitmap, "bitmap");



    // ���÷��� ȭ�� �ʱ�ȭ
    disp_init();

    // ���ѷα� �����̵� �����ϱ�
    load_slides();

    /* �̺�Ʈ �ҽ��� 'disp'(�����ϵ� â)���� �޾ƾ� �� */
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));


    bool running = true;
    bool redraw = true;
    GameState state = STATE_MENU;

    // �˷��׷� ���̺귯�� Ÿ�̸� ���� 
    al_start_timer(timer);

    // ���� �ݺ�
    while (running)
    {
        al_wait_for_event(queue, &event);

        // �̺�Ʈ�� ���� ���̽� ���� ������
        switch (event.type) {
            // ���÷��� ȭ�� ���ῡ ���õ� �̺�Ʈ
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

            // Ű�е� ���� �̺�Ʈ �߻� ��
        case ALLEGRO_EVENT_KEY_DOWN:
            // ALLEGRO_KEY_ESCAPE�� ESCŰ�� ������ ȭ�� ����
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                running = false;

            // SŰ ��ư�� ���� �� 
            else if (event.keyboard.keycode == ALLEGRO_KEY_S && state == STATE_RUNNING)
            {
                next_slide();
                redraw = true;
            }
            break;

            // ���콺 Ŀ�� ��ǥ ���� �̺�Ʈ
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            /* ���÷��� ��ǥ �� ���� ��ǥ�� ��ȯ �� hover ���� */
            float bx = to_buffer_x(event.mouse.x);
            float by = to_buffer_y(event.mouse.y);
            update_hover_all(bx, by);
            redraw = true;

        }
        break;

        // ���콺 Ŭ�� ���� �̺�Ʈ �߻� ��      
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event.mouse.button == 1) {
                float bx = to_buffer_x(event.mouse.x);
                float by = to_buffer_y(event.mouse.y);
                // ���콺 ��ġ�� ��ư ��ġ

                Button* b = hit_button(bx, by);
                if (!b)  break;

                // ���� ���� ��ư
                if (b == &pos1)
                {
                    show_back_only();
                    state = STATE_CHOICE;
                    pos8.active = true;
                    pos9.active = true;
                }

                // �ڷ� ���� ����
                else if (b == &pos2)
                {
                    show_main_menu();
                }

                // ��ŷ , ���� ���� 
                else if ((b == &pos3) || (b == &pos4))
                {
                    show_back_only();
                }

                // ���̵� : ���� ���
                else if (b == &pos5)
                {
                    set_pro_job();

                    game_difficulty = DIFF_HARD;
                    printf("Very Easy~~~~~~~~~~~~ : %d\n", DIFF_EASY);    // �׽�Ʈ
                    state = STATE_RUNNING;

                }
                // ���̵� : �߰� ���

                else if (b == &pos6)
                {
                    set_pro_job();

                    game_difficulty = DIFF_NORMAL;
                    printf("Normal~~~~~~~~~~~~ : %d\n", DIFF_NORMAL);    //
                    state = STATE_RUNNING;

                }
                // ���̵� : ����� ���
                else if (b == &pos7)
                {
                    set_pro_job();
                    game_difficulty = DIFF_HARD;
                    printf("Hard~~~~~~~~~~~~ : %d\n", DIFF_HARD);      // �׽�Ʈ    
                    state = STATE_RUNNING;


                }
                // ���� ���� 1 : DANSO
                else if (b == &pos8)
                {
                    state = STATE_MODE;     // ��� ����
                    job = JOB_DANSO;        // 
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;


                }

                // ���� ���� 2 : ZARUBAN
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

            // �˷��׷� Ÿ�̸� �̺�Ʈ 
        case ALLEGRO_EVENT_TIMER:
            frames++;
            redraw = true;
            break;
        }

        /* ���� �̺�Ʈ ť�� ���� �̺�Ʈ�� �߻� �Ǵ� ��� */
        if (redraw && al_is_event_queue_empty(queue)) {
            /* ���ۿ� �׸��� �� �������ؼ� â�� ���� */
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(30, 30, 30));   /* ��� */
            prologue_display(bitmap);

            switch (state) {

            case STATE_MENU:
                Button_draw(&pos1, font);
                Button_draw(&pos2, font);
                Button_draw(&pos3, font);
                Button_draw(&pos4, font);

                break;

                /*���̵� ���� ȭ��*/
            case STATE_MODE:
                Button_draw(&pos5, font);
                Button_draw(&pos6, font);
                Button_draw(&pos7, font);

                break;

                // ���� ���� ȭ��
            case STATE_CHOICE:
                Button_draw(&pos8, font);   //danso
                Button_draw(&pos9, font);   //zaruban

                break;

                // ���ѷα� ����
            case STATE_RUNNING:
                // �� ȭ��
            {
                if (ps.blink)
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                // ���ѷα� ȭ���� ���������� ���� (���� ������ ����)
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

    /* �Լ� ������ ����(Ÿ�̸�, ��Ʈ, ���÷���, �̺�Ʈ ť)*/
    al_destroy_timer(timer);
    al_destroy_font(font);
    disp_deinit();
    al_destroy_bitmap(bitmap);
    al_destroy_event_queue(queue);

    return 0;
}
#endif