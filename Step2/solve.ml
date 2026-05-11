open Read_graph ;;
open Compare_graphs ;;




(* FONCTIONS UTILITAIRES, complètes *)

let rec merge_and_filter u pos l1 l2 =
  match l1 with
  | [] -> l2
  | h::t when h = u || h > pos || List.mem h l2 -> merge_and_filter u pos t l2
  | h::t -> h::(merge_and_filter u pos t l2) ;;


let remove_dupplicates l = List.sort_uniq Stdlib.compare l ;;



(* Pratique pour le debugging *)
let print_graph (names, color, g) =
  for u = 0 to Array.length g - 1 do
    let (r,gr,b) = color.(u) in
    Printf.printf "%d %d (%d, %d, %d) " u names.(u) r gr b ;
    Array.iter (fun v -> Printf.printf "%d " v) g.(u) ;
    print_string "\n"
  done ;;

let print_dico d =
  print_string "{" ;
  Hashtbl.iter (fun (r,g,b) v ->
    Printf.printf "(%d,%d,%d) : %d, " r g b v) d;
  print_string "}\n" ;;
   





(* Pour la fin du Backtracking *)
let print_solution l =
  List.iter (fun (u,(r,g,b)) -> Printf.printf "Colorier le sommet %d en (%d, %d, %d))\n" u r g b) l ;
  print_string "\n" ;;






(* FIN FONCTIONS UTILITAIRES *)








