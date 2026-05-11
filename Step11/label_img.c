// Il faut lire la solution
// l'int\'egrer dans l'image
// afficher l'image.


#include <stdio.h>
#include "img_io.h"
#include "label_img.h"



#define H_DIGIT 5
#define W_DIGIT 3



pixel*** get_digit_ref() {
  pixel B = black ;
  pixel W = white ;

  pixel digit_ref[10][H_DIGIT][W_DIGIT] = {
    {
      { B, B, B },
      { B, W, B },
      { B, W, B }, 
      { B, W, B }, 
      { B, B, B }
    },
    {
      { B, B, W } ,
      { W, B, W } ,
      { W, B, W } ,
      { W, B, W } ,
      { B, B, B }
    },
    {
      { B, B, B },
      { W, W, B } ,
      { B, B, B },
      { B, W, W } ,
      { B, B, B }
    },
    {
      { B, B, B },
      { W, W, B } ,
      { W, B, B },
      { W, W, B } ,
      { B, B, B }
    } ,
    {
      { B, W, B } ,
      { B, W, B } ,
      { B, B, B } ,
      { W, W, B } ,
      { W, W, B }
    },
    {
      { B, B, B },
      { B, W, W } ,
      { B, B, B },
      { W, W, B } ,
      { B, B, B }
    },
    {
      { B, B, B },
      { B, W, W },
      { B, B, B }, 
      { B, W, B }, 
      { B, B, B }
    },
    {
      { B, B, B },
      { W, W, B } ,
      { W, B, B },
      { W, W, B } ,
      { W, W, B }
    } ,
    {
      { B, B, B },
      { B, W, B },
      { B, B, B }, 
      { B, W, B }, 
      { B, B, B }
    },
    {
      { B, B, B },
      { B, W, B },
      { B, B, B }, 
      { W, W, B }, 
      { B, B, B }
    }
  } ;
  pixel*** d = malloc(sizeof(pixel**) * 10) ;
  for (int k = 0; k < 10; k++) {
    d[k] = malloc(sizeof(pixel*) * H_DIGIT) ;
    for (int i = 0 ; i < H_DIGIT; i++) {
      d[k][i] = malloc(sizeof(pixel) * W_DIGIT) ;
      for (int j = 0; j < W_DIGIT; j++) {
	d[k][i][j] = digit_ref[k][i][j] ;
      }
    }
  }
  return d ;
}



/*
int read_solution(char* filename, pixel* colors, vertex* vert) {
  int len = 0 ;
  FILE* f = fopen(filename, "r") ;
  int r,g,b ;
  int u ;
  while (fscanf(f, "(%d, (%d, %d, %d)) ", &u, &r, &g, &b) != EOF) {
    colors[len].r = r ;
    colors[len].g = g ;
    colors[len].b = b ;
    vert[len] = u ;
    len++ ;
  }
  fclose(f) ;
  return len ;
}
*/



// Precondition : l_img est en noir et blanc.
// x + height <= hauteur de img
// y + width <= largeur de img
void print_digit(pixel** img, int x, int y, pixel** digit, int height, int width) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (eq_pixel(digit[i][j], black)) {
	img[x+i][y+j] = black ;
      }
    }
  }
}



bool check(int** components, int x, int y, int digit_h, int digit_w, int* x_witness, int* y_witness, int u) {
  for (int j = digit_w-1; j >= 0; j--) {
    for (int i = digit_h-1; i >= 0; i--) {	
      if (components[x+i][y+j] != u) {
	*x_witness = i+x ;
	*y_witness = j+y ;
	return false ;
      }
    }
  }
  return true ;       
}

int min_(int a, int b) {
  if (a < b) return a ; else return b ; }


bool find_coord(int** components, int height, int width, int u, int digit_h, int digit_w,
		int* x, int* y) {
  *x = 0 ;
  *y = 0 ;
  int min_abs = height ;
  while (*x + digit_h < height) {
    *y = 0 ;
    while (*y + digit_w < width) {
      int x_witness ;
      int y_witness ;
      if (check(components, *x, *y, digit_h, digit_w, &x_witness, &y_witness, u))
	return true ;
      (*y) = y_witness+1 ;
      min_abs = min_(min_abs, x_witness) ;
    }
    *x = min_abs + 1;
    min_abs = height ;
  }
  return false ;
}

	// QUESTION THEORIQUE : comment pousser l'id\'ee plus loin ?
	// --> parcourir les lignes \`a l'envers une fois sur deux.




pixel** get_digit(int n, int* height, int* width, pixel*** digit_ref) {
  int* d = malloc(sizeof(int) * 200) ;
  int len = 0 ;
  if (n==0) {
    d[0] = 0 ;
    len++ ;
  }
  while (n > 0) {
    d[len] = n % 10 ;
    n = n/ 10 ;
    len++ ;
  }

  int h_digit = H_DIGIT ;
  int w_digit = W_DIGIT ;
  int h = h_digit ;
  int w = w_digit * len + len - 1 ;
  pixel** digit = malloc(sizeof(pixel*) * h) ;
  for (int i = 0; i < h; i++) {
    digit[i] = malloc(sizeof(pixel) * w) ;
    for (int j = 0; j < w; j++) {
      digit[i][j] = white ;
    }
  }

  int x = 0;
  int y = 0;
  for (int k = len-1; k >= 0; k--) {
    print_digit(digit, x, y, digit_ref[d[k]], h_digit, w_digit) ;
    y += w_digit ;
    for (int i = 0; i < h; i++)
      digit[i][y] = white ;
    y++ ;
  }
  *height = h ;
  *width = w ;
  free(d) ;
  return digit ;
}




pixel** resize(pixel** img, int height, int width, int zoom) {
  pixel** new_img = malloc(sizeof(pixel*) * height * zoom) ;
  for (int i = 0; i < height*zoom; i++) {
    new_img[i] = malloc(sizeof(pixel) * width * zoom) ;
    for (int j = 0; j < width*zoom; j++) {
      new_img[i][j] = img[i/zoom][j/zoom] ;
    }
  }
  return new_img ;
}


// Precondition : p EST dans le tableau
int index_(pixel p, int len, pixel* reference) {
  for (int i = 0; i < len; i++) {
    if (p.r == reference[i].r && p.g == reference[i].g &&  p.b == reference[i].b)
      return i ;
  }
  assert(false) ;
  return (-1) ; // should never happen
}




void label_img(int nb_vertex, pixel* colors, int** components,
	       pixel** img, int height, int width, pixel*** digit_ref) {
 
  for (int u = 0; u < nb_vertex; u++) {
    int h_digit, w_digit ;
    pixel** digit_small = get_digit(u, &h_digit, &w_digit, digit_ref) ;
    pixel** digit = resize(digit_small, h_digit, w_digit, 3) ;
    free_2D((char**)digit_small, h_digit) ;
    int x, y ;
    
    if (find_coord(components, height, width, u, 3 * h_digit + 6, 3 * w_digit + 6, &x, &y)) {
      print_digit(img, x+3, y+3, digit, 3*h_digit, 3*w_digit) ;
    } else {
      fprintf(stderr, "Cannot find room for vertex number %d.\n", u) ;
    }


    free_2D((char**)digit, h_digit * 3) ;
  }    
}

/* POSSIBLE IMPROVEMENTS

- reuse the memory allocated for digit
- don't start search from the top again for every digit.
- when a vertex cannot be printed, reduce the zoom until it works.
 */


