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
#include <stdlib.h> // abs()
#include <math.h>
#include "agar.h"
#include <assert.h>

//#define DEBUG


void change_camera(option_t *settings,float x,float y)
{
  /* aslında kamerayı (0,0) da tutmaya calısırız */
  settings->cam_coord.x = x - screenWidth/2;
  settings->cam_coord.y = y - screenHeight/2;

/* ekranın farklı bolgelerine gecisler saglanır */
  if(settings->cam_coord.x < 0) settings->cam_coord.x = 0;
  if(settings->cam_coord.y < 0) settings->cam_coord.y = 0;
  if(settings->cam_coord.x > worldWidth - screenWidth )
    settings->cam_coord.x = worldWidth -screenWidth ;
  if(settings->cam_coord.y > worldHeight - screenHeight)
    settings->cam_coord.y = worldHeight - screenHeight;
}

void update_scale(const float radius,float *zoom)
{
  /* yarıcap degerleri ekran boyutuna gore degistirilebilir */
  if(radius>500)
    *zoom=0.5;
  else if(radius>400)
    *zoom=0.6;
  else if(radius>300)
    *zoom=0.7;
  else if(radius>200)
    *zoom=0.8;
  else if(radius>100)
    *zoom=0.9;
}


int attack_bait(agar_t baits[],int size,agar_t *my)
{
  int i=0,j=0;
  int eaten=0;
    for(i=0;i<size;++i)
      if(baits[i].alive)
        if(can_eat(*my,baits[i]))
        {
          check_baits(&baits[i]);
          my->r+=0.2;
          my->mass+=1;
          eaten++;
        }
  /* yarıcap buyukse aynı anda 1den fazla yiyebilir */
  return eaten;
}

int attack_bots(agar_t bots[],int size,agar_t *player)
{
  int i=0;
  int eaten=0;
  for(i=0;i<size;++i)
    if(bots[i].alive)
      if(can_eat(*player,bots[i])) /* boyu yiyebilirmi*/
      {
        bots[i].alive=false;
        player->r+=bots[i].r*0.2;
        player->mass+=bots[i].mass;
        eaten++;
      }
  return eaten;
}


bool can_eat(agar_t my,agar_t bait)
{
  bool eat=false;
  /* sınırlar içinde mi*/
  if(sqrt(pow((my.coord.x-bait.coord.x),2) + pow((my.coord.y-bait.coord.y),2)) < my.r )
    if(my.r>bait.r+5) /* karsılastırma */
      eat=true;
  return eat;
}


void create_baits(agar_t baits[],int bait_num)
{
  int i;

  for(i=0;i<bait_num;++i)
  {
    baits[i].coord.x=rand()%worldWidth;
    baits[i].coord.y=rand()%worldHeight;
    baits[i].r=10.0;
    baits[i].alive=true;
    baits[i].color=al_map_rgb(rand()%256,rand()%256,rand()%256);
  }
}

void check_baits(agar_t *baits)
{
  baits->coord.x=rand()%worldWidth;
  baits->coord.y=rand()%worldHeight;
  baits->color=al_map_rgb(rand()%256,rand()%256,rand()%256);
}

void draw_baits(agar_t baits[],int bait_num)
{
  int i;

  for(i=0;i<bait_num;++i)
  {
    if(baits[i].alive)
      al_draw_filled_circle(baits[i].coord.x,baits[i].coord.y,baits[i].r,baits[i].color);
  }
}

