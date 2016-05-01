/*#######################################################*/
/*#######################################################*/
/*              GEBZE TEKNIK UNIVERSITESI                */
/*             2015 C PROGRAMLAMA YAZ ODEVI              */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                HASAN MEN - 131044009                  */
/*#######################################################*/
/*#######################################################*/
#ifndef AGAR_HEADER_FILE
#define AGAR_HEADER_FILE
#endif

#define screenWidth  1000 /* ekran boyutu */
#define screenHeight  700

#define worldWidth  4000 /* harita boyutu */
#define worldHeight  4000

#define FPS 60.0 /* ekran tazeleme hızı */

#define FONT_SIZE 12  /* font bilgileri */
#define FONT_SIZE24 24
#define FONT_FILE "arial.ttf"

/* kisisel veri tipleri */
typedef enum {PLAY,OPTION,INSTRACTIONS,CREDITS,EXIT}menu_choice;
typedef enum {DOWN,LEFT,RIGHT,UP}Direction_t;/* hareketler*/

typedef struct {
  float x,y;
}coordinat_t; // coordinat bilgileri

typedef struct{
  coordinat_t coord;
  float r; // yarıcap
  int mass; // toplam puan
  int alive; // yasama durumu
  ALLEGRO_COLOR color; // baitler icin renk
  int time; // yeniden birlesme zamanı
  float speed; // hareket hızı
  bool is_twin; // split_agar fonksiyonu icin kontrol
  int whose_twin; // kimin ikizi - ona birlesecek
}agar_t;

/* ayarlar*/
typedef struct {
  int baits_num; // yem miktarı
  int bot_number; // bot miktarı
  int skin_counter; // skin numarası
  int map_counter; // harita numarası
  ALLEGRO_BITMAP *skin; // ana karakter skini
  ALLEGRO_BITMAP *backg; // arkaplan
  coordinat_t mouse_coord; // fare koordinatları
  coordinat_t menu_arrow; // menu secim oklari
  coordinat_t cam_coord; // kamera koordinatları
  int mouse_active; // mause klavye ayarı
  int keyboard_active;
  float zoom; // zoom miktarı
}option_t;

// oyun sonu bilgi dondurmk icin veri tipi
typedef struct{
  float mass;
  float radius;
  int eaten_bait;
  int eaten_bot;
}score_t;

/* Fonksiyonlarda formal parametre karısıklıgı icin global olarak tanımlandı*/
ALLEGRO_DISPLAY *display;

/* 13 AGAR_SKIN */
ALLEGRO_BITMAP  *skin_2ch,*skin_bait,*skin_cia,
                *skin_doge,*skin_facepunch,*skin_lmao,
                *skin_matriarchy,*skin_nazi,
                *skin_reddit,*skin_sanik,*skin_satanist,
                *skin_turkey,*skin_wojak;
ALLEGRO_BITMAP *bg,*bg1,*bg2; /* 3 farklı oyun ici tema */
ALLEGRO_BITMAP *menu_all,*menu_all_arrow,*instrucs,*options,*credits;

ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TRANSFORM camera;
ALLEGRO_FONT *font,*font24;
ALLEGRO_TIMER *timer;

/* NOT :: call_by_value de zaman ve depolama kaybı oldugu için bazı degiskenler
 call_by_reference ile gonderildi. */


/*############################################################################*/
/* oyunun oynanis bilgileri */
void menu_instructions(option_t *settings);
/* genel menumuz */
bool menu(option_t *settings);
/*mause kontrolu - mause nerede */
bool is_mause_in(int x1,int y1,int x2,int y2,coordinat_t mouse);
/* ayarlarin kontrolu */
void menu_options(option_t *settings);
/* emegi gecen hakkında bilgi */
void menu_credits(option_t *settings);
/*############################################################################*/

/*############################################################################*/
/* ana kontrolcunun -player[0]- konumuna gore kamerayı ayarlar                */
/* settings -> call by ref. - camera koordinatları içinde depolanır.          */
/* x,y -> kontrolcunun konumu                                                 */
/*############################################################################*/
void change_camera(option_t *settings,float x,float y);

/*############################################################################*/
/* ana kontrolcunun yarıcapına gore ekran zoom degerini belirler              */
/* *zoom - call by ref.   -settingste depolanır.                              */
/* radius - call by val.  -> kontrolcu yarıcapı                               */
/*############################################################################*/
void update_scale(const float radius,float *zoom);

/*############################################################*/
/* Bu fonksiyon can_eat fonksiyonunu cagırır.                 */
/* BU fonksiyon tum botları kontrol eder ve yeme basına       */
/* yarıcap ve toplam puanı arttırır.                          */
/* Yenen bot miktarını return eder(Buyuksek 1den fazla yeriz) */
/* --------                                                   */
/* bot -> botların oldugu dizi                                */
/* size -> yem miktarı -> declared constant                   */
/* player -> bota müdahale edecek kişi -> bot yada ana oyuncu */
/*############################################################*/
int attack_bots(agar_t bots[],int size,agar_t *player);

