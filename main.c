#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG = 0;




typedef struct node{
    int distance;
    struct node* left;
    struct node* right;
    struct node* parent;
    int* vehicles; // array dinamico in cui ogni veicolo è identificato con la sua autonomia
    int num_vehicles; // lunghezza array dinamico 
} station;

station* Stations = NULL;


void station_constructor(station* station, int distance){
    station->distance = distance;
    station->left = NULL;
    station->right = NULL;
    station->parent = NULL;
    station->vehicles = NULL;
    station->num_vehicles = 0;
}


// ------------------------------------------------ ALBERI BINARI DI RICERCA ----------------------------------------------------------------

station* Tree_search(station* x, int distance){
    if (x == NULL || distance == x->distance){
        return x;
    }
    if (distance < x->distance){
        return Tree_search(x->left, distance);
    } else {
        return Tree_search(x->right, distance);
    }
}


int Tree_insert(station* element){
    station* x = Stations;
    station* y = NULL;
    while (x != NULL){
        y = x;
        if (element->distance < x->distance){
            x = x->left;
        } else if (element->distance > x->distance){      // DA QUI HO AGGIUNTO
            x = x->right;
        } else {
            return 1;
        }
    }
    element->parent = y;
    if (y == NULL){
        Stations = element;
    } else if (element->distance < y->distance){
        y->left = element;
    } else if (element->distance > y->distance){                                                // DA QUI HO AGGIUNTO
        y->right = element;
    } else {
        return 1;
    }
    return 0;
}


void Transplant(station* u, station* v){
    if (u->parent == NULL){
        Stations = v;
    } else if (u == u->parent->left){
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL){
        v->parent = u->parent;
    }
}

station* Tree_min(station* x){
    while (x->left != NULL){
        x = x->left;
    } return x;
}

void Tree_delete(station* element){    // given element
    if (element->left == NULL){
        Transplant(element, element->right);
    } else if (element->right == NULL){
        Transplant(element, element->left);
    } else {
        station* y = Tree_min(element->right);
        if (y->parent != element){
            Transplant(y, y->right);
            y->right = element->right;
            y->right->parent = y;
        }
        Transplant(element, y);
        y->left = element->left;
        y->left->parent = y;
    }
    free(element->vehicles);
    free(element);
    
}


int* Inorder_tree_walk_array_left_right(station* x, int* array, int *len, int min_dist, int max_dist){
    if (x != NULL){
        array = Inorder_tree_walk_array_left_right(x->left, array, len, min_dist, max_dist);
        if (x->distance >= min_dist && x->distance <= max_dist){
            array[(*len)] = x->distance;
            array[(*len)+1] = *(x->vehicles);
            (*len) += 2;
            array = realloc(array, ((*len)+2)*sizeof(int));
        }
        array = Inorder_tree_walk_array_left_right(x->right, array, len, min_dist, max_dist);
        return array;
    } else {
        return array;
    }
}

int* Inorder_tree_walk_array_right_left(station* x, int* array, int *len, int min_dist, int max_dist){
    if (x != NULL){
        array = Inorder_tree_walk_array_right_left(x->right, array, len, min_dist, max_dist);
        if (x->distance >= min_dist && x->distance <= max_dist){
            array[(*len)] = x->distance;
            array[(*len)+1] = *(x->vehicles);
            (*len) += 2;
            array = realloc(array, ((*len)+2)*sizeof(int));
        }
        array = Inorder_tree_walk_array_right_left(x->left, array, len, min_dist, max_dist);
        return array;
    } else {
        return array;
    }
}




// ----------------------------------------------------------------------------------------------------------------


// ------------------------------------------------ MAX-HEAP ----------------------------------------------------------------


void Max_heapify(int* A, int len, int i){  // serve per sistemare dopo aver aggiunto un elemento
    int l = 2*i+1; //left
    int r = 2*i+2; // right
    int max;
    if (l < len && A[l]>A[i]){
        max = l;
    } else {
        max = i;
    }
    if (r < len && A[r] > A[max]){
        max = r;
    }
    if (max != i){
        int tmp = A[i];
        A[i] = A[max];
        A[max] = tmp;
        if (((i-2)/2)>=0){
            Max_heapify(A, len, (i-1)/2);
        }
    }
}