score_t start_game(option_t *settings)
{
  /* degiskenler */
  bool done=false;
  float movespeed=5;
  Direction_t dir=-1;
  bool play=true;
  bool draw=false;
  score_t score;
  int arr_size=100;
  int i;

  agar_t *baits=(agar_t *)malloc(sizeof(agar_t)*(settings->baits_num));
  agar_t player[100]={{rand()%worldWidth,rand()%worldHeight,35,40,1,al_map_rgb(0,0,0),0},0};
  agar_t *bots=(agar_t *)malloc(sizeof(agar_t)*(settings->bot_number));

  /* yem ve botları random olusturduk */
  create_baits(baits,settings->baits_num);
  create_bots(bots,settings->bot_number);

  /* sadece tek agar ile oyuna baslar */
  for(i=1;i<arr_size;i++)
  {
    player[i].alive=false;
  }
  /* ilk degerler verildi */
  score.eaten_bot=0;
  score.eaten_bait=0;

  /* EVENTIN BASLAMASI */
  ALLEGRO_EVENT ev;
  while(!done && player[0].alive) /* OLUNCE OYUN BİTER */
  {
    /* ekranın güncellenmesi, cizimlerin yapılması */
    if(draw && al_is_event_queue_empty(event_queue))
    {
      al_clear_to_color(al_map_rgb(0,0,0));
      al_draw_bitmap(settings->backg,0,0,0); /* arka plan*/

      /* HAREKETLER */
      move_bots(bots,settings->bot_number,baits,settings->baits_num,player,arr_size);

      if(settings->mouse_active)
        move_accor_mouse(player,arr_size,settings);
      else if(settings->keyboard_active)
        move(player,dir,arr_size);

        /* split varsa sure kontrolu ile tekrardan birlesim*/
        combine_agar(player,arr_size);

        /* teker teker yemeler kontrol edilir*/
      for(i=0;i<arr_size;++i)
        if(player[i].alive)
        {
          score.eaten_bait+=attack_bait(baits,settings->baits_num,&player[i]);
          score.eaten_bot+=attack_bots(bots,settings->bot_number,&player[i]);
        }


      /* hareket hızı duzenleme */
      update_speed(player,arr_size);
      update_speed(bots,settings->bot_number);

      /*############# UPDATE CAMERA - ZOOM ###############*/
      update_scale(player[0].r,&settings->zoom);
      change_camera(settings,player[0].coord.x,player[0].coord.y);
      al_identity_transform(&camera);
      al_translate_transform(&camera,-(settings->cam_coord.x),-(settings->cam_coord.y));
      al_scale_transform(&camera,settings->zoom,settings->zoom);
      al_use_transform(&camera);


      /* kontrol amaclı bazı koodrinatlar */
#ifdef DEBUG
      printf("x=%.2f -- y=%.2f -- r=%.2f-- Cx=%.2f -- Cy=%.2f\n",player[0].coord.x,player[0].coord.y,player[0].r,(settings->cam_coord.x),(settings->cam_coord.y));
      printf("M_X=%.2f / M_Y=%.2f\n",settings->mouse_coord.x,settings->mouse_coord.y);
      al_draw_textf(font,al_map_rgb(0,0,0),settings->cam_coord.x,settings->cam_coord.y,ALLEGRO_ALIGN_LEFT,"gx :%.2f - g2:%.2f - mx:%.2f - my:%.2f - ax:%.2f - ay:%.2f",go_with_mouse.x,go_with_mouse.y,player[0].coord.x,player[0].coord.y,settings->cam_coord.x,settings->cam_coord.y);
#endif

      /* cizimlerin yapılması ve bufferin ekrana yansıtılması */
      draw_baits(baits,settings->baits_num);
      draw_agar(player,settings,arr_size);
      draw_bots(bots,settings->bot_number);
      al_draw_textf(font,al_map_rgb(0,0,0),settings->cam_coord.x,settings->cam_coord.y,ALLEGRO_ALIGN_LEFT,"Mass :%d - R:%.2f",player[0].mass,player[0].r);

      al_flip_display(); /* otomatik double buffering yapar */
      draw=false;
    }

    al_wait_for_event(event_queue,&ev);
    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)/* carpıya basarsa oyun biter*/
      done=true;
    else if(ev.type ==ALLEGRO_EVENT_KEY_DOWN) /* klavyeden girdiler event listesine eklenir*/
    {
      if(ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
        dir=DOWN;
      else if(ev.keyboard.keycode == ALLEGRO_KEY_UP)
        dir=UP;
      else if(ev.keyboard.keycode == ALLEGRO_KEY_RIGHT)
        dir=RIGHT;
      else if(ev.keyboard.keycode == ALLEGRO_KEY_LEFT)
        dir=LEFT;
      else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
        split_agar(player,100,settings,dir);
      else if(ev.keyboard.keycode == ALLEGRO_KEY_W)
        settings->zoom+=0.05;
      else if(ev.keyboard.keycode == ALLEGRO_KEY_S)
        settings->zoom-=0.05;
    }
    else if(ev.type==ALLEGRO_EVENT_MOUSE_AXES) /* mouse koordinatlarının depolanması*/
    {
      settings->mouse_coord.x = ev.mouse.x;
      settings->mouse_coord.y = ev.mouse.y;
    }
    else if (ev.type == ALLEGRO_EVENT_TIMER)/* 60FPS ile guncelleme olur*/
    {
        draw=true;
    }
  }
  /* scoreun return edilmesi */
  score.radius = player[0].r;
  score.mass = player[0].mass;
  return score;
}

