#ifndef ANALYSIS_H
#define ANALYSIS_H

static pixel Vert_1 =  { 72,155,129} ;
static pixel Orange_1 ={ 234,120,57} ;
static pixel Bleu_1 = { 30,60,80} ;
static pixel Jaune_1 ={ 216,177,73} ;

static pixel Noir_2 =  {53,48,27} ;
static pixel Vert_2 =  {111,188,153} ;
static pixel Rouge_2 = { 211,73,73} ;

static pixel Rouge_3 =  {165,51,75} ;
static pixel Jaune_3 =  {214,133,55} ;
static pixel Bleu_3 =  {24,57,70} ;
static pixel Vert_3 =  {145,212,180} ;

static pixel Bleu_4 =  {104,189,204} ;
static pixel Jaune_4 = { 229,206,101} ;
static pixel Rose_4 =  {197,90,132} ;

static pixel Beige_5 = {222,209,147} ;
static pixel Vert_5 = {46,77,67} ;
static pixel Rouge_5 = { 207,73,62} ;
static pixel Kaki_5 =  {178,189,141} ;

static pixel red_6 = { 119,36,60 } ;
static  pixel green_6 = { 80, 156,145 } ;
static  pixel blue_6 = { 40, 82,94} ;
static  pixel yellow_6 = { 232, 182,91} ;


int distance(pixel p1, pixel p2) ;
pixel** detect_contour(pixel** img, int height, int width, int threshold) ;
pixel** remove_isolated_pixels(pixel** img_BW, int height, int width, int radius, int prop) ;

// void monochromize_img(pixel** img, int height, int width, int radius, int len, pixel* reference);

#endif