void Max_heapify_top_down(int* A, int len, int i){ // serve per sistemare dopo aver rimosso un elemento
    int l = 2*i+1; //left
    int r = 2*i+2; // right
    int max;
    char go;
    if (l < len && A[l]>A[i]){
        max = l;
        go = 'l';
    } else {
        max = i;
    }
    if (r < len && A[r] > A[max]){
        max = r;
        go = 'r';
    }
    if (max != i){
        int tmp = A[i];
        A[i] = A[max];
        A[max] = tmp;
        if (go == 'r'){
            Max_heapify_top_down(A, len, 2*i+2);
        } else {
            Max_heapify_top_down(A, len, 2*i+1);
        }
    }
}





int* Heap_add(int autonomy, int* vehicles, int *num_vehicles){
    if (*num_vehicles == 0){
        vehicles = malloc(sizeof(int));
        *vehicles = autonomy;
        (*num_vehicles) ++;
    } else {
        (*num_vehicles) ++;
        vehicles = realloc(vehicles, *num_vehicles*sizeof(int));
        vehicles[*num_vehicles-1] = autonomy;
        Max_heapify(vehicles, *num_vehicles, (*num_vehicles-2)/2);   
    }
    return vehicles;
}


// -----------------------------------------------HELPERS-----------------------------------------------------------------



int * increase_counter (int *counter, int *limits, int poss_size, int * finished){
    int c = 0;
    while (1){
        if (c == poss_size){
            *finished = 1;
            break;
        }
        if (counter[c] < limits[c]){
            counter[c]++;
            break;
        } else {
            counter[c] = 0;
            c ++;
        }
    }
    return counter;
}


int confronta(int *res2, int *res_init, int res_size){
    for (int i = 0; i<res_size; i++){
        if (res2[i]<res_init[i]){
            return 1;
        }
    }
    return 0;
}





int * pianifica_helper(int *found, int *res_size, int *res, int *array, int len, int dist1, int dist2, int **possibilities1, int **possibilities2, int **possibilities3,  int *poss_size){

    int start = 0, dist = 0, max_index = 0, max_sum = 0, cur_index = 0, exists = 0;
    for (int i = 0, j = 1; i < len; i+=2, j+=2){
        if (start){
            dist += array[i-2] - array[i];
            
            if (dist > array[cur_index+1]){  // non posso andare oltre con l'autonomia che ho e scelgo la stazione migliore
                if (exists){
                    if (i - 2 - max_index > 0){ // se ci sono possibili stazioni in cui posso amdare dopo ma non massime
                        *(*(possibilities1)+ (*(poss_size))) = max_index; // beginning index in array
                        *(*(possibilities2)+ (*(poss_size))) =  (i - 2 - max_index)/2; // numbers of elements
                        *(*(possibilities3)+ (*(poss_size))) = *res_size; // index in res
                        *(poss_size) += 1;
                        *possibilities1 = realloc (*possibilities1, ( *(poss_size)+1 ) * sizeof(int));
                        *possibilities2 = realloc (*possibilities2, ( *(poss_size)+1 ) * sizeof(int));
                        *possibilities3 = realloc (*possibilities3, ( *(poss_size)+1 ) * sizeof(int));
                    }
                    i = max_index;
                    cur_index = i;
                    j = i+1;
                    res[*res_size] = array[i];
                    (*res_size)++;
                    res = realloc(res, ((*res_size)+1)*sizeof(int));
                    exists = 0;
                    dist = 0;
                    max_sum = 0;
                } else {
                    printf("nessun percorso");
                    break;
                }
            } else {
                exists = 1;
            }
            
            
            
            if (array[i] == dist2){ // fine DA RENDERE PIU SEMPLICE, FOR NON SERVE CREDO 
                if (array[cur_index+1] >= array[cur_index]-dist2){ // se la'autonomia della stazione in cui mi trovo mi consente di fare la strada per arrivare alla fine 
                    res[*res_size] = dist2;
                    (*res_size)++;
                    *found = 1;
                    break;
                } else {
                    for (int k = cur_index+2; k < i && !found; k++){
                        if (array[k+1]>=dist2-array[k]){

                            res[*res_size] = array[k];
                            (*res_size)++;
                            res = realloc(res, (*(res_size)+1)*sizeof(int));

                            res[*res_size] = array[i];
                            (*res_size)++;

                            *found = 1;
                        }
                    }
                    if (*found){
                        break;
                    } else {
                        printf("nessun percorso");
                        break;
                    }
                }
            }

            if (exists && array[j]+dist >= max_sum){
                max_sum = array[j] + dist;
                max_index = i;
            }

        } else if (array[i] == dist1){ // inizio
            start = 1;
            max_index = i;
            max_sum = dist + array[j];
            dist = 0;
            cur_index = i;
            res[*res_size] = dist1;
            (*res_size)++;
            res = realloc(res, (*(res_size)+1)*sizeof(int));
        }
    }
    return res;
}



