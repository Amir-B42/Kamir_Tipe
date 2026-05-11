#include <stdio.h>
#include "img_io.h"
#include <string.h>


/* ------ Excerpt From img_io.h
struct pixel { unsigned char r ; unsigned char g; unsigned char b ; } ;
typedef struct pixel pixel ;

pixel black = { 0,0,0 } ;
pixel white = { 255,255,255 } ;
   ------- */



bool eq_pixel(pixel p1, pixel p2) {
  return p1.r == p2.r && p1.g == p2.g && p1.b == p2.b ;
}


void free_2D(char** arr, int nb_lines) {
  for (int i = 0; i < nb_lines; i++)
    free(arr[i]) ;
  free(arr) ;
}


pixel** read_img(char* filename, unsigned int* height, unsigned int* width) {
  int len = strlen(filename) ;
  if (filename[len-4] != '.' || filename[len-3] != 'p' || filename[len-2] != 'p' ||
      filename[len-1] != 'm') {
    fprintf(stderr, "Input image must have extension .ppm") ;
    exit(1) ;
  }
  
  FILE* f_in = fopen(filename, "rb") ;
  if (f_in == NULL) {
    fprintf(stderr, "Cannot open file %s.\n", filename) ;
    exit(1) ;
  }
  
  int max_value ;
  fscanf(f_in, "P6 %d %d %d", width, height, &max_value) ;
  fgetc(f_in) ;

  pixel** img = malloc(sizeof(pixel*) * (*height)) ;
  assert(img != NULL) ;
  for (int i = 0; i < *height; i++) {
    img[i] = malloc(sizeof(pixel) * (*width)) ;
    assert(img[i] != NULL) ;
  }

  unsigned char r,g,b ;
  int eof_check ;
  for (int i = 0; i < *height; i++) {
    for (int j = 0; j < *width; j++) {
      // fscanf(f_in, "%c%c%c", &r, &g, &b) ;
      r = fgetc(f_in) ;
      g = fgetc(f_in) ;
      eof_check = fgetc(f_in) ;
      b = (unsigned char)eof_check ; // conversion
      if (eof_check == EOF) {
	fprintf(stderr, "File is ill-formed  : less pixels than header dimensions.\n") ;
	fclose(f_in) ; // utile ?
	exit(1) ;
      }
      img[i][j].r = r ;
      img[i][j].g = g ;
      img[i][j].b = b ;
    }
  }
  
  if (fgetc(f_in) != EOF) {
    fprintf(stderr, "File is ill-formed  : more pixels than header dimensions.\n") ;
    fclose(f_in) ; // utile ?
    exit(1) ;
  }
  fclose(f_in) ;
  return img ;
}


void print_img(pixel** img, int height, int width, char* filename) {
  //  printf("P6 %s %s 255\n", convert_back_ascii(width), convert_back_ascii(height)) ;
  FILE* f_out = fopen(filename, "wb");
  
  fprintf(f_out, "P6 %d %d 255\n", width, height) ;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      // fprintf(f_out, "%c%c%c", img[i][j].r, img[i][j].g, img[i][j].b) ;
      fputc(img[i][j].r, f_out);
      fputc(img[i][j].g, f_out) ;
      fputc(img[i][j].b, f_out) ;

      
      /// /!\ PAS DE \n en fin de ligne !
    }
  }
  fclose(f_out) ;
}




pixel** img_copy(image img, int height, int width) {
  pixel** img_c = malloc(sizeof(pixel*) * height) ;
  for (int i = 0; i < height; i++) {
    img_c[i] = malloc(sizeof(pixel) * width) ;
    for (int j = 0; j < width; j++) {
      img_c[i][j] = img[i][j] ;
    }
  }
  return img_c ;
}







