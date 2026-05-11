#include <stdio.h>
#include "img_io.h"
#include "img_analysis.h"
#include "graphs.h"


/* Graphes repr\'esent\'es par liste d'adjacence =
 tableau de listes chain\'ees.

 From parcours.h :


 typedef int vertex ;
typedef int** graph ; // graph = matrice adjacence.
 
*/


/// FONCTIONS ET TYPES UTILITAIRES

struct cell { vertex vertex; struct cell* next; } ;
typedef struct cell cell ;
typedef cell* list ;



void free_list(list l) {
  if (l != NULL) {
    free_list(l->next) ;
    free(l) ;
  }
}


list cons(vertex u, list l) {
  cell* c = malloc(sizeof(cell)) ;
  c->vertex = u ;
  c->next = l ;
  return c ;
}

int pop(list* l) {
  assert(*l != NULL) ;
  int elt = (*l)->vertex ;
  list hd = *l ;
  *l = (*l)->next ;
  free(hd) ;
  return elt ;
}

// FIN FONCTION UTILITAIRES


    


int explore_component(list* stack, pixel** img, int height, int width,
		      vertex vertex_num, vertex** components) {
  int size = 0 ;
  list* s = stack ;
  while (*s != NULL) {
    int i = pop(stack) ;
    int j = pop(stack) ;
    if (components[i][j] == -1) {
      // printf("%d %d %d %d %d\n", i, j, vertex_num, height, width) ;
      //  get_char() ;
      components[i][j] = vertex_num ;
      size++ ;
      if (i-1 >= 0 && eq_pixel(img[i][j], img[i-1][j]))
	*s = cons(i-1, cons(j, *s)) ; // push
      if (i+1 < height && eq_pixel(img[i][j] , img[i+1][j]))
	*s = cons(i+1, cons(j, *s)) ;
      if (j-1 >= 0 && eq_pixel(img[i][j] , img[i][j-1]))
	*s = cons(i, cons(j-1, *s)) ;
      if (j+1 < width && eq_pixel(img[i][j] , img[i][j+1]))
	*s = cons(i, cons(j+1, *s)) ;
    }
  }
  return size ;
}
 


  
// 
int** compute_components(pixel** img, int height, int width, int* nb_vertex, list* sizes_ret) {

  list sizes = NULL ;
  *nb_vertex = 0 ;

  // Initialisation
  int** components = malloc(sizeof(int*) * height) ;
  for (int i = 0; i < height; i++) {
    components[i] = malloc(sizeof(int) * width) ;
    for (int j = 0; j < width; j++) {
      components[i][j] = - 1 ; // non vu
    }
  }

  // Calcul des composantes monochromes
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (components[i][j] == -1 && eq_pixel(img[i][j],black)) {
	// printf("%d %d\n", i,j ) ;
	list stack = cons(i, cons(j, NULL)) ;	
	int size = explore_component(&stack, img, height, width, *nb_vertex, components) ;
	sizes = cons(size, sizes) ;
	(*nb_vertex)++ ;
      }
    }
  }
  *sizes_ret = sizes ;
  return components ;
}


void filter_small_components(pixel** img, int height, int width,
			     int** components, int nb_vertex,
			     list sizes, int threshold) {
   
			    
  // Retrait des petites composantes :
  int* switch_component = malloc(sizeof(int) * (nb_vertex)) ;
  list l = sizes ;
  for (int i = nb_vertex-1 ; i >= 0; i--) {
    int size = l->vertex ;
    l = l->next ;
    if (size < threshold)
      switch_component[i] = true ;
    else
      switch_component[i] = false ;
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int c = components[i][j] ;
      if (c != -1 && switch_component[c]) {
	if (eq_pixel(img[i][j], white)) {
	  img[i][j] = black ;
	} else {
	  img[i][j] = white ;
	}
      }
    }
  }
  free_list(sizes) ;
  free(switch_component) ;
 }




bool are_close_colors(int r1, int g1,int  b1, int r2, int g2, int b2, int threshold) {
  return (abs(r1-r2) < threshold) && (abs(g1-g2) < threshold) && (abs(b1-b2) < threshold) ;
}