// ----------------------------------------------------------------------------------------------------------------


// ------------------------------------------------ FUNZIONI ----------------------------------------------------------------

void AggiungiStazione (int distance, int num_vehicles, int autonomies[512]) {
    station* element = (station*) malloc(sizeof(station));
    station_constructor(element, distance);
    int res = Tree_insert(element); // inserisci stazione
    int added = 0;
    if (res == 0){
        printf("aggiunta\n");
        added = 1;
    } else {
        printf("non aggiunta\n");
        free(element);
    }
    if (added){
        if (num_vehicles == 0){
	    element -> vehicles = malloc(sizeof(int));
	    *(element -> vehicles) = 0;
	    element -> num_vehicles = 1;
	} else {
	    for (int i = 0; i < num_vehicles; i++){
   	        element->vehicles = Heap_add(autonomies[i], element->vehicles, &element->num_vehicles); // inserisci tutte le auto
	    }
	}
    }

}


void DemolisciStazione (int distance) {
    station* res = Tree_search(Stations, distance);  // cerca stazione
    if (res == NULL){
        printf("non demolita\n");
    } else {
        Tree_delete(res);    // se esiste, cancella la stazione
        printf("demolita\n");
    }
}


void AggiungiAuto (int distance, int autonomy){
    station* res = Tree_search(Stations, distance); // cerca stazione
    if (res == NULL){
        printf("non aggiunta\n");
    } else {
        res->vehicles = Heap_add(autonomy, res->vehicles, &res->num_vehicles); // aggiunge auto nella stazione
        printf("aggiunta\n");
    }
}


void RottamaAuto (int distance, int autonomy){
    int found = 0, index = -1;
    station* res = Tree_search(Stations, distance); // cerca la stazione
    if (res == NULL){
        printf("non rottamata\n");
    } else {
        for (int i = 0; i < res->num_vehicles && !found; i++){ // cerca la macchina nella stazione, se la trova
            if (autonomy == res->vehicles[i]){
                index = i;
                found = 1;
            }
        }
        if (!found){
            printf("non rottamata\n");
        } else {
            res->vehicles[index] = res->vehicles[res->num_vehicles-1];
            res->num_vehicles --;
            res->vehicles = realloc(res->vehicles,res->num_vehicles*sizeof(int));
            Max_heapify_top_down(res->vehicles, res->num_vehicles, index);   
            printf("rottamata\n");
        }
    }
}



