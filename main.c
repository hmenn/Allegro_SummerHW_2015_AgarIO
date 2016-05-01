/*#######################################################*/
/*#######################################################*/
/*              GEBZE TEKNIK UNIVERSITESI                */
/*             2015 C PROGRAMLAMA YAZ ODEVI              */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                HASAN MEN - 131044009                  */
/*#######################################################*/
/*#######################################################*/
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include "agar.h"


int main()
{
  srand(time(NULL));
  al_init();
  display = al_create_display(screenWidth,screenHeight );
  al_set_window_position(display,200,200);

  bool done=false;
  bool play=true;;
  option_t settings;
  score_t end_game;

  /* Default ayar degerleri */
  settings.skin_counter=0;
  settings.bot_number=15;
  settings.map_counter=1;
  settings.baits_num=1000;
  settings.mouse_coord.x=screenWidth/2;
  settings.mouse_coord.y=screenHeight/2;
  settings.mouse_active=1;
  settings.keyboard_active=0;
  settings.zoom=1.0f; /* kamera oranı */


  /* allegro birimlerinin yüklenmesi */
  al_init_font_addon();
  al_init_ttf_addon();
  al_install_mouse();
  al_install_keyboard();
  al_init_image_addon();
  al_init_primitives_addon();

  /* grafiklerin yüklenmesi */
  bg = al_load_bitmap("files/bg.png");
  bg1 = al_load_bitmap("files/bg_1.png");
  bg2 = al_load_bitmap("files/bg_2.png");
  skin_2ch=al_load_bitmap("skins/2ch.png");
  skin_bait=al_load_bitmap("skins/bait.png");
  skin_cia=al_load_bitmap("skins/cia.png");
  skin_doge=al_load_bitmap("skins/doge.png");
  skin_facepunch=al_load_bitmap("skins/facepunch.png");
  skin_lmao=al_load_bitmap("skins/lmao.png");
  skin_matriarchy=al_load_bitmap("skins/matriarchy.png");
  skin_nazi=al_load_bitmap("skins/nazi.png");
  skin_reddit=al_load_bitmap("skins/reddit.png");
  skin_sanik=al_load_bitmap("skins/sanik.png");
  skin_satanist=al_load_bitmap("skins/satanist.png");
  skin_turkey=al_load_bitmap("skins/turkey.png");
  skin_wojak=al_load_bitmap("skins/wojak.png");

  menu_all= al_load_bitmap("files/menu_sec.png");
  menu_all_arrow= al_load_bitmap("files/menu_sec_arrow.png");
  instrucs = al_load_bitmap("files/instructions.png");
  options = al_load_bitmap("files/options.png");
  credits = al_load_bitmap("files/credits.png");

  timer=al_create_timer(1.0/FPS);
  event_queue = al_create_event_queue();
  font=al_load_font(FONT_FILE,FONT_SIZE,0);
  font24=al_load_font(FONT_FILE,FONT_SIZE24,0);

  /* event liste giriş izinleri */
  al_register_event_source(event_queue,al_get_timer_event_source(timer));
  al_register_event_source(event_queue,al_get_display_event_source(display));
  al_register_event_source(event_queue,al_get_keyboard_event_source());
  al_register_event_source(event_queue,al_get_mouse_event_source());
  al_start_timer(timer);

  /* default grafikler */
  settings.skin=skin_2ch;
  settings.backg=bg;

  /* oyun iterasyonu */
  do
  {
    play = menu(&settings);
    if(play)
    {
      end_game = start_game(&settings);
      show_stats(end_game);
    }

  }while(play);


  /* kullanılan yerlerin yok edilmesi */
  /* free islemi */
  al_destroy_display(display);
  al_destroy_timer(timer);
  al_destroy_font(font);
  al_destroy_font(font24);
  al_destroy_event_queue(event_queue);

  al_destroy_bitmap(options);
  al_destroy_bitmap(instrucs);
  al_destroy_bitmap(menu_all_arrow);
  al_destroy_bitmap(menu_all);

  al_destroy_bitmap(bg);
  al_destroy_bitmap(bg1);
  al_destroy_bitmap(bg2);
  al_destroy_bitmap(skin_2ch);
  al_destroy_bitmap(skin_bait);
  al_destroy_bitmap(skin_cia);
  al_destroy_bitmap(skin_doge);
  al_destroy_bitmap(skin_facepunch);
  al_destroy_bitmap(skin_lmao);
  al_destroy_bitmap(skin_matriarchy);
  al_destroy_bitmap(skin_nazi);
  al_destroy_bitmap(skin_reddit);
  al_destroy_bitmap(skin_sanik);
  al_destroy_bitmap(skin_satanist);
  al_destroy_bitmap(skin_turkey);
  al_destroy_bitmap(skin_wojak);

  return 0;
  }
/* Main.c sonu */
