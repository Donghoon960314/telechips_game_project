#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "common.h"

void audio_init()
{
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(128);

    sample_normal_shot = al_load_sample("shot.flac");
    must_init(sample_normal_shot, "shot sample");
    sample_strong_shot = al_load_sample("shot.flac");
    must_init(sample_strong_shot, "shot sample");
}

void audio_deinit()
{
    al_destroy_sample(sample_normal_shot);
    al_destroy_sample(sample_strong_shot);
}