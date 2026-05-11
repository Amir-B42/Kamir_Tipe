/* BONUS : Application d'une permutation en ESPACE CONSTANT


   Dans cet exercice, on souhaite appliquer une permutation \`a un tableau d'entier a,
   en place, en espace constant.
   La sp\'ecification du programme est donn\'ee sous forme d'un programme : 
  
*/

int* apply_perm(int len, int* a, int* p) {
  // length of p = length of a = len
  int* res = malloc(sizeof(int) * len) ;
  for (int i = 0; i < len; i++) {
    res[p[i]] = a[i] ;
  }
  return res ;
}


/* Vous devez \'ecrire apply_perm_in_place.
Sp\'ecification :
Entr\'ee : int len, int* a, int* p; p est une permutation valide (contient une et une seule fois chaque entier de [| 0, len-1 |]
Effet : le code suivant ne produit pas d'erreur

int* pp = copy_array(len, p) ;
int* r = apply_perm(len, a, p) ;
apply_perm_in_place(len, a, p) ;
assert(are_equal(len, a, r)) ; // i.e. the two codes are equivalent
assert(are_equal(len, p, pp)) ; // i.e. p is not modified
free(r) ;

Et la complexit\'e spatiale de apply_perm_in_place est O(1).

 */

void apply_perm_in_place(int len, int* a, int* p) {
}



/* Indication :

   On peut voir une permutation comme un graphe, dont les cycles \`a support disjoints sont les composantes connexes du graphe.
   Il est ais\'e d'op\'erer la permutation en place au sein d'une composante connexe (en espace constant).
   La difficult\'e est d'identifier si une composante connexe a d\'ej\`a \'et\'e trait\'ee, pour ne pas la traiter deux fois.
   Pour cela, on souhaiterait avoir un tableau visited comme pour les parcours de graphe, mais c'est proscris par la contrainte en espace.
   Il faut donc trouver une autre astuce pour savoir si une composante connexe a d\'ej\`a \'et\'e trait\'ee.

   Indication++ :
   - l'optimisation en espace implique souvent une surconsommation en temps (et r\'eciproquement)
   - il est logique de parcourir le tableau a pour i = 0 \`a len-1, donc au tour i, les j < i ont d\'ej\`a \'et\'e trait\'es.


 */
