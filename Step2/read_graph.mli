

type color = int * int * int ;;
type graph = color array * int array array ;;

val read_graph : string -> graph