void move(agar_t *agar,Direction_t dir,int size)
{
  int i;
  for(i=0;i<size;i++)
    switch(dir)
      {
        case DOWN:{
          if(agar[i].coord.y<worldHeight)
            agar[i].coord.y+=agar[i].speed*2;
        }  break;
        case UP:{
          if(agar[i].coord.y>=0)
            agar[i].coord.y-=agar[i].speed*2;
        }  break;
        case LEFT:{
          if(agar[i].coord.x>0)
            agar[i].coord.x-=agar[i].speed*2;
        } break;
        case RIGHT:{
          if(agar[i].coord.x<worldWidth)
            agar[i].coord.x+=agar[i].speed*2;
        } break;
        default: break;
      }
}

void move_accor_mouse(agar_t *player,int size,const option_t *settings)
{
  int i;

  for(i=0;i<size;i++)
    if(player[i].coord.x>0 && player[i].coord.x <worldWidth && player[i].coord.y>0 && player[i].coord.y<worldWidth)
    {
      player[i].coord.x += ((settings->mouse_coord.x+settings->cam_coord.x - player[i].coord.x)*(player[i].speed/225.0));
      player[i].coord.y += ((settings->mouse_coord.y+settings->cam_coord.y - player[i].coord.y)*(player[i].speed/225.0));
    }
}

bool menu(option_t *settings)
{
  int counter=0; /* menude hangi secime girecegi */
  int done=false;
  int draw=true;
  menu_choice choose1=-1; /* otomatik secim yapmaması için -1 verildi*/
  bool play=true;
  ALLEGRO_EVENT menu_ev;
  settings->zoom=1.0f; /* oyun esnasinda degisimi 0la */

  while(!done)
  {
    if(draw && al_is_event_queue_empty(event_queue))
    {
      draw=false;

      /* buyuk agar olup menuye donunce zoom sıfırlanmalı */
      /* MENUDE KAMERA ZOOMU DEGİSİYOR ONU SIFIRLAMAK LAZIM */
      al_identity_transform(&camera);
      al_translate_transform(&camera,0,0);
      al_scale_transform(&camera,settings->zoom,settings->zoom);
      al_use_transform(&camera);

      al_clear_to_color(al_map_rgb(255,255,255));
      al_draw_bitmap(menu_all,0,0,0); /* menuyu ciz */
      al_draw_bitmap(menu_all_arrow,settings->menu_arrow.x,settings->menu_arrow.y,0); /* secim okları */
#ifdef DEBUG
      al_draw_textf(font,al_map_rgb(255,0,0),0,15,0,"x->%5.2f y->%5.2f",settings->mouse_coord.x,settings->mouse_coord.y);
      al_draw_textf(font,al_map_rgb(255,0,0),0,50,0,"Counter->%d",counter);
#endif
      al_flip_display();
    }

    al_wait_for_event(event_queue,&menu_ev);
    if(menu_ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)/* carpıya basarsa tamamen kapa*/
    {
      done=true;
      play = false;
    }

    else if(menu_ev.type == ALLEGRO_EVENT_KEY_DOWN)
    {
      if(menu_ev.keyboard.keycode==ALLEGRO_KEY_UP)
      {
        if(counter<=0)
          counter=PLAY;
        else --counter;
      }
      else if(menu_ev.keyboard.keycode==ALLEGRO_KEY_DOWN)
      {
        if(counter>=EXIT)
          counter=EXIT;
        else ++counter;
      }
      else if(menu_ev.keyboard.keycode==ALLEGRO_KEY_Q)
        done=true;
      else if(menu_ev.keyboard.keycode==ALLEGRO_KEY_ENTER)
      {
        choose1=counter;
      }
    }
    /* mouse aktif menu secimi */
    else if(menu_ev.type == ALLEGRO_EVENT_MOUSE_AXES)
    {
      settings->mouse_coord.x = menu_ev.mouse.x;
      settings->mouse_coord.y = menu_ev.mouse.y;

      if(is_mause_in(175,20,600,95,settings->mouse_coord))
        counter=PLAY;
      else if(is_mause_in(175,115,600,185,settings->mouse_coord))
        counter=OPTION;
      else if(is_mause_in(175,200,600,250,settings->mouse_coord))
        counter=INSTRACTIONS;
      else if(is_mause_in(175,295,600,330,settings->mouse_coord))
        counter=CREDITS;
      else if(is_mause_in(175,370,600,400,settings->mouse_coord))
        counter=EXIT;
    }
    else if(menu_ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) // butona basıldımı
    {
      if(menu_ev.mouse.button & 1) /* belirli alan icinde secim yapma */
      {
        if(is_mause_in(175,20,600,95,settings->mouse_coord))
          choose1=PLAY;
        else if(is_mause_in(175,115,600,185,settings->mouse_coord))
          choose1=OPTION;
        else if(is_mause_in(175,200,600,250,settings->mouse_coord))
          choose1=INSTRACTIONS;
        else if(is_mause_in(175,295,600,330,settings->mouse_coord))
          choose1=CREDITS;
        else if(is_mause_in(175,370,600,400,settings->mouse_coord))
          choose1=EXIT;
      }
    }
    else if(menu_ev.type == ALLEGRO_EVENT_TIMER)
      draw=true;

    /* sayaca gore oklarn yeri belirlenir */
    switch(counter)
    {
      case PLAY: settings->menu_arrow.y=0; break;
      case OPTION: settings->menu_arrow.y=90; break;
      case INSTRACTIONS: settings->menu_arrow.y=170; break;
      case CREDITS: settings->menu_arrow.y=250; break;
      case EXIT: settings->menu_arrow.y=325; break;
      default: break;
    }
    switch(choose1)
    {
      case PLAY: {done=true; play=true;} break;
      case OPTION: menu_options(settings); break;
      case INSTRACTIONS: menu_instructions(settings); break;
      case CREDITS: menu_credits(settings); break;
      case EXIT:{done=true; play=false;} break;
      default: break;
    }
    choose1=-1; /* secenegi sifirla */
  }

  return play; /* oyuna baslama durumu return edilir */
}

