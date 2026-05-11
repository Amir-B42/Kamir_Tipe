#ifndef IMG_IO_H
#define IMG_IO_H

// Il n'y aura plus besoin d'inclure ses lib dès lors que
// vous chargez img_io.h
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>



struct pixel { unsigned char r ; unsigned char g; unsigned char b ; } ;
typedef struct pixel pixel ;
typedef pixel** image ;


static pixel black = { 0,0,0 } ;
static pixel white = { 255,255,255 } ;


bool eq_pixel(pixel p1, pixel p2) ;
void free_2D(char** arr, int nb_lines) ;
image read_img(char* filename, unsigned int* height, unsigned int* width) ;
void print_img(pixel** img, int height, int width, char* filename) ;

pixel** img_copy(image img, int height, int width) ;






#endif
