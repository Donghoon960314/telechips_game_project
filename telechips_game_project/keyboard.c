#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT* event)
{
    switch (event->type)
    {
        // 1/60초마다 호출
    case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= ~KEY_SEEN;
        break;

        // 키를 누른 경우
    case ALLEGRO_EVENT_KEY_DOWN:
        key[event->keyboard.keycode] = KEY_SEEN | KEY_DOWN;
        break;

        // 키를 뗀 경우
    case ALLEGRO_EVENT_KEY_UP:
        key[event->keyboard.keycode] &= ~KEY_DOWN;
        break;
    }
}
/*
예시1)
키를 처음 눌렀을 때:
key[LEFT] = KEY_SEEN | KEY_DOWN = 00000011
현재 눌림 (KEY_DOWN)
이번 프레임에 눌림 (KEY_SEEN)

키를 계속 누르고 있는 동안:
key[LEFT] &= ~KEY_SEEN → 00000010
KEY_DOWN만 남음 → 계속 눌려 있는 상태만 유지

키를 뗐을 때:
key[LEFT] &= ~KEY_DOWN → 00000000
눌림 상태 완전히 해제

예시2)
프레임 1 : 키 누름
key[LEFT] = SEEN | DOWN = 3 (00000011)
→ "방향키 입력 시작!" 이벤트 처리

프레임 2 : 계속 누르는 중
key[LEFT] = DOWN = 2 (00000010)  // SEEN 제거됨
→ 계속 눌려있으니 이동 유지

프레임 3 : 여전히 누르는 중
key[LEFT] = DOWN = 2
→ 계속 눌려있으니 이동 유지

프레임 4 : 키를 뗌
key[LEFT] = 0
→ 입력 해제
*/