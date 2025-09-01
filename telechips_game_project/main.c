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
ALLEGRO_FONT* name_font = NULL;   // ?„ì—­ ?•ì˜
ALLEGRO_FONT* title_font = NULL;  // ?„ì—­ ?•ì˜
ALLEGRO_FONT* button_to_rank_title_font = NULL;


//======================================================
//                  MAIN GAME LOOP
//======================================================
int main() {
    must_init(al_init(), "allegro"); // Allegro ?¼ì´ë¸ŒëŸ¬ë¦?ì´ˆê¸°??
    must_init(al_install_keyboard(), "keyboard"); // ?¤ë³´???…ë ¥ ì´ˆê¸°??
    must_init(al_install_mouse(), "mouse");       // ë§ˆìš°???…ë ¥ ì´ˆê¸°??

    // 60fps ?€?´ë¨¸ ?ì„±
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    must_init(al_init_font_addon(), "font");      // ?°íŠ¸ ëª¨ë“ˆ ì´ˆê¸°??
    must_init(al_init_ttf_addon(), "ttf");
    must_init(al_init_image_addon(), "image");    // ?´ë?ì§€ ëª¨ë“ˆ ì´ˆê¸°??
    must_init(al_init_primitives_addon(), "primitives"); // ?„í˜• ëª¨ë“ˆ ì´ˆê¸°??

    
    
    al_init_font_addon();
    al_init_ttf_addon();


    // ?´ë²¤?????ì„±
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    // UI???°íŠ¸ / ë¹„íŠ¸ë§?ë¡œë“œ

    //ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_FONT* font = al_load_ttf_font("OpenSans_Bold.ttf",40,0);
    button_to_rank_title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 60, 0);
    name_font = al_load_ttf_font("BebasNeue-Regular.ttf", 50, 0);
    title_font = al_load_ttf_font("BebasNeue-Regular.ttf", 80, 0);

    printf("DEBUG: name_font=%p, title_font=%p\n", name_font, title_font);

    if (!name_font || !title_font) {
        fprintf(stderr, "?°íŠ¸ ë¡œë“œ ?¤íŒ¨! ?¤í–‰ ê²½ë¡œ??BebasNeue-Regular.ttfê°€ ?ˆëŠ”ì§€ ?•ì¸?˜ì„¸??\n");
        return -1; // ?„ë¡œê·¸ë¨ ë°”ë¡œ ì¢…ë£Œ
    }

    must_init(font, "builtin font");
    ALLEGRO_BITMAP* bitmap = al_load_bitmap("start_display.png");
    must_init(bitmap, "bitmap");

    ALLEGRO_BITMAP* bitmap3 = al_load_bitmap("TUTORIAL.png");
    must_init(bitmap3, "bitmap");

    // ?”ìŠ¤?Œë ˆ??& ?¤ë””??ì´ˆê¸°??
    disp_init(); // ?”ë©´ ì´ˆê¸°??
    audio_init(); // ?¤ë””??ì´ˆê¸°??
    sprites_init(); // ?¤í”„?¼ì´??ìºë¦­?? ?„ì´???? ì´ˆê¸°??
    hud_init(); // HUD ì´ˆê¸°??


    // ?„ë¡¤ë¡œê·¸ ?¬ë¼?´ë“œ ?ì„±
    load_slides();

    // ?´ë²¤???ì— ?´ë²¤???ŒìŠ¤ ?±ë¡
    al_register_event_source(queue, al_get_mouse_event_source());    // ë§ˆìš°??
    al_register_event_source(queue, al_get_keyboard_event_source()); // ?¤ë³´??
    al_register_event_source(queue, al_get_display_event_source(disp)); // ?”ìŠ¤?Œë ˆ??
    al_register_event_source(queue, al_get_timer_event_source(timer));  // ?€?´ë¨¸

    // ê²Œì„ ê°ì²´ ì´ˆê¸°??
    keyboard_init(); // ?¤ë³´???íƒœ ì´ˆê¸°??
    shots_init();    // ì´ì•Œ ì´ˆê¸°??
    enemies_init();  // ??ì´ˆê¸°??
    items_init(); // ?„ì´??ì´ˆê¸°??
    stage_image_pop_init(); // ?¤í…Œ?´ì? ë°°ë„ˆ ì´ˆê¸°??

    //======================================================
    //                ê²Œì„ ?íƒœ ë³€??ì´ˆê¸°??
    //======================================================
    frames = 0;          // ?„ë ˆ??ì¹´ìš´??
    score = 0;           // ?ìˆ˜ ì´ˆê¸°??
    time_limit = 60;    // ?œí•œ ?œê°„
    time_left = time_limit;

    char player_name[32]; // Àü¿ªº¯¼ö¿¡ ÇÃ·¹ÀÌ¾î ÀÌ¸§
    int min, sec;         // ºĞ,ÃÊ(°ÔÀÓ±â·Ï)
    bool game_end_ranking = false; //·©Å·Ã¢À¸·Î Á¢±ÙÇÏ´Â°ÍÀÌ °ÔÀÓ ³¡³ª°í ÀÎÁö/ ¹è³Ê¸¦ Å¬¸¯ÇØ¼­ ÀÎÁö È®ÀÎÇÒ ¼ö ÀÖ°Ô²û.

    bool done = false;   // ê²Œì„ ì¢…ë£Œ ?¬ë?
    bool redraw = true;  // ?”ë©´ ?¤ì‹œ ê·¸ë¦´ì§€ ?¬ë?
    GameState state = STATE_MENU; // ì²˜ìŒ?ëŠ” UI ë©”ë‰´ ?íƒœ
    ALLEGRO_EVENT event;

    // ?€?´ë¨¸ ?œì‘
    al_start_timer(timer);
    srand((unsigned int)time(NULL));

    //======================================================
    //                  ë©”ì¸ ë£¨í”„ ?œì‘
    //======================================================
    while (1)
    {
        // ?´ë²¤???€ê¸?
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
            //======================================================
            //                    UI ê´€??ì²˜ë¦¬
            //======================================================
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            /* ?”ìŠ¤?Œë ˆ??ì¢Œí‘œ ??ë²„í¼ ì¢Œí‘œë¡?ë³€????hover ê°±ì‹  */
            float bx = to_buffer_x(event.mouse.x);
            float by = to_buffer_y(event.mouse.y);
            update_hover_all(bx, by);
            redraw = true;
        }
        break;

        // ë§ˆìš°???´ë¦­ ê´€???´ë²¤??ë°œìƒ ??
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event.mouse.button == 1) {
                float bx = to_buffer_x(event.mouse.x);
                float by = to_buffer_y(event.mouse.y);

                // ë§ˆìš°???„ì¹˜ê°€ ë²„íŠ¼ ?„ì¹˜
                Button* b = hit_button(bx, by);
                if (!b)  break;

                // ê²Œì„ ?œì‘
                if (b == &pos1)
                {
                    show_back_only();
                    state = STATE_CHOICE;
                    pos8.active = true;
                    pos9.active = true;
                }

                // ?¤ë¡œ ê°€ê¸?
                else if (b == &pos2)
                {
                    show_main_menu();
                    state = STATE_MENU;
                }

                // ??‚¹,ê²Œì„ ?¤ëª…
                else if (b == &pos3)
                {
                    show_back_only();
                    state = STATE_TUTORIAL;
                }
                else if (b == &pos4)
                {
                    state = STATE_RANKING;
                }

                // ?œì´?? ?¬ìš´ ëª¨ë“œ
                else if (b == &pos5)
                {
                    set_pro_job();
                    game_difficulty = DIFF_EASY;
                    state = STATE_PROLOGUE;

                }

                // ?œì´?? ì¤‘ê°„ ëª¨ë“œ
                else if (b == &pos6)
                {
                    set_pro_job();
                    game_difficulty = DIFF_NORMAL;
                    state = STATE_PROLOGUE;
                }

                // ?œì´?? ?´ë ¤??ëª¨ë“œ
                else if (b == &pos7)
                {
                    set_pro_job();
                    game_difficulty = DIFF_HARD;
                    state = STATE_PROLOGUE;
                }

                // ì§ì—… ? íƒ 1: Tanjiro
                else if (b == &pos8)
                {
                    state = STATE_MODE;
                    job_type = JOB_TYPE_1;
                    pos5.active = true;
                    pos6.active = true;
                    pos7.active = true;
                }

                // ì§ì—… ? íƒ 2: Rengoku
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
                // ??‚¹ ?”ë©´???ŒëŠ” ?´ë–¤ ?¤ë? ?ŒëŸ¬??ë©”ë‰´ë¡?ë³µê?
                state = STATE_MENU;
                game_end_ranking = false;   // ê²Œì„ ?ë‚˜???¤ì–´??ê±°ë‹ˆê¹??˜ì´?¼ì´??O
                stage_num = 1; //ê²Œì„?œì‘ -> ì¢…ë£Œ -> ??‚¹?…ë ¥ -> esc -> ?¤ì‹œ ê²Œì„?œì‘ ? ë•Œ ?„ìš”??
                show_main_menu();   // ë©”ë‰´ ë²„íŠ¼ ?¤ì‹œ ?œì„±??
                redraw = true;
            }
            else if (state == STATE_PROLOGUE && event.keyboard.keycode == ALLEGRO_KEY_S) {
                next_slide(); // ?„ë¡¤ë¡œê·¸ ?˜ê¸°ê¸?
                redraw = true;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                done = true;
            }
            break;

            // ?Œë ˆê·¸ë¡œ ?€?´ë¨¸ ?´ë²¤??
        case ALLEGRO_EVENT_TIMER:
            if (state == STATE_RUNNING) {
                // ê²Œì„ ë¡œì§ ?…ë°?´íŠ¸ (?¤í…Œ?´ì? ?ˆì¼ ?Œë§Œ)
                shots_update();
                player_update();
                enemies_update();
                items_update();
                hud_update();
                update_shake();

                // ë§??„ë ˆ?„ë§ˆ???œê°„ ê°ì†Œ
                if (frames % 60 == 0 && time_left > 0)
                    time_left--;

                // ?œê°„ ì§€?˜ë©´ ê²Œì„ ?¤ë²„
                if (time_left <= 0)
                    player.hp = 0;

                // ¸Å ÇÁ·¹ÀÓ¸¶´Ù stage_reset()À» °è¼Ó È£ÃâÇÏ¸é¼­ ½ºÅ×ÀÌÁö Á¾·á È®ÀÎ
                if (stage_reset() > 3) { //stage_resetÀÇ return °ª == stage_num
                    rank_name_open(al_get_timer_count(timer), player_name, &min, &sec); //½ºÅ×ÀÌÁö Á¾·áÈÄ ·©Å· Ã¢¿¡¼­ ÀÌ¸§ ÀÔ·Â
                    game_end_ranking = true;   //game_end_rankingº¯¼ö¸¦ true·Î ¹Ù²Ù²¿  RANKING_STATE·Î ³Ñ¾î°¬À»¶§ÀÇ RANKÈ­¸éÀ» ¶ç¿ò.
                    
                    al_flush_event_queue(queue); //¸ğµç ÀÌº¥Æ®¸¦ ºñ¿ì°í ´ÙÀ½ ½ºÅ×ÀÌÁö ÁØºñ

                    state = STATE_RANKING; //RANKIHG STATE ÁøÀÔ

                    disp_pre_draw();
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    
                    disp_post_draw();

                    redraw = false;

                    continue;
                }

                // ì²?120?„ë ˆ???™ì•ˆë§??Œí™˜ ?ˆìš©
                if (frames > 120) {
                    spawn_enabled = false;
                }

                // ESC ??-> ê²Œì„ ì¢…ë£Œ
                if (key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                // R ??-> ê²Œì„ ë¦¬ìŠ¤?€??(?Œë ˆ?´ì–´ ì£½ì—ˆ???Œë§Œ)
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

                    // ?¤ì‹œ ?¤í…Œ?´ì? 1ë¡??œì‘
                }
            }

            frames++;
            redraw = true;
            break;

            // ì°??«ê¸° ?´ë²¤??
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        // ì¢…ë£Œ ì¡°ê±´ ?•ì¸
        if (done)
            break;


        // ?¤ë³´???…ë ¥ ?…ë°?´íŠ¸
        keyboard_update(&event);

        // ?”ë©´ ê·¸ë¦¬ê¸?
        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw(); // ?”ë¸” ë²„í¼ ì¤€ë¹?
            al_clear_to_color(al_map_rgb(0, 0, 0)); // ?”ë©´ ?´ë¦¬??(ê²€?•ìƒ‰)

            // UI ?íƒœ???°ë¥¸ ?”ë©´ ?œì‹œ
            switch (state)
            {
            case STATE_MENU:
                prologue_display(bitmap);
                Button_draw(&pos1, font);
                Button_draw(&pos2, font);
                Button_draw(&pos3, font);
                Button_draw(&pos4, font);
                break;

                /*?œì´??? íƒ ?”ë©´*/
            case STATE_MODE:
                prologue_display(bitmap);
                Button_draw(&pos5, font);
                Button_draw(&pos6, font);
                Button_draw(&pos7, font);
                break;

                // ì§ì—… ? íƒ ?”ë©´
            case STATE_CHOICE:
                prologue_display(bitmap);
                Button_draw(&pos8, font); //danso
                Button_draw(&pos9, font); //zaruban
                break;

                // ?„ë¡¤ë¡œê·¸ ì§„ì…
            case STATE_PROLOGUE:
                if (!ps.blink)
                {
                    if (ps.curr <= ps.end)
                    {
                        ALLEGRO_BITMAP* bitmap2 = prologue_List[ps.curr];
                        prologue_display(bitmap2); // ?„ë¡¤ë¡œê·¸ ?¬ë¼?´ë“œ ?œì‹œ
                    }
                }
                else
                {
                    state = STATE_RUNNING;  // ?„ë¡¤ë¡œê·¸ ????ê²Œì„ ?œì‘
                    frames = 0; //8.31 ?˜ì •
                    time_left = time_limit;     //8.31 ?˜ì •
                    keyboard_init(); //ì§€ë§˜ë?ë¡??€ì§ì´?”ê±° ì´ˆê¸°??8.31
                    stage_font(0);          // Stage1 ë°°ë„ˆ ?„ì?
                    
                    al_stop_timer(timer); //ê²Œì„ ì¢…ë£Œ???¬ì‹œ???œê°„ ?¤ë¥˜ ?˜ì •//
                    al_set_timer_count(timer, 0);
                    al_start_timer(timer);

                    stage_init();
                    player_init();   // ?Œë ˆ?´ì–´ ì´ˆê¸°??
                }
                break;

                // ?„ë¡¤ë¡œê·¸ê°€ ?ë‚¬????-> ë³¸ê²©?ì¸ ê²Œì„ ?”ë©´
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

            case STATE_RANKING: //·©Å· È­¸é ÁøÀÔ½Ã ·©Å·Ãâ·ÂÇÏ´Â ÇÔ¼ö È£Ãâ(°°Àº ÇÔ¼ö, ÀÎÀÚ¸¸ ´Ù¸£°Ô)
                if (game_end_ranking)
                {
                    print_ranking_table(player_name, min, sec, END_TO_RANK); //°ÔÀÓÀÌ ³¡³ª°í ·©Å·È­¸é ÁøÀÔ½Ã 
                }
                else
                {
                    print_ranking_table("", 0, 0, BUTTON_TO_RANK); //¹öÆ°´­·¯¼­ ·©Å·È­¸é ÁøÀÔ 
                }
                break;
            
            case STATE_TUTORIAL:
                prologue_display(bitmap);
                Tutorial_display(bitmap3);
                Button_draw(&pos2, font);
                break;
            }
            disp_post_draw(); // ?”ë¸” ë²„í¼ ?”ë©´??ì¶œë ¥
            redraw = false;
        }
    }

    /* ?¨ìˆ˜ ë§ˆë¬´ë¦?? ì–¸(?€?´ë¨¸, ?°íŠ¸, ?”ìŠ¤?Œë ˆ?? ?´ë²¤????*/
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
    