void PianificaPercorso (int dist1, int dist2) {
    if (dist1 == dist2){
        printf("%d\n", dist1);
    } else if (dist1 < dist2){ // da sinistra verso destra , inorder tree-walk left, parent, right
	int* array = malloc(2*sizeof(int));
        int len = 0;
        array = Inorder_tree_walk_array_right_left(Stations, array, &len, dist1, dist2);

        int *res = malloc(sizeof(int));
        int res_size = 0, start = 0, dist = 0, exists = 0, found = 0;

        for (int i = 0, j=1; i < len; i+=2, j+=2){
            if (start){
                dist += array[i-2] - array[i];
                if (dist > array[j]){ // cambio macchina 
                    int inner_found = 0;
                    for (int h = i+2, k = j+2; k < len; h+=2, k+=2){ // scorro fino in fondo
                        dist+=array[h-2]-array[h];
                        if (array[h] == dist2){
                            break;
                        } 
                        if (array[k] >= dist){
                            inner_found=1;
                            i = h;
                            j = k;
                        }
                    }
                    if (inner_found){
                        res[res_size] = array[i];
                        res_size++;
                        res = realloc(res, (res_size+1)*sizeof(int));
                        dist = 0;
                        exists = 0;
                    }
                    else {
                        if (!exists){
                            printf("nessun percorso");
                            break;
                        } else { // cambio macchina
                            i -= 2;
                            j -= 2;
                            res[res_size] = array[i];
                            res_size++;
                            res = realloc(res, (res_size+1)*sizeof(int));
                            dist = 0;
                            exists = 0;
                        }
                    }
                } else { 
                    exists = 1;
                    // all good
                }
                if (array[i]==dist1){ // finito
                    res[res_size] = array[i];
                    res_size ++;
                    found = 1;
                    break;
                }
            }
            if (array[i] == dist2){
                start = 1;
                res[res_size] = dist2;
                res_size++;
                res = realloc(res, (res_size+1)*sizeof(int));
                dist = 0;
            }
        }

        if (found){
            for (int i=res_size-1; i>=0; i--){
                if (i != 0){
                    printf("%d ",res[i]);
                } else {
                    printf("%d",res[i]);
                }
            }
        }
        printf("\n");
    	free(array);
    	free(res);
    
    } else { // da destra verso sinistra, inorder tree-walk right, parent, left
        int* array = malloc(2*sizeof(int));
        int len = 0;
        array = Inorder_tree_walk_array_right_left(Stations, array, &len, dist2, dist1);


        int * res = malloc(sizeof(int));
        int * possibilities1 = malloc(sizeof(int));
        int * possibilities2 = malloc(sizeof(int));
        int * possibilities3 = malloc(sizeof(int));
        int res_size = 0, found = 0, poss_size = 0, finished = 0;
        res = pianifica_helper(&found, &res_size, res, array, len, dist1, dist2, &possibilities1, &possibilities2, &possibilities3, &poss_size);


        int * counter = malloc(poss_size * sizeof(int));
        for (int i = 0; i < poss_size; i++) {
            counter[i] = 0;
        }


        counter = increase_counter(counter, possibilities2, poss_size, &finished);


        while (!finished) { // ogni ciclo controlla una permutazione
            int change = 0;
            for (int i = 0; i < poss_size; i++){ // ogni ciclo gestisce un elemento della permutazione
                int dist = array[possibilities1[i] + counter[i]*2] - res[possibilities3[i]+1];
                if ( array[possibilities1[i] + (counter[i]*2) + 1] >= dist){
                    //good
                    change = 1;
                } else {
                    change = 0;
                    break;
                }
            }
            if (change){
                int * res2 = malloc(res_size*sizeof(int));
                for (int j = 0; j < res_size; j++){
                    res2[j] = res[j];
                }
                for (int j = 0; j < poss_size; j++){
                    res2[possibilities3[j]] = array[possibilities1[j] + counter[j]*2];
                }
                change = confronta(res2, res, res_size);
                if (change){
		    free(res);
                    res = res2;
                } else {
                    free(res2);
                    
                }
            }
            
            counter = increase_counter(counter, possibilities2, poss_size, &finished);
        }

        
        if (found){
            for (int i = 0; i < res_size; i++){
                if (i != res_size-1){
                    printf("%d ",res[i]);
                } else {
                    printf("%d",res[i]);
                }
            }
        }
        printf("\n");
    	free(res);
    	free(array);
    	free(possibilities1);
    	free(possibilities2);
    	free(possibilities3);
    	free(counter);
    }
}


// // -----------------------------------------------------MAIN-----------------------------------------------------------

void Dismantle(station* x){
    if (x != NULL){
        Dismantle(x->left);
        station* next = x->right;
        free(x->vehicles);
        free(x);
        Dismantle(next);
    }
}




int main(){
    char input[1024]; // probabilmente serve di più
    int res;
    res = scanf("%s ", input);
    int distance=0, vehicles_num=0, autonomy=0, dist1=0, dist2=0, autonomies[512];
    for (int i=0; i<512; i++){
        autonomies[i] = 0;
    }
    do {
        if (!strcmp(input, "aggiungi-stazione")){
            res = scanf("%d %d", &distance, &vehicles_num);
            for (int i = 0; i < vehicles_num; i++){
                res = scanf(" %d", &autonomies[i]);
            }
            AggiungiStazione(distance, vehicles_num, autonomies);
        } else if (!strcmp(input, "demolisci-stazione")){
            res = scanf("%d ", &distance);
            DemolisciStazione(distance);
        } else if (!strcmp(input, "aggiungi-auto")){
            res = scanf("%d %d ", &distance, &autonomy);
            AggiungiAuto(distance, autonomy);
        } else if (!strcmp(input, "rottama-auto")){
            res = scanf("%d %d ", &distance, &autonomy);
            RottamaAuto(distance, autonomy);
        } else if (!strcmp(input, "pianifica-percorso")){
            res = scanf("%d %d ", &dist1, &dist2);
            PianificaPercorso(dist1, dist2);
        }
        res = scanf ("%s ", input);
        if (res == EOF){
            break;
        }
    } while (!feof(stdin));
   

    
    Dismantle(Stations);
    



    return 0;
}
