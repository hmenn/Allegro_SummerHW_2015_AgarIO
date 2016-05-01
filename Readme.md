LİNUXTA ALLEGRO5 YUKLU İSE DİREK BUNLARLA DERLE CALISTIR

gcc -c agar.c main.c

gcc main.o agar.o -o agar -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_image -lm

./agar