let apply_color c u (names, colors, g) =
  let n = Array.length g in
  assert(Array.length colors = n) ;
  assert(Array.length names = n) ;
  assert(0 <= u && u < n) ;

  (* On commence par créer une permutation qui permet de placer
     en fin de tableau les sommets qui vont disparaitre,
     cad les voisins de u qui sont de couleur c *)
  let perm = Array.make n (-1) in
  let pos = ref (n-1) in
  for i = 0 to Array.length g.(u)-1 do
    let v = g.(u).(i) in
    if colors.(v) = c then begin
      perm.(v) <- !pos ;
      (* Printf.printf "%d %d " v perm.(v);*)
      decr pos
    end
  done;
  (* print_string "\n" ;*)
  let k = ref 0 in
  for i = 0 to n-1 do
    if perm.(i) = -1 then begin
      perm.(i) <- !k ;
      (* Printf.printf "%d %d " i perm.(i); *)
      incr k
    end
  done ;

  assert(!k = !pos+1) ;
  (* Printf.printf "\n%d %d\n" u perm.(u) ; *)
  assert(perm.(u) <= !pos) ;

  (* On applique la permutation au graphe *)
  let (c',g') = permut_graph perm (colors,g) in
  let names' = permut_array perm names in

  (* On retire les sommets qui disparaissent *)
  let g'' = Array.sub g' 0 (!pos+1) in
  let c'' = Array.sub c' 0 (!pos+1) in
  let names'' = Array.sub names' 0 (!pos+1) in

  (* On remplace les sommets disparus par u dans les listes d'adjacence. *)
  for u' = 0 to !pos do
    for i = 0 to Array.length g''.(u') - 1 do
      let v = g''.(u').(i) in
      if v > !pos then
	g''.(u').(i) <- perm.(u)
    done ;
    (* CELA PEUT GENERER DES DOUBLONS DANS LES LISTES D'ADJACENCE !!! *)
    g''.(u') <- Array.of_list (remove_dupplicates (Array.to_list g''.(u')))
  done ;
  
  (* Puis r\'eciproquement, on ajoute dans g.(u) les voisins des sommets qui disparaissent *)
  (* sauf que maintenant u s'appelle perm.(u).
   ET on s'assure que u NE SOIT PAS voisin de lui m\^eme *)
  c''.(perm.(u)) <- c ;
  let list = ref (merge_and_filter perm.(u) !pos (Array.to_list g''.(perm.(u))) [])  in
  for k = n-1 downto !pos+1 do
    list := merge_and_filter perm.(u) !pos (Array.to_list g'.(k)) !list
  done;
  g''.(perm.(u)) <- Array.of_list !list ;



  (* On renvoie le tout *)
  (names'', c'', g'') ;;
  



(* III - BACKTRACKING *)


  

let occ_colors color =
  let d = Hashtbl.create 20 in
  Array.iter
    (fun c -> match Hashtbl.find_opt d c with
    | None -> Hashtbl.add d c 1
    | Some occ -> Hashtbl.replace d c (occ+1)
    ) color ;
  d ;;





let decrement_occ d c qte =
  match Hashtbl.find_opt d c with
  | None -> failwith "Precondition non respectee"
  | Some occ -> begin
      match occ - qte with
      | 0 -> Hashtbl.remove d c
      | k when k > 0 -> Hashtbl.replace d c k
      | _ -> failwith "Precondition non respectee"
  end ;;

let increment_occ d c qte =
  match Hashtbl.find_opt d c with
  | None -> Hashtbl.add d c qte
  | Some occ -> Hashtbl.replace d c (qte + occ) ;;
      

let maj_color d =
  let c = ref (0,0,0) in
  let m = ref 0 in
  Hashtbl.iter (fun k v ->
    if v > !m then begin
      c := k ;
      m := v
    end) d ;
  !c ;;


let weighted_colors d =
  let cpt = ref 0 in
  Hashtbl.iter (fun k v ->
    if v > 1 then cpt := !cpt + 2 else incr cpt) d ;
  !cpt ;;


(* first a greedy algorithm to approx the optimum *)


let deg c u g colors =
  let cpt = ref 0 in
  for i = 0 to Array.length g.(u) - 1 do
    let v = g.(u).(i) in
    if colors.(v) = c then
      incr cpt
  done ;
  !cpt ;;
      

      

let solve_greedy (colors,g) =
  let n = ref (Array.length g) in
  assert(Array.length colors = !n) ;
  (* Noms initial des sommets, pour pouvoir reconstituer la solution *)
  let names = ref (Array.init !n (fun i -> i)) in
  let sol = ref [] in

  let d = occ_colors colors in
  let g = ref g in
  let colors = ref colors in
  let nb_steps = ref 0 in

  while Hashtbl.length d > 1 do

    (* BEGIN DEBUGGING 
    Printf.printf "loop started, nb_steps = %d \n" !nb_steps ;
    print_graph (!names, !colors, !g) ;
    Printf.printf "Current solution : " ;
    print_solution !sol ;
    print_dico d ;

    flush(stdout) ;

    let _ = read_int () in () ;
     END DEBUGGING *)



    
    (* find the best vertex *)
    let m = ref 0 in
    let b_u = ref 0 in
    let b_c = ref !colors.(0) in
    for u = 0 to !n-1 do
      Hashtbl.iter (fun c _ -> 
	let d = deg c u !g !colors in
	if d > !m then begin
	  m := d ;
	  b_u := u ;
	  b_c := c
	end) d
    done ;

    (* Si m = 0 --> graph = independant set
       --> on trouve la couleur majoritaire *)
    (* meilleur solution = colorier tous les autres en c *)
    if !m = 0 then begin
      let c = maj_color d in
      nb_steps := !nb_steps + !n - Hashtbl.find d c ;
      for u = 0 to !n-1 do
	if !colors.(u) <> c then begin
	  sol := (!names.(u), c) :: !sol ;
	  decrement_occ d !colors.(u) 1 ;
	  increment_occ d c 1 ;	    
	end		    
      done
    end
    else begin
      (* Sinon On applique c \`a u *)
      let (names', colors', g') = apply_color !b_c !b_u (!names, !colors, !g) in

    
      (* MAJ solution courante et dictionnaire d *)
      sol := (!names.(!b_u), !b_c) :: !sol ;
      decrement_occ d !colors.(!b_u) 1 ;
      decrement_occ d !b_c (!n - Array.length g' - 1) ;
    
      (* On maj le graphe pour le prochain tour  *)
      n := Array.length g' ;
      g := g' ;
      colors := colors' ;
      names := names' ;
      incr nb_steps
    end
  done ;
  (!nb_steps, List.rev !sol) ;;


      
let solve_backtrack (colors,g) =
  let n = Array.length g in
  assert(Array.length colors = n) ;
  (* Noms initial des sommets, pour pouvoir reconstituer la solution *)
  let names = Array.init n (fun i -> i) in
  let sol = ref [] in

  (* Meilleur solution jusque la : on commence par une approx gloutonne *)
  let (nb_steps_greedy, sol_greedy) = solve_greedy (colors, g) in
  let opt_steps = ref nb_steps_greedy in
  let opt_sol = ref (List.rev sol_greedy) in

  let nb_best_found = ref 0 in


  let d = occ_colors colors in
  
  let rec aux (names, colors, g) nb_steps =

    (* BEGIN DEBUGGING 
    Printf.printf "aux started, nb_steps = %d \n" nb_steps ;
    print_graph (names, colors, g) ;
    Printf.printf "Current solution : " ;
    print_solution !sol ;
    Printf.printf "Optimal solution so far : " ;
    print_solution !opt_sol ;
    print_dico d ;

    flush(stdout) ;

    let _ = read_int () in () ;
    END DEBUGGING *)
    

    
    let n = Array.length g in
    assert(Array.length names = n) ;
    assert(Array.length colors = n) ;

    match Hashtbl.length d with
      (* Cas de base positif *)
    | 1 when nb_steps < !opt_steps -> 
	opt_steps := nb_steps ;
	opt_sol := !sol ;
	incr nb_best_found
	    (* Cas de base négatif *)
    | 1 -> ()
	  (* Cas d'échec *)
    | nb_colors when (nb_steps + weighted_colors d - 2 >= !opt_steps) || (nb_steps + nb_colors -1 >= !opt_steps)  -> ()
    | _ ->
	(* Pour chaque sommet, on essaie de lui appliquer la couleur d'un de ses voisins *)
	let edges_exist = ref false in
	
	for u = 0 to n-1 do
	  let possible_colors =
	    assert(Array.for_all (fun v -> 0 <= v && v < n) g.(u)) ;
	    remove_dupplicates (Array.to_list (Array.map (fun v -> colors.(v)) g.(u))) in
	  List.iter (fun c ->
	    let (names', colors', g') = apply_color c u (names, colors, g) in
	    (* MAJ solution courante et dictionnaire d *)
	    sol := (names.(u), c) :: !sol ;
	    decrement_occ d colors.(u) 1 ;
	    decrement_occ d c (n - Array.length g' - 1) ;
	    aux (names', colors', g') (nb_steps+1) ;
	    (* On efface nos traces *)
	    sol := List.tl !sol ;
	    increment_occ d colors.(u) 1 ;
	    increment_occ d c (n- Array.length g' - 1)	
		    ) possible_colors ;
	  if possible_colors != [] then
	    edges_exist := true ;
	done ;
	if not !edges_exist then begin
	  let c = maj_color d in
	  let nb_occ = Hashtbl.find d c in
	  (* meilleur solution = colorier tous les autres en c *)
	  let nb_steps_this_branch = nb_steps + n - nb_occ in
	  if nb_steps_this_branch < !opt_steps then begin
	    incr nb_best_found ;
	    opt_steps := nb_steps_this_branch ;
	    opt_sol := !sol ;
	    for u = 0 to n-1 do
	      if colors.(u) <> c then
		opt_sol := (names.(u), c) :: !opt_sol
	    done ;
	  end
	end
  in aux (names, colors, g) 0 ;
  Printf.printf "Greedy (%d moves) is Beat %d times.\n" nb_steps_greedy !nb_best_found ;
  !opt_steps, List.rev !opt_sol ;;







let inverse_perm p u =
  let n = Array.length p in
  let i = ref 0 in
  while !i < n && p.(!i) <> u do
    incr i
  done ;
  !i ;;


let display_solution graph sol =
  let n = Array.length (fst graph) in
  let names = Array.init n (fun i -> i) in
  let s = ref sol in
  let g = ref (names, fst graph, snd graph) in
  print_graph !g ;
  while !s <> [] do
    let (u,c) = List.hd !s in
    let (r,gr,b) = c in
    s := List.tl !s ;
    let (names,_,_) = !g in
    let origin_u = inverse_perm names u in
    g := apply_color c origin_u !g ;
    Printf.printf "Applying color (%d,%d,%d) to vertex %d (aka %d) we get : \n" r gr b u origin_u ; 
    print_graph !g
  done ;;



let main () =
  if Array.length Sys.argv <> 2 then
    raise (Invalid_argument ("Wrong number of arguments for command " ^ Sys.argv.(0))) ;
  let filename = Sys.argv.(1) in
  let g1 = read_graph filename  in 
  let nb_steps,sol = solve_backtrack g1 in 
  print_solution sol ;;
    

main () ;;