void menu_instructions(option_t *settings)
{
  int counter=0;
  int done=false;
  int draw=true;
  ALLEGRO_EVENT ev;

  while(!done)
  {
    if(draw && al_is_event_queue_empty(event_queue))
    {
      draw=false;
      al_clear_to_color(al_map_rgb(15,200,200));
      al_draw_bitmap(instrucs,0,0,0);
#ifdef DEBUG
      al_draw_textf(font,al_map_rgb(255,0,0),0,15,0,"x->%5.2f y->%5.2f",settings->mouse_coord.x,settings->mouse_coord.y);
#endif
      al_flip_display();
    }

    al_wait_for_event(event_queue,&ev);
    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
      done=true;
    else if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)/* backspace ile menuye don*/
      done=true;
    else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES)
    {
        settings->mouse_coord.x=ev.mouse.x;
        settings->mouse_coord.y=ev.mouse.y;
    }
    else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) // butona basıldımı
    {
      if(ev.mouse.button & 1 && is_mause_in(640,570,800,600,settings->mouse_coord))
          done=true;
    }
    else if(ev.type == ALLEGRO_EVENT_TIMER)
      draw=true;
  }
}

bool is_mause_in(int x1,int y1,int x2,int y2,coordinat_t mouse)
{
  /* belirli alan icinde ise true return et*/
  if (mouse.x >= x1 && mouse.x <= x2 && mouse.y >= y1 && mouse.y <= y2)
     return true;
  else
      return false;
}

