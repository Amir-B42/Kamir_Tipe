type permutation = int array ;;

  
val swap : 'a array -> int -> int -> unit
val permut_array : int array -> 'a array -> 'a array 
val permut_graph : permutation -> Read_graph.graph -> Read_graph.graph
val compare : Read_graph.graph -> Read_graph.graph -> permutation