/*############################################################*/
/* Oyun baslangıcında bot_num kadar bot olusturur.               */
/* Tum degiskenler random olusturulur.                        */
/*------------                                                */
/* bots -> botların islenecegi kısım                         */
/* bot_num -> kac tane yem olsuturulacagı                    */
/*############################################################*/
void create_bots(agar_t bots[],int bot_num);

/* botların ekrana basılması */
void draw_bots(agar_t bots[],int bot_num);

/* ortamdaki yem-bot-agarlara gore botu yapay zeka gibi oynatır*/
void move_bots(agar_t bots[],int bot_num,agar_t baits[],int baits_num,
                agar_t *player,int arr_size);

/* botun konumunu yeme gore gunceller */
void follow_target(agar_t *bot,agar_t *bait);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* TUM OYUN BURADAN BASLATILIR                                                */
/* oyuncu,bot ve yemler olusturulur.Haritayı doldurup oyunu baslatır          */
/* oyun sonu degerleri score_t turunde return eder                            */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
score_t start_game(option_t *settings);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* settingsten aktif mouse konumuna gore agar konumu guncellenir              */
/* *player -> oyuncumuz                                                       */
/* size -> dizi boyutu                                                        */
/* *settins -> mouse konumu okunacaktır                                       */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void move_accor_mouse(agar_t *player,int size,const option_t *settings);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Klavyeden girilen yone gore hareket saglanır.                   */
/* my-> kullanıcı                                                  */
/* dir -> gidilecek yon -> sag,sol,asagı,yukarı,capraz,            */
/* size -> dizi boyutu                                             */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void move(agar_t *my,Direction_t dir,int size);

/*############################################################*/
/*  Yari caplarla dogru orantılı olacak sekilde hız ayarı     */
/* agar -> bot yada kullanıcı olabilir                        */
/* size -> düzenlenecek hedep sayisi                          */
/*############################################################*/
void update_speed(agar_t *agar,int size);

/*############################################################*/
/* yemleri ekrana basar                                       */
/* ölu yemler icin canlılık kontrolu yapar                    */
/*############################################################*/
void draw_baits(agar_t baits[],int bait_num);

/*############################################################*/
/* Oyun baslangıcında size kadar yem olusturur.               */
/* Tum degiskenler random olusturulur.                        */
/*------------                                                */
/* baits -> yemlerin islenecegi kısım                         */
/* bait_num -> kac tane yem olsuturulacagı                    */
/*############################################################*/
void create_baits(agar_t baits[],int bait_num);

/*############################################################*/
/* Oldurulen yemi yeniden canlandırır.                        */
/* Yeniden canlanınca koordinat degisir                       */
/*  Her olumden sonra yenisi olusur                           */
/*  baits -> olu olan yemler buradan kontrol edilip onarılır  */
/*############################################################*/
void check_baits(agar_t *baits);

/*############################################################*/
/* yem ile my arasındaki mesafe ve yarı cap kontrolu yapar    */
/* eger yer ise 1 diğer durumda 0 return eder                 */
/* ------------                                               */
/* my -> avcı   // bait -> yem(call_by_value)                 */
/*############################################################*/
bool can_eat(agar_t my,agar_t bait);

/*############################################################*/
/* Bu fonksiyon üstteki can_eat fonksiyonunu cagırır.         */
/* BU fonksiyon tum baitleri kontrol eder ve yeme basına      */
/* yarıcap ve toplam puanı arttırır.                          */
/* Yenen bait miktarını return eder(Buyuk isek 1den fazla yeriz)*/
/* --------                                                   */
/* bait -> ymlern oldugu dizi                                 */
/* size -> yem miktarı -> declared constant                   */
/* my -> yeme müdahale edecek kişi -> bot yada ana oyuncu     */
/*############################################################*/
int attack_bait(agar_t baits[],int size,agar_t *my);

/*############################################################*/
/* agar_t dizisinin icindeki canlı oyuncuları ekrana basar    */
/* agar -> agarların yer aldıgı dizisini                      */
/* size -> agar dizisinin boyutu                              */
/*############################################################*/
void draw_agar(agar_t *agar,option_t *settings,int size);

/*############################################################*/
/* Canlı her agar icin bolme islemini gerceklestirir.         */
/* yarı capının %40ı kadarını yeni agara atar                 */
/* size = 100 olarak onceden tanımlanmıstır.Max 100 agar olur */
/*############################################################*/
void split_agar(agar_t agar[],int size,const option_t *settings,
                      Direction_t keyboard);

/*##############################################################*/
/*Suresi azalan agarları kimden parcalandıysa onunla birlestirir*/
/*birlesecek agar yok ise 0.agara birlestirir                   */
/* birlesme aniden olur.                                        */
/* agar- > agar dizimiz     / size -> dizi boyutu               */
/*##############################################################*/
void combine_agar(agar_t *agar,int size);

/*##############################################################*/
/* Oyun sonu sonucunu ekran basar                               */
/* stats -> oyun sonucu depolanır                               */
/*##############################################################*/
void show_stats(score_t stats);