pixel* retrieve_colors(image img, int height, int width, int** components, int nb_vertex, list sizes) {
  int* avg = malloc(3* sizeof(int) * nb_vertex) ;
  for (int k = 0; k < 3*nb_vertex; k++)
    avg[k] = 0 ;
  
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int c = components[i][j] ;
      if (c != -1) {
	pixel p = img[i][j] ;
	avg[3*c] += p.r ;
	avg[3*c+1] += p.g ;
	avg[3*c+2] += p.b ;
      }
    }
  }
  list l = sizes ;
  for (int k = nb_vertex-1; k >= 0; k--) {
    int size = l->vertex ;
    l = l->next ;
    avg[3*k] = avg[3*k] / size ;
    avg[3*k+1] = avg[3*k+1] / size ;
    avg[3*k+2] = avg[3*k+2] / size ;
  }
  free_list(sizes) ;


  /* UNIFORMIZATION OF COLORS

     If want close colors to be the exact same.
     If two colors are close, we put them in the same
     equivalence class, then avg among eq classes.
  */
  // Union-find kind of thing
  int nb_colors = nb_vertex ;
  int* classes = malloc(sizeof(int) * nb_vertex) ;
  int* cardinals = malloc(sizeof(int) * nb_vertex) ;
  for (int k = 0; k < nb_vertex; k++) {
    classes[k] = k ;
    cardinals[k] = 1 ;
    int r2 = avg[3*k] ;
    int g2 = avg[3*k+1] ;
    int b2 = avg[3*k+2] ;
    for (int j = 0; j < k; j++) {
      if (are_close_colors(avg[3*j], avg[3*j+1], avg[3*j+2], r2, g2, b2, 50))  {
	int l = j ;
	while (l != classes[l]) {
	  l = classes[l] ;
	  printf("smells fishy ... vertices %d and %d and %d\n", k, j, l) ;
	}
	classes[k] = l ;
	cardinals[l] += 1 ;
	nb_colors-- ;
	break ;
      }
    }
    // break comes here.
  }

  for (int k = 0; k < nb_vertex; k++) {
    if (k != classes[k]) {
      int l = classes[k] ;
      avg[3*l] += avg[3*k] ;
      avg[3*l+1] += avg[3*k+1] ;
      avg[3*l+2] += avg[3*k+2] ;
    }
  }

  for (int k = 0; k < nb_vertex; k++) {
    if (k == classes[k]) {
      avg[3*k] = avg[3*k] / cardinals[k] ;
      avg[3*k+1] = avg[3*k+1] / cardinals[k] ;
      avg[3*k+2] = avg[3*k+2] / cardinals[k] ;
    }
  }


  // Array of colors for each vertex
  pixel* colors = malloc(sizeof(pixel) * nb_vertex) ;
  for (int k = 0; k < nb_vertex; k++) {
    int l = k ;
    while (l != classes[l])
      l = classes[l] ;
    if (are_close_colors(avg[3*l], avg[3*l+1], avg[3*l+2],255,255,255,50)) {
      colors[k] = white ;
    } else {
      pixel p = { avg[3*l], avg[3*l+1], avg[3*l+2] } ;
      colors[k] = p ;
    }
  }

  free(classes) ;
  free(cardinals) ;
  free(avg) ;
  return colors ;   
}



void erase_borders(image img, int height, int width, int** components) {
  bool flag = true ;
  pixel q_black = { 1,1,1 } ;

  while (flag) {
    flag = false ;
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
	if (components[i][j] == -1) {
	  if (!flag)
	    flag = true ;
	  img[i][j] = q_black ;
	  if (i > 0 && components[i-1][j] != -1 && !eq_pixel(img[i-1][j], q_black))
	    components[i][j] = components[i-1][j] ;
	  else if (j > 0 && (components[i][j-1] != -1) && !eq_pixel(img[i][j-1], q_black))
	    components[i][j] = components[i][j-1] ;
	  else if (i < height-1 && (components[i+1][j] != -1) && !eq_pixel(img[i+1][j], q_black))
	    components[i][j] = components[i+1][j] ;
	  else if (j < width-1 && (components[i][j+1] != -1) && !eq_pixel(img[i][j+1], q_black))
	    components[i][j] = components[i][j+1] ;	  
	}
      }
    }
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
	if (eq_pixel(img[i][j], q_black))
	  img[i][j] = black ;
      }
    }
  }
}




void monochromize_img(image img, int height, int width,
		      int** components, int nb_vertex, pixel* colors) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int c = components[i][j] ;
      pixel p = colors[c] ;      
      img[i][j] = p ;
    }
  }
}  
/*
void erase_borders(image img, int height, int width, int** components, pixel* color) {
  bool flag = true ;
  pixel q_black = { 1,1,1 } ;

  while (flag) {
    flag = false ;
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
	if (components[i][j] = -1) {
	  if (!flag)
	    flag = true ;
	  img[i][j] = q_black ;
	  if (i > 0 && components[i-1][j] != -1 && !eq_pixel(img[i-1][j], q_black))
	    components[i][j] = components[i-1][j] ;
	  else if (j > 0 && (components[i][j-1] != -1) && !eq_pixel(img[i][j-1], q_black))
	    components[i][j] = components[i][j-1] ;
	  else if (i < height-1 && (components[i+1][j] != -1) && !eq_pixel(img[i+1][j], q_black))
	    components[i][j] = components[i+1][j] ;
	  else if (j < width-1 && (components[i][j+1] != -1) && !eq_pixel(img[i][j+1], q_black))
	    components[i][j] = components[i][j+1] ;	  
	}
      }
    }
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
	if (eq_pixel[i][j], q_black)
	  img[i][j] = color[components[i][j]] ;
      }
    }    
  }
}
*/

