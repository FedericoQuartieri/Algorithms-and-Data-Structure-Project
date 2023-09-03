#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


void Transplant(station* u, station* v){  // serve per tree_delete
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
        } else if (element->distance > x->distance){      // Da qui ho aggiunto rispetto al Cormen
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
    } else if (element->distance > y->distance){           // Da qui ho aggiunto rispetto al Cormen
        y->right = element;
    } else {
        return 1;
    }
    return 0;
}


void Tree_delete(station* element){
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
        vehicles = (int*) malloc(sizeof(int));
        if (vehicles == NULL){
            fprintf(stderr, "ERRORE");
            exit(-1);
        }
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


void Dismantle(station* x){
    if (x != NULL){
        Dismantle(x->left);
        station* next = x->right;
        free(x->vehicles);
        free(x);
        Dismantle(next);
    }
}


int * pianifica_helper(int *found, int *res_size, int *res, int *array, int len, int dist1, int dist2, int *poss1, int *poss2, int *poss3){
    int start = 0, dist = 0, max_index = 0, max_sum = 0, cur_index = 0, exists = 0;
    for (int i = 0, j = 1; i < len; i+=2, j+=2){
        if (start){
            dist += array[i-2] - array[i];
            
            if (dist > array[cur_index+1]){  // non posso andare oltre con l'autonomia che ho e scelgo la stazione migliore
                if (exists){
                    if (i - 2 - max_index > 0){ // se ci sono possibili stazioni in cui posso amdare dopo ma non massime
                        *poss1 =  max_index; // beginning index in array
                        *poss2 = (i - 2 - max_index)/2;// numbers of elements
                        *poss3 = *res_size; // index in res
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


// ------------------------------------------------ FUNZIONI PRINCIPALI ----------------------------------------------------------------


void AggiungiStazione (int distance, int num_vehicles, int autonomies[512]) {
    station* element = (station*) malloc(sizeof(station));
    if (element == NULL){
        fprintf(stderr, "ERRORE");
        exit(-1);
    }
    station_constructor(element, distance);
    int res = Tree_insert(element); // inserisci stazione
    if (res == 0){
        printf("aggiunta");
        if (num_vehicles == 0){
            element -> vehicles = malloc(sizeof(int));
            if (element -> vehicles == NULL){
                fprintf(stderr, "ERRORE");
                exit(-1);
            }
            *(element -> vehicles) = 0;
            element -> num_vehicles = 1;
        } else {
            for (int i = 0; i < num_vehicles; i++){
                element->vehicles = Heap_add(autonomies[i], element->vehicles, &element->num_vehicles); // inserisci tutte le auto
            }
        }
    } else {
        printf("non aggiunta");
        free(element);
    }


}


void DemolisciStazione (int distance) {
    station* res = Tree_search(Stations, distance);  // cerca stazione
    if (res == NULL){
        printf("non demolita");
    } else {
        Tree_delete(res);    // se esiste, cancella la stazione
        printf("demolita");
    }
}


void AggiungiAuto (int distance, int autonomy){
    station* res = Tree_search(Stations, distance); // cerca stazione
    if (res == NULL){
        printf("non aggiunta");
    } else {
        res->vehicles = Heap_add(autonomy, res->vehicles, &res->num_vehicles); // aggiunge auto nella stazione
        printf("aggiunta");
    }
}


void RottamaAuto (int distance, int autonomy){
    int found = 0, index = -1;
    station* res = Tree_search(Stations, distance); // cerca la stazione
    if (res == NULL){
        printf("non rottamata");
    } else {
        for (int i = 0; i < res->num_vehicles && !found; i++){ // cerca la macchina nella stazione, se la trova
            if (autonomy == res->vehicles[i]){
                index = i;
                found = 1;
            }
        }
        if (!found){
            printf("non rottamata");
        } else {
            res->vehicles[index] = res->vehicles[res->num_vehicles-1];
            res->num_vehicles --;
            res->vehicles = realloc(res->vehicles,res->num_vehicles*sizeof(int));
            Max_heapify_top_down(res->vehicles, res->num_vehicles, index);   
            printf("rottamata");
        }
    }
}


void PianificaPercorso (int dist1, int dist2) {
    if (dist1 == dist2){
        printf("%d", dist1);

    } else if (dist1 < dist2){ // da sinistra verso destra
	    int* array = malloc(2*sizeof(int));
        if (array == NULL){
            fprintf(stderr, "ERRORE");
            exit(-1);
        }

        int len = 0;
        array = Inorder_tree_walk_array_right_left(Stations, array, &len, dist1, dist2);

        int *res = (int*) malloc(sizeof(int));
        if (res == NULL){
            fprintf(stderr, "ERRORE");
            exit(-1);
        }

        int res_size = 0, dist = 0, found = 0;
        res[res_size] = dist2;
        res_size++;
        res = realloc(res, (res_size+1)*sizeof(int));

        for (int i = 0, j=1; i < len; i+=2, j+=2){
            int index = 0;
            for (int h = i+2, k = j+2; h < len; h+=2, k+=2){ // scorro fino in fondo
                dist += array[h-2]-array[h];
                if (array[h] == dist1){ // uguale a h = len-1 in teoria
                    if (array[k] >= dist){ // fine
                        res[res_size] = array[h];
                        res_size ++;
                        found = 1;
                        i = len;
                        break; // non serve ma vabbe
                    } else { // uso l'ultimo posto dove sapevo di poter andare oppure non ci sono percorsi
                        if (index == 0){
                            printf("nessun percorso");
                            i = len;
                            break;
                        } else {
                            i = index-2;
                            j = i+1;
                            res[res_size] = array[index];
                            res_size++;
                            res = realloc(res, (res_size+1)*sizeof(int));
                            dist = 0;
                            break; // non serve ma vabbe
                        }
                    }
                } else {
                    if (array[k] >= dist){ // posso arrivare fino a qui
                        index = h;
                    }
                }
            }
            if (array[i] == dist1){ // fine, uguale a i = len-1 in teoria
                if (!found){
                    found = 1;
                    res[res_size] = array[i];
                    res_size ++;
                }
                break; // non serve ma vabbe
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

        free(array);
        free(res);

    } else { // da destra verso sinistra (dist2 < dist1)

        int* array = (int*) malloc(2*sizeof(int));
        if (array == NULL){
            fprintf(stderr, "ERRORE");
            exit(-1);
        }

        int len = 0;
        array = Inorder_tree_walk_array_right_left(Stations, array, &len, dist2, dist1);

        int * res = (int*) malloc(sizeof(int));
        if (res == NULL){
                fprintf(stderr, "ERRORE");
                exit(-1);
        }

        int * res_f = (int*) malloc(sizeof(int));
        if (res_f == NULL){
                fprintf(stderr, "ERRORE");
                exit(-1);
        }
        res_f[0] = dist2;

        int res_size = 0, res_f_size = 1, found = 0, poss1 = 0, poss2 = 0, poss3 = 0, *res_final = NULL;
        res = pianifica_helper(&found, &res_size, res, array, len, dist1, dist2, &poss1, &poss2, &poss3);

        if (found){
            while (poss2 != 0){
                int index = poss1;
                for (int i = 0; i <= poss2; i++){ // controllo ultimo elemento
                    if (array[poss1 + i*2] - res[poss3+1] <= array[poss1 + i*2 + 1]){ // se posso andare più avanti di quanto sono adato prima e arrivo cmq alla fine
                        index = poss1 + i*2;
                    }
                }

                for (int i = res_size-2; i > poss3; i --){
                    res_f_size ++;
                    res_f = realloc(res_f, res_f_size*sizeof(int));
                    res_f[res_f_size-1] = res[i];
                }

                res_f_size ++;
                res_f = realloc(res_f, res_f_size*sizeof(int));
                res_f[res_f_size-1] = array[index];

                len = index+2;
                array = realloc(array, (len*2)*sizeof(int));

                res_size = 0;
                res = realloc(res, sizeof(int));
                if (res == NULL){
                        fprintf(stderr, "ERRORE");
                        exit(-1);
                }

                poss1 = 0;
                poss2 = 0;
                poss3 = 0;

                res = pianifica_helper(&found, &res_size, res, array, len, dist1, res_f[res_f_size-1], &poss1, &poss2, &poss3);
            }

            res_final = (int*) malloc((res_size+res_f_size)*sizeof(int));
            if (res_f == NULL){
                    fprintf(stderr, "ERRORE");
                    exit(-1);
            }

            res_size --;
            res = realloc(res, res_size*sizeof(int));

            for (int i = 0; i < res_size; i++){
                res_final[i] = res[i];
            }
            for (int i = res_size, j = res_f_size -1 ; i < res_size+res_f_size; i++, j--){
                res_final[i] = res_f[j];
            }
        }

        if (found){
            for (int i = 0; i < res_f_size + res_size; i++){
                if (i != res_size + res_f_size - 1){
                    printf("%d ", res_final[i]);
                } else {
                    printf("%d", res_final[i]);
                }
            }
        }

        free(res);
        free(array);
        free(res_f);
        free(res_final);
    }
}


// // // -----------------------------------------------------MAIN-----------------------------------------------------------


int main(){
    char input[1024];
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
        res = scanf ("%s", input);
        printf("\n");
        
    } while (res != EOF);

    Dismantle(Stations);
}