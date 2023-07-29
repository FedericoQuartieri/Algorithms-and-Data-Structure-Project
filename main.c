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
}


int* Inorder_tree_walk_array_left_right(station* x, int* array, int *len){
    if (x != NULL){
        array = Inorder_tree_walk_array_left_right(x->left, array, len);
        array[(*len)] = x->distance;
        array[(*len)+1] = *(x->vehicles);
        (*len) += 2;
        array = realloc(array, ((*len)+2)*sizeof(int));
        array = Inorder_tree_walk_array_left_right(x->right, array, len);
        return array;
    } else {
        return array;
    }
}

int* Inorder_tree_walk_array_right_left(station* x, int* array, int *len){
    if (x != NULL){
        array = Inorder_tree_walk_array_right_left(x->right, array, len);
        array[(*len)] = x->distance;
        array[(*len)+1] = *(x->vehicles);
        (*len) += 2;
        array = realloc(array, ((*len)+2)*sizeof(int));
        array = Inorder_tree_walk_array_right_left(x->left, array, len);
        return array;
    } else {
        return array;
    }
}



// void Inorder_left_right(station* x, int dist1, int dist2){
//     if (x != NULL){
//         Inorder_tree_walk(x->left, dist1, dist2);
//         if (x->distance == dist1){
//             if (x->distance + *(x->vehicles)>max_sum){
//                 max_sum = x->distance + *(x->vehicles);
//                 max_autonomy = *(x->vehicles);
//             }
//         }
//         Inorder_tree_walk(x->right, dist1, dist2);
//     }
// }


// ----------------------------------------------------------------------------------------------------------------


// ------------------------------------------------ MAX-HEAP ----------------------------------------------------------------


void Max_heapify(int* A, int len, int i){
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




void Max_heapify_top_down(int* A, int len, int i){
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
        // for (int i = *num_vehicles; i > 0; i-- ){
        //     Max_heapify(vehicles, *num_vehicles, i);
        // }
        Max_heapify(vehicles, *num_vehicles, (*num_vehicles-2)/2);   
    }
    return vehicles;
}


// ----------------------------------------------------------------------------------------------------------------


// ------------------------------------------------ FUNZIONI ----------------------------------------------------------------

void AggiungiStazione (int distance, int num_vehicles, int autonomies[512]) {
    station* element = (station*) malloc(sizeof(station));
    station_constructor(element, distance);
    int res = Tree_insert(element);
    if (res == 0){
        printf("aggiunta\n");
    } else {
        printf("non aggiunta\n");   // ERRORE NON AGGIUNGE ANCHE QUANDO DOVREBBE
    }
    for (int i = 0; i < num_vehicles; i++){
        element->vehicles = Heap_add(autonomies[i], element->vehicles, &element->num_vehicles);
    }
    // for (int i = 0; i < element->num_vehicles; i++){
    //     printf("%d      ", element->vehicles[i]);
    // }

}


void DemolisciStazione (int distance) {
    station* res = Tree_search(Stations, distance);
    if (res == NULL){
        printf("non demolita\n");
    } else {
        Tree_delete(res);
        printf("demolita\n");
    }
}


void AggiungiAuto (int distance, int autonomy){
    station* res = Tree_search(Stations, distance); 
    if (res == NULL){
        printf("non aggiunta\n");
    } else {
        res->vehicles = Heap_add(autonomy, res->vehicles, &res->num_vehicles);
        printf("aggiunta\n");
    }
}


void RottamaAuto (int distance, int autonomy){
    int found = 0, index = -1;
    station* res = Tree_search(Stations, distance);
    if (res == NULL){
        printf("non rottamata\n");
    } else {
        for (int i = 0; i < res->num_vehicles && !found; i++){ // scorre e controlla se è già presente la macchina
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
        array = Inorder_tree_walk_array_right_left(Stations, array, &len);

        for (int i = 0; i < len; i++){
            printf("%d\n",array[i]);
        }

        int *res = malloc(sizeof(int));
        int res_size = 0, start = 0, dist = 0, exists = 0, found = 0;

        for (int i = 0, j=1; i < len; i+=2, j+=2){
            if (start){
                dist += array[i-2] - array[i];
                if (dist > array[j]){ // cambio macchina 
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


    } else { // da destra verso sinistra, inorder tree-walk right, parent, left
        printf("PASS\n");
    }
}


// // ----------------------------------------------------------------------------------------------------------------


int main(){
    char input[1024]; // probabilmente serve di più
    scanf ("%s ", input);
    int distance=0, vehicles_num=0, autonomy=0, dist1=0, dist2=0, autonomies[512];
    for (int i=0; i<512; i++){
        autonomies[i] = 0;
    }
    do {
        if (!strcmp(input, "aggiungi-stazione")){
            scanf("%d %d", &distance, &vehicles_num);
            for (int i = 0; i < vehicles_num; i++){
                scanf(" %d", &autonomies[i]);
            }
            AggiungiStazione(distance, vehicles_num, autonomies);
        } else if (!strcmp(input, "demolisci-stazione")){
            scanf("%d ", &distance);
            DemolisciStazione(distance);
        } else if (!strcmp(input, "aggiungi-auto")){
            scanf("%d %d ", &distance, &autonomy);
            AggiungiAuto(distance, autonomy);
        } else if (!strcmp(input, "rottama-auto")){
            scanf("%d %d ", &distance, &autonomy);
            RottamaAuto(distance, autonomy);
        } else if (!strcmp(input, "pianifica-percorso")){
            scanf("%d %d ", &dist1, &dist2);
            PianificaPercorso(dist1, dist2);
        }
        scanf ("%s ", input);
    } while (!feof(stdin));

    // int* array = malloc(2*sizeof(int));
    // int len = 0;

    // array = Inorder_tree_walk_array_right_left(Stations, array, &len);

    // for (int i = 0; i < len; i++){
    //     printf("%d\n",array[i]);
    // }

    //Inorder_tree_walk(Stations);



    return 0;
}