void menu_options(option_t *settings)
{
  int counter=0;
  int done=false;
  int draw=false;
  ALLEGRO_EVENT ev;

  while(!done) /* event basladı */
  {
    if(draw) /* cizimler yapıldı*/
    {
      draw=false;

      al_clear_to_color(al_map_rgb(255,255,255));
      al_draw_bitmap(options,0,0,0);
#ifdef DEBUG
      //al_draw_textf(font,al_map_rgb(255,0,0),500,380,0,"%d",settings->map_counter);
      al_draw_textf(font,al_map_rgb(255,0,0),0,15,0,"x->%5.2f y->%5.2f",settings->mouse_coord.x,settings->mouse_coord.y);
#endif
      al_draw_textf(font,al_map_rgb(255,0,0),300,380,0,"%d",settings->bot_number);

      if(settings->mouse_active)
        al_draw_textf(font,al_map_rgb(255,0,0),235,195,ALLEGRO_ALIGN_CENTER,"X");
      else al_draw_textf(font,al_map_rgb(255,0,0),255,245,ALLEGRO_ALIGN_CENTER,"X");

      /* skin secimini ciz ve kaydet */
      switch (settings->skin_counter) {
        case 0:{al_draw_scaled_bitmap(skin_2ch,0,0,512,512,207,107,60,60,0); settings->skin=skin_2ch;} break;
        case 1:{al_draw_scaled_bitmap(skin_facepunch,0,0,512,512,207,107,60,60,0); settings->skin=skin_facepunch;} break;
        case 2:{al_draw_scaled_bitmap(skin_bait,0,0,512,512,207,107,60,60,0); settings->skin=skin_bait;} break;
        case 3:{al_draw_scaled_bitmap(skin_cia,0,0,512,512,207,107,60,60,0); settings->skin=skin_cia;} break;
        case 4:{al_draw_scaled_bitmap(skin_doge,0,0,512,512,207,107,60,60,0); settings->skin=skin_doge;} break;
        case 5:{al_draw_scaled_bitmap(skin_lmao,0,0,512,512,207,107,60,60,0); settings->skin=skin_lmao;} break;
        case 6:{al_draw_scaled_bitmap(skin_matriarchy,0,0,512,512,207,107,60,60,0); settings->skin=skin_matriarchy;} break;
        case 7:{al_draw_scaled_bitmap(skin_nazi,0,0,512,512,207,107,60,60,0); settings->skin=skin_nazi;} break;
        case 8:{al_draw_scaled_bitmap(skin_reddit,0,0,512,512,207,107,60,60,0); settings->skin=skin_reddit;} break;
        case 9:{al_draw_scaled_bitmap(skin_sanik,0,0,512,512,207,107,60,60,0); settings->skin=skin_sanik;} break;
        case 10:{al_draw_scaled_bitmap(skin_satanist,0,0,512,512,207,107,60,60,0); settings->skin=skin_satanist;} break;
        case 11:{al_draw_scaled_bitmap(skin_turkey,0,0,512,512,207,107,60,60,0); settings->skin=skin_turkey;} break;
        case 12:{al_draw_scaled_bitmap(skin_doge,0,0,512,512,207,107,60,60,0); settings->skin=skin_wojak;} break;
      }
      /* kucuk olcekte harita ve skinler basılır*/
      switch (settings->map_counter) {
        case 1:{al_draw_scaled_bitmap(bg,20,20,90,80,270,280,70,60,0); settings->backg = bg;} break;
        case 2:{al_draw_scaled_bitmap(bg1,20,20,90,80,270,280,70,60,0); settings->backg = bg1;} break;
        case 3:{al_draw_scaled_bitmap(bg2,0,0,4000,4000,270,280,70,60,0); settings->backg = bg2;} break;
      }
      al_flip_display();
    }


    al_wait_for_event(event_queue,&ev);

    switch (ev.type) {
      case ALLEGRO_EVENT_DISPLAY_CLOSE: done=true; break;
      case ALLEGRO_EVENT_MOUSE_AXES:
      {
        settings->mouse_coord.x=ev.mouse.x;
        settings->mouse_coord.y=ev.mouse.y;
      }break;
      case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      {
        if(ev.mouse.button & 1 && is_mause_in(625,555,800,600,settings->mouse_coord))
            done=true;
        else if(ev.mouse.button & 1 && is_mause_in(360,380,395,410,settings->mouse_coord))
        {
            if(settings->bot_number<20)
              settings->bot_number++;
        }
        else if(ev.mouse.button & 1 && is_mause_in(230,380,265,410,settings->mouse_coord))
        {
            if(settings->bot_number>0)
              settings->bot_number--;
        }
        else if(ev.mouse.button & 1 && is_mause_in(160,125,190,150,settings->mouse_coord))
        {
          if(settings->skin_counter>0)
            settings->skin_counter--;
        }
        else if(ev.mouse.button & 1 && is_mause_in(285,125,315,150,settings->mouse_coord))
        {
          if(settings->skin_counter<12)
            settings->skin_counter++;
        }
        else if(ev.mouse.button & 1 && is_mause_in(365,300,395,320,settings->mouse_coord))
        {
          if(settings->map_counter<3)
            settings->map_counter++;
        }
        else if(ev.mouse.button & 1 && is_mause_in(222,300,250,320,settings->mouse_coord))
        {
          if(settings->map_counter>1)
            settings->map_counter--;
        }
        else if(ev.mouse.button & 1 && is_mause_in(223,188,250,210,settings->mouse_coord))
        {
          /* mause kapa klavye ac */
          if(!settings->mouse_active)
          {
            settings->mouse_active=true;
            settings->keyboard_active=false;
          }
        }
        else if(ev.mouse.button & 1 && is_mause_in(240,235,270,260,settings->mouse_coord))
        {
          /* klavye ac mouse kapa*/
          if(!settings->keyboard_active)
          {
            settings->mouse_active=false;
            settings->keyboard_active=true;
          }
        }
      }break;
      case ALLEGRO_EVENT_TIMER: draw=true; break;
    }
    if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
      done=true;
  }
}

