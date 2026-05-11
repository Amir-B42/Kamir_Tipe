#ifndef GRAPHS_H
#define GRAPHS_H


typedef int vertex ;
typedef int** graph ; // graph = matrice adjacence.



int** graph_output(image img, image img_BW, int height, int width, int threshold,
		     pixel** colors_ret, int* nb_vertex_ret,
		   char* filename) ;



#endif
