#include "img_io.h"
#include "img_analysis.h"
#include "graphs.h"
#include "label_img.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main (int argc, char** argv) {

  // ARGS
  if (argc != 4) {
    fprintf(stderr, "Wrong number of arguments : expected 5.\n") ;
    exit(1) ;
  }
    
  char* file_in = argv[1] ; // image IN
  int threshold = atoi(argv[2]) ; // threshold for proximity with black
  // --> 100 is great.
  int thresh = atoi(argv[3]) ; // threshold to filter small components
  // and nb of edges
  // --> 50 is great
  int filename_len = strlen(file_in) ;

  // READ IMG
  unsigned int width, height ;
  pixel** img = read_img(file_in, &height, &width) ;
  pixel** img1 = img ;

  // READING TEST
  print_img(img, height, width, "test.ppm") ;

  
  // BW GENERATION = CONTOUR DETECTION
  pixel** img_BW1 = detect_contour(img, height, width, threshold) ;

  char* image_BW1 = malloc(sizeof(char) * (filename_len + 10)) ;
  char* suffix_BW1 = "_BW.ppm" ;
  strcpy(image_BW1, file_in) ;
  image_BW1[filename_len-4] = '\0' ;
  strcat(image_BW1, suffix_BW1) ;

  print_img(img_BW1, height, width, image_BW1) ;


  // CONVERSION TO GRAPH
  char* img_mono1 = malloc(sizeof(char) * (filename_len + 10)) ;  
  char* suffixx_mono1 = "_mono.ppm" ;
  strcpy(img_mono1, file_in) ;
  img_mono1[filename_len-4] = '\0' ;
  strcat(img_mono1, suffixx_mono1) ;
  char* img_graph1 = malloc(sizeof(char) * (filename_len + 10)) ;
  char* suffixx_graph1 = "_graph" ;
  strcpy(img_graph1, file_in) ;
  img_graph1[filename_len-4] = '\0' ;
  strcat(img_graph1, suffixx_graph1) ;

  pixel* colors ;
  int nb_vertex ; 
  int** components =
    graph_output(img1, img_BW1, height, width, thresh, &colors, &nb_vertex, img_graph1) ;
  // THIS HAS FREED IMG_BW
  // THIS HAS PRINTED GRAPH IN FILE ..._graph

  // ADDING THE VERTICES TO THE IMG
  pixel*** digit_ref = get_digit_ref() ;
  label_img(nb_vertex, colors, components, img1, height, width, digit_ref) ;
  print_img(img1, height, width, img_mono1) ;

  free_2D((char**)img1, height) ;


  // FREE
  free_2D((char**)components, height) ;
  // probably forgot some things

  
  // PROBLEM SOLVING IN OCAML
  char* img_sol = malloc(sizeof(char) * (filename_len + 10)) ;  
  char* suffixx_sol = "_solution" ;
  strcpy(img_sol, file_in) ;
  img_sol[filename_len-4] = '\0' ;
  strcat(img_sol, suffixx_sol) ;

  
  char cmd[300] = "./../Step2/solve " ;
  strcat(cmd, img_graph1) ;
  strcat(cmd, " > ") ;
  strcat(cmd, img_sol) ;
  // this builds the string "./../Step2/solve file_graph > file_solution"
  system(cmd) ;
}