int remove_white_components(int** components, int height, int width, int nb_vertex,
			     pixel* colors) {
  int nb_whites = 0 ;
  int* mapping = malloc(sizeof(int) * nb_vertex) ;
  int u = 0 ;
  for (int k = 0; k < nb_vertex; k++) {
    if (eq_pixel(white, colors[k])) {
      nb_whites++ ;
      mapping[k] = -1 ;
    } else {
      mapping[k] = u ;
      colors[u] = colors[k] ;
      u++ ;
    }
  }

  for (int i = 0; i < height; i++) { 
    for (int j = 0; j < width; j++) { 
      components[i][j] = mapping[components[i][j]] ;
    }
  }
  
  return nb_vertex - nb_whites ;
}



int** make_graph(int** components, int height, int width, int nb_vertex, int threshold) {
  int** g = malloc(sizeof(int*) * nb_vertex) ;
  for (int u = 0; u < nb_vertex; u++) {
    g[u] = malloc(sizeof(int) * nb_vertex) ;
    for (int v = 0 ; v < nb_vertex; v++) {
      g[u][v] = 0 ;
    }
  }

  for (int i = 0; i < height-1; i++) {
    for (int j = 0; j < width-1; j++) {
      int u = components[i][j] ;
      int v = components[i+1][j] ;
      int w = components[i][j+1] ;
      if (u != -1 && v != -1 && u != v) {
	g[u][v]++ ;
	g[v][u]++ ;
      }
      if (u != -1 && w != -1 && u != w) {
	g[u][w]++ ;
	g[w][u]++ ;
      }
    }
  }

  for (int u = 0; u < nb_vertex; u++) {
    for (int v = 0; v < nb_vertex; v++) {
      if (g[u][v] < threshold)
	g[u][v] = 0 ;
      else
	g[u][v] = 1;
    }
  }
  return g ;
}





void print_graph(int nb_vertex, graph g, pixel* colors, char* filename) {
  FILE* f_out = fopen(filename, "w") ;
  if (f_out == NULL) {
    fprintf(stderr, "Error accessing file %s.\n", filename) ;
    exit(1) ;
  }
  // fprintf(f_out, "%d\n", nb_vertex) ;
  for (int u = 0; u < nb_vertex; u++) {
    fprintf(f_out, "%hd %hd %hd", colors[u].r, colors[u].g, colors[u].b) ;
    for (int v = 0; v < nb_vertex; v++) {
      if (g[u][v] == 1)
	fprintf(f_out, " %d", v) ;
    }
    fprintf(f_out, "\n") ;
  }
  fclose(f_out) ;
}






int** graph_output(image img, image img_BW, int height, int width, int threshold,
		     pixel** colors_ret, int* nb_vertex_ret,
		     char* filename) {
  int nb_vertex ;
  list sizes ;


  int** components = compute_components(img_BW, height, width, &nb_vertex, &sizes) ;
  filter_small_components(img_BW, height, width, components, nb_vertex, sizes, threshold) ;
  free_2D((char**)components, height) ;
  components = compute_components(img_BW, height, width, &nb_vertex, &sizes) ;
  /*
  filter_small_components(img_BW, height, width, components, nb_vertex, sizes, threshold) ;
  free_2D((char**)components, height) ;
  components = compute_components(img_BW, height, width, &nb_vertex, &sizes) ;
  */  

  printf("Nb_vertices (including white areas): %d \n", nb_vertex) ;
  fflush(stdout) ;

  pixel* colors = retrieve_colors(img, height, width, components, nb_vertex, sizes) ;
  erase_borders(img_BW, height, width, components) ;
  free_2D((char**)img_BW, height) ;
  

  // free_list(sizes) ;
  monochromize_img(img, height, width, components, nb_vertex, colors) ;
  nb_vertex = remove_white_components(components, height, width, nb_vertex, colors) ;

  printf("Nb_vertices (excluding white areas): %d \n", nb_vertex) ;
  fflush(stdout) ;

  int** g = make_graph(components, height, width, nb_vertex, threshold) ;
  print_graph(nb_vertex, g, colors, filename) ;
  free_2D((char**)g, nb_vertex) ;
  
  *colors_ret = colors ;
  *nb_vertex_ret = nb_vertex ;
  return components ;
}