void menu_credits(option_t *settings)
{
  int counter=0;
  int done=false;
  int draw=true;
  ALLEGRO_EVENT ev;

  while(!done)
  {
    if(draw && al_is_event_queue_empty(event_queue))
    {
      draw=false;
      al_clear_to_color(al_map_rgb(15,200,200));
      al_draw_bitmap(credits,0,0,0);
#ifdef DEBUG
      al_draw_textf(font,al_map_rgb(255,0,0),0,15,0,"x->%5.2f y->%5.2f",settings->mouse_coord.x,settings->mouse_coord.y);
#endif
      al_flip_display();
    }

    al_wait_for_event(event_queue,&ev);
    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
      done=true;
    else if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)/* backspace ile menuye don*/
      done=true;
    else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES)
    {
        settings->mouse_coord.x=ev.mouse.x;
        settings->mouse_coord.y=ev.mouse.y;
    }
    else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) // butona basıldımı
    {
      if(ev.mouse.button & 1 && is_mause_in(640,570,800,600,settings->mouse_coord))
          done=true;
    }
    else if(ev.type == ALLEGRO_EVENT_TIMER)
      draw=true;
  }

}

void combine_agar(agar_t *agar,int size)
{
  int i;
  int combine_with=0;

  for(i=1;i<size;i++)
  {
    if(agar[i].alive)
    {
      agar[i].time-=0.0001;/* time guncelle*/
      if(agar[i].time <= 0) /* zaman tukenirse birlestir*/
      {
        /* asil agar yasamıosa 0la birles */
        if(agar[agar[i].whose_twin].alive)
          combine_with=agar[i].whose_twin;
        else combine_with=0;

        agar[combine_with].r+=agar[i].r; /* yarıcapı ekle*/
        agar[combine_with].mass+=agar[i].mass; /* toplam puanıda ekle */
        agar[i].alive = false;
      }
    }
  }
}

