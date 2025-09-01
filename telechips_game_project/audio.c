//======================================================
//                    audio.c
//======================================================
// 2025 telechips allegro game_project
/**
 @file      audio.c
 @brief     게임에 사용되는 오디오 파일 호출 초기화 및 삭제
 @author    김혁, 신동훈, 정명훈, 이재강
*/

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

// 오디오 초기화
void audio_init()
{
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(128);

    sample_normal_shot = al_load_sample("attack1.mp3");
    must_init(sample_normal_shot, "attack1 sample");

    sample_strong_shot = al_load_sample("attack2.mp3");
    must_init(sample_strong_shot, "attack2 sample");

    sample_strong_shot2 = al_load_sample("attack3.mp3");
    must_init(sample_strong_shot2, "attack3 sample");

    sample_ENEMY1 = al_load_sample("enemy1.mp3");
    must_init(sample_strong_shot, "enemy1 sample");

    sample_BOSS1 = al_load_sample("BOSS1_who_are_you.mp3");
    must_init(sample_BOSS1, "boss1_shot");

    sample_BOSS2 = al_load_sample("BOSS1_an.mp3");
    must_init(sample_BOSS2, "boss2_shot");

    bgm_stream = al_load_audio_stream("bgm.mp3", 4, 2048);
    must_init(bgm_stream, "background music");
    al_attach_audio_stream_to_mixer(bgm_stream, al_get_default_mixer());
    al_set_audio_stream_playmode(bgm_stream, ALLEGRO_PLAYMODE_LOOP); // 반복 재생

}

void audio_deinit()
{
    al_destroy_sample(sample_normal_shot);
    al_destroy_sample(sample_strong_shot);
    al_destroy_sample(sample_BOSS1);
    al_destroy_sample(sample_BOSS2);

    if (bgm_stream) {
        al_destroy_audio_stream(bgm_stream);
    }

}