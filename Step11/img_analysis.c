#include "img_io.h"
#include "img_analysis.h"




int sqrt_(int n) {
  if (n == 0)
    return 0 ;
  if (n == 1)
    return 1 ;
  int a = 2 * sqrt_(n/4) ;
  if ((a+1)*(a+1) > n)
    return a ;
  else
    return a+1 ;
}





int distance1(pixel p1, pixel p2) {
  int r1,g1,b1, r2,g2,b2 ;
  r1 = p1.r ;
  g1 = p1.g ;
  b1 = p1.b ;
  r2 = p2.r ;
  g2 = p2.g ;
  b2 = p2.b ;
  int d1 = abs(r1 - r2) ;
  int d2 = abs(g1- g2) ;
  int d3 = abs(b1 - b2) ;
  int r_bar = (r1+r2)/2 ;
  if (r_bar < 128)
    return 2*d1 + 4*d2 + 3*d3 ;
  else
    return 3*d1 + 4*d2 + 2*d3 ;
} // O(1)




int distance(pixel p1, pixel p2) {
  int r1,g1,b1, r2,g2,b2 ;
  r1 = p1.r ;
  g1 = p1.g ;
  b1 = p1.b ;
  r2 = p2.r ;
  g2 = p2.g ;
  b2 = p2.b ;
  int d1 = abs(r1 - r2) ;
  int d2 = abs(g1- g2) ;
  int d3 = abs(b1 - b2) ;
  return 2126*d1*d1 + 7152*d2*d2 + 722*d3*d3 ;
} // O(1)


bool are_close_colors2(int r1, int g1,int  b1, int r2, int g2, int b2, int threshold) {
  return (abs(r1-r2) < threshold) && (abs(g1-g2) < threshold) && (abs(b1-b2) < threshold) ;
}





pixel** detect_contour(pixel** img, int height, int width, int threshold) {
  pixel** new_img = malloc(sizeof(pixel*) * height) ;
  for (int i = 0 ; i < height; i++) {
    new_img[i] = malloc(sizeof(pixel) * width) ;
    new_img[i][0] = white ;
    new_img[i][width-1] = white ;
  }
  for (int j = 0; j < width; j++) {
    new_img[0][j] = white ;
    new_img[height-1][j] = white ;
  }

  
  for (int i = 1; i < height; i++) {
    for (int j = 1; j < width; j++) {
      int vert_diff_r = img[i][j].r - img[i-1][j].r ;
      int vert_diff_g = img[i][j].g - img[i-1][j].g ;
      int vert_diff_b = img[i][j].b - img[i-1][j].b ;
      int hori_diff_r = img[i][j].r - img[i][j-1].r ;
      int hori_diff_g = img[i][j].g - img[i][j-1].g ;
      int hori_diff_b = img[i][j].b - img[i][j-1].b ;
      pixel p = {
	sqrt_(vert_diff_r * vert_diff_r + hori_diff_r * hori_diff_r) ,
	sqrt_(vert_diff_g * vert_diff_g + hori_diff_g * hori_diff_g) ,
	sqrt_(vert_diff_b * vert_diff_b + hori_diff_b * hori_diff_b)
      } ;
      if (distance(p,black) < threshold)
      // if (are_close_colors2(p.r, p.g, p.b,0,0,0,threshold))
	new_img[i][j] = black ;
      else
	new_img[i][j] = white ;
    }
  }
  return new_img ;
  
}




// Sont complètes et correctes
int min(int a, int b) {
  if (a < b)
    return a ;
  return b;
}
int max(int a, int b) {
  if (a > b)
    return a ;
  return b;
}

// DEPRECATED
pixel** remove_isolated_pixels(pixel** img_BW, int height, int width, int radius, int prop) {

  pixel** new_img = malloc(sizeof(pixel*) * height) ;
  assert(new_img != NULL) ;
  
  for (int i = 0; i < height; i++) {
    new_img[i] = malloc(sizeof(pixel) * width) ;
    assert(new_img[i] != NULL) ;
    for (int j = 0; j < width; j++) {
      // majority pixel around img[i][j]
      int cpt = 0 ; // cpt for BLACK
      int size_sphere = 0; 
      for (int p = max(i-radius+1, 0); p < min(i+radius, height); p++) {
	for (int q = max(j-radius+1, 0); q < min(j+radius, width); q++) {
	  if (eq_pixel(img_BW[p][q], black))
	    cpt++ ;
	  size_sphere++ ;
	}
      }
      if (eq_pixel(img_BW[i][j], black) || cpt > (prop-1)*size_sphere/prop)
	new_img[i][j] = black ;
      else
	new_img[i][j] = white ;
    }
  }
  return new_img ;
}


