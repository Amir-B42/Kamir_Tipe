(* File Format : contains N lines of the form
   R G B u1 u2 u3 ... uk
   that is : integers separated by whitespace
   the three first represent the color (R,G,B) of the vertex
   and u1 .. uk denote the neighbours of the vertex.
   The corresponding graph has N vertices, numbered 0 to N-1.
   The ith line is the adjacency list of the ith vertex.


   Useful Functions  : check documentation to see usage

   open_in, close_in --> 
   input_line
   String.split_on_char
   int_of_string
   Array.of_list


   Constraints :
   - User gets a clear error if file does not exist.
   - User gets a clear error if file does not respect the format, including the line where the error is.
   - Errors are raised using exceptions.

   
 *)


type color = int * int * int ;;
type graph = color array * int array array ;;



let read_graph file_name =
  let f_in = open_in file_name in
  let graph_list = ref [] in
  let color_list = ref [] in
  let line_nb = ref 0 in
  try
    while true do
      let line_raw = input_line f_in in
      let line = String.split_on_char ' ' line_raw in
      begin
      match (List.map int_of_string line) with
      | r :: g :: b :: adj_list -> 
	  graph_list := (Array.of_list adj_list) :: !graph_list ;
	  color_list := (r,g,b) :: !color_list
      | _ -> raise (Invalid_argument ("Format error at line" ^ (string_of_int !line_nb) ^ " : line must have at least 3 integers.")) ;
      end ;
      incr line_nb
    done
  with
  | End_of_file ->
      close_in f_in ;
      (Array.of_list (List.rev !color_list), Array.of_list (List.rev !graph_list))
  | Failure s ->
      close_in f_in ;
      if s = "int_of_string" then
	raise (Invalid_argument ("Format error at line " ^ (string_of_int !line_nb) ^ " : non-numerical char"))
      else
	raise (Failure s)
  | exn ->
      close_in f_in ;
      raise exn ;;
				 