void split_agar(agar_t agar[],int size,const option_t *settings,Direction_t keyboard)
{
  int i,j;
  int counter=0; /* sadece 1tane kopya olusturması için */
  /* yoksa tüm array aynı kopya ile dolar */

  /* ilk agarı belirle */
  agar[0].is_twin=false;

  for(i=0;i<size;++i)
  {
    if(agar[i].alive && !agar[i].is_twin && agar[i].r > 40)
    {
        for(j=0;j<size;++j)
        {
          if(!agar[j].alive && !counter) /* kopyayı il bosluga koy ve kopyalama bitsin*/
          {
            counter=true;
            agar[j].alive=true;
            agar[j].r = agar[i].r * 0.4;
            agar[j].mass = agar[i].mass * 0.4;
            agar[j].speed = 5;
            agar[j].is_twin=true; /* parcalar hep ikiz olsun*/
            agar[j].whose_twin=i;
            agar[j].time=300+ agar[j].r*100; /* yaklası 30sn + yarıcapa gore sn*/

            if(settings->keyboard_active) /* kontrol birimine gore ikizin yeri*/
            {
              switch (keyboard) {
                case DOWN:{
                  agar[j].coord.x=agar[i].coord.x;
                  agar[j].coord.y=agar[i].coord.y+50;
                }break;
                case UP:{
                  agar[j].coord.x=agar[i].coord.x;
                  agar[j].coord.y=agar[i].coord.y-50;
                }break;
                case LEFT:{
                  agar[j].coord.x=agar[i].coord.x-50;
                  agar[j].coord.y=agar[i].coord.y;
                }break;
                case RIGHT:{
                  agar[j].coord.x=agar[i].coord.x+50;
                  agar[j].coord.y=agar[i].coord.y;
                }break;
              }
            }
            else /* mause gore fırlat */
            {
              agar[j].coord.x=agar[0].coord.x;
              agar[j].coord.y=agar[0].coord.y;
            }
          }
        }
        agar[i].r *= 0.6; /* asilin boyutunu güncelle */
        agar[i].mass *= 0.6; /* puanını güncelle */
    }
    counter=0; /* sadece tek ikiz olustur */
  }

  for(i=0;i<size;i++)
    agar[i].is_twin=false; /* yeniden split icin ikizligi kaldır */
}


void draw_agar(agar_t *agar,option_t *settings,int size)
{
  int i;

  for(i=0;i<size;++i)
  {
    if(agar[i].alive)
    {
      al_draw_scaled_bitmap(settings->skin,0,0,al_get_bitmap_width(settings->skin),
                            al_get_bitmap_height(settings->skin),agar[i].coord.x-agar[i].r,
                            agar[i].coord.y-agar[i].r,agar[i].r*2,agar[i].r*2,0);
      //al_draw_circle(agar[i].coord.x,agar[i].coord.y,agar[i].r,al_map_rgb(255,0,0),2);
      al_draw_textf(font,al_map_rgb(0,0,0),agar[i].coord.x,agar[i].coord.y,ALLEGRO_ALIGN_CENTER,"%d",agar[i].time);
    }
  }
}

void update_speed(agar_t *agar,int size)
{
  int i;

  for(i=0;i<size;i++)
    if(agar[i].alive)
      agar[i].speed = 200.0 / ((agar[i].r*2+50));

}

void create_bots(agar_t bots[],int bot_num)
{

  int i;
  for(i=0;i<bot_num;++i)
  {
    bots[i].coord.x=rand()%worldWidth;
    bots[i].coord.y=rand()%worldHeight;
    bots[i].r=15+rand()%20;
    bots[i].mass=10+bots[i].r;
    bots[i].alive=true;
    bots[i].color=al_map_rgb(rand()%256,rand()%256,rand()%256);/* renkler random*/
    bots[i].speed=4;
  }
}

void draw_bots(agar_t bots[],int bot_num)
{
  int i=0;
  for(i=0;i<bot_num;++i)
  {
    if(bots[i].alive)
    {
      /* merkezde yarıcapı yazar */
      al_draw_filled_circle(bots[i].coord.x,bots[i].coord.y,bots[i].r,bots[i].color);
      al_draw_textf(font,al_map_rgb(255,255,255),bots[i].coord.x,bots[i].coord.y,ALLEGRO_ALIGN_CENTER,"-%.1f-",bots[i].r);
    }
  }
}

void move_bots(agar_t bots[],int bot_num,agar_t baits[],int baits_num,agar_t *player,int arr_size)
{
  float nr_dist=100000; /* nearest id and distance */
  float distance;
  int nr_id=0; /* near_id */
  int i,j,k;
  int player_id;
  int max=0,max_id=0;

  for(j=0;j<bot_num;++j)
  {
    if(bots[j].alive) /* bot aktifmi */
    {
      /* yem kontrolu */
      for(i=0;i<baits_num;++i)
      {
          distance = sqrt(pow(abs(bots[j].coord.x-baits[i].coord.x),2)+pow(abs(bots[j].coord.y-baits[i].coord.y),2));
          if( distance <= nr_dist)
          {
            nr_dist = distance;
            nr_id=i; /* yemin adresi*/
          }
      }

      /* bot kontrolu */
      for(k=0;k<bot_num;++k)
      {
        if(k!=j)
        { /* bot takınsa ve kendisi değilse onu sec */
          distance = sqrt(pow(abs(bots[j].coord.x-bots[k].coord.x),2)+pow(abs(bots[j].coord.y-bots[k].coord.y),2));
          if(distance < bots[j].r+bots[k].r + 50 && bots[k].r + 2 < bots[j].r)
            nr_id=-2;
        }
      }

      /* player kontrolu */
      nr_dist=10000;
      for(i=0;i<arr_size;i++)
      {
          if(player[i].alive)
          {
            distance = sqrt(pow(abs(bots[j].coord.x-player[i].coord.x),2)+pow(abs(bots[j].coord.y-player[i].coord.y),2));
            if(distance < bots[j].r + player[i].r + 300 && player[i].r + 5 < bots[j].r && distance < nr_dist)
            {
              nr_dist=distance;
              nr_id=-1;
              player_id=i; /* playeri belirle */
            }
          }
      }


      /* kontrolleri uygulama */

      if(nr_id>=0)
      {
        /* secili yeme dogru harekete gecer */
        follow_target(&bots[j],&baits[nr_id]);
        if(can_eat(bots[j],baits[nr_id]))
        {
          check_baits(&baits[nr_id]);
          bots[j].r+=0.2;
          bots[j].mass+=1;
        }
      }
      else if(nr_id == -1) /* kullnıcı yakınsa ona yonelir */
      {
        attack_bait(baits,baits_num,&bots[j]);
        follow_target(&bots[j],&player[player_id]);
        if(can_eat(bots[j],player[player_id]))
        {
          player[player_id].alive=false;
          /* eger ana karakteri yerse en buyuk ona ana karakter olsun*/
          if(player_id==0)
          {

            for(i=1;i<100;i++) /* 0.elemana max r olan gelsin */
            {
                if(player[i].alive && player[i].r >= max)
                {
                  max_id=i;
                  max=player[i].r;
                }
            }
            player[max_id].alive = false;/* max olanı oldur */
            /* 0'ı max ile doldur */
            player[0].alive = true;
            player[0].coord.x = player[max_id].coord.x;
            player[0].coord.y = player[max_id].coord.y;
            player[0].r = player[max_id].r;
            player[0].speed = player[max_id].speed;
            player[0].mass+=player[max_id].mass;

            if(max_id==0)/* tek agar kaldıysa oyun biter */
              player[max_id].alive = false;
          }
          else
          {
              player[0].mass+=player[max_id].mass;
          }
        }
      }
      /* pek saglıklı calısmıyor burası */
      else if(nr_id == -2) /* yakınlarda bot varsa ona saldır */
      {
        attack_bait(baits,baits_num,&bots[j]);
        follow_target(&bots[j],&bots[k]);
        if(can_eat(bots[j],bots[k]))
        {
          bots[k].alive=false;
          bots[j].r += bots[k].r*0.2;
        }
      }
      nr_id=0; /* saldırılacak olanı sıfırla */
    }
  }
}

void follow_target(agar_t *bot,agar_t *bait)
{
  // aradaki mesafeler
  float disx = bait->coord.x - bot->coord.x;
  float disy = bait->coord.y - bot->coord.y;

  /* bot haritadan cıkamaz*/
  if(bot->coord.x > 0 && bot->coord.x < worldWidth && bot->coord.y > 0 && bot->coord.y <worldHeight)
  {
    if(disx>=0)
      bot->coord.x += bot->speed;
    else bot->coord.x -= bot->speed;

    if(disy>=0)
      bot->coord.y += bot->speed;
    else bot->coord.y -= bot->speed;
  }
}


void show_stats(score_t stats)
{
  al_clear_to_color(al_map_rgb(15,200,200));

  al_draw_textf(font24,al_map_rgb(0,0,0),500,200,ALLEGRO_ALIGN_CENTER,
  "Eaten Bait: %d",stats.eaten_bait);
  al_draw_textf(font24,al_map_rgb(0,0,0),500,300,ALLEGRO_ALIGN_CENTER,
  "Eaten Bot:%d",stats.eaten_bot);
  al_draw_textf(font24,al_map_rgb(0,0,0),500,400,ALLEGRO_ALIGN_CENTER,
  "Mass:%.2f",stats.mass);
  al_flip_display();
  al_rest(2.0);
}
