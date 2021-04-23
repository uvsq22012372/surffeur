#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAXCHAR 1000000
typedef struct array_Ty array_Ty;
struct array_Ty {
    int sommet;
    struct list_Ty *list;
};

typedef struct list_Ty list_Ty;
struct list_Ty {
    float valeur;
    int sommet;
    struct list_Ty *next;
};


int nbr_Vertices;
int additionalVertices = 0;
char *ptr;
struct list_Ty *list;
struct list_Ty *l;

struct array_Ty* CreateMatrix(int *f, char* filePath){
    printf("Loading Matrix file ....\n");

    FILE *file;
    struct array_Ty* Matrix;
    char line[MAXCHAR];

    file = fopen(filePath, "r");
    if (file == NULL) perror("fopen in gm_write_dot");

    if (fgets(line, MAXCHAR, file) != NULL){
        Matrix = malloc(nbr_Vertices * sizeof(*Matrix));
    }
    fgets(line, MAXCHAR, file);

    printf("Creating Matrix structure in memory ....\n");
    for(int i = 0; i < nbr_Vertices; i++){
        fgets(line, MAXCHAR, file);
        ptr = strtok(line, " ");
        int n = atoi(ptr);
        Matrix[n-1].sommet = n;
        Matrix[n-1].list = NULL;
        ptr = strtok(NULL, " ");

        list = malloc(sizeof(struct list_Ty));
        int s = atoi(ptr);
        if (s == 0 ) f[i] = 1; else f[i] = 0;
        for(int j = 0; j < s; j++){
            if (!Matrix[n-1].list) Matrix[n-1].list = list;
            else {
                list->next = malloc(sizeof(struct list_Ty));
                list = list->next;
            }
            ptr = strtok(NULL, " ");
            list->sommet = atoi(ptr);
            ptr = strtok(NULL, " ");
            list->valeur = atof(ptr);
            list ->next = NULL;
        }
    }
    fclose(file);

    printf("Matrix Created successfully ....\n");
    return Matrix;
}

void PrintMatrix(array_Ty* matrix, int size){
    int i = 0;
    while (matrix[i].sommet <= size){
        l = matrix[i].list;
        printf("-%d-", matrix[i].sommet);
        while (l != NULL){
            printf(" |%d <-> %.4f| ", l->sommet, l->valeur);
            l = l->next;
        }
        printf("\n\n");
        i++;
    }
}

void PrintVector(float *vct, int size){
    for(int i = 0; i < size; i++){
        printf("%f - ", vct[i]);
    }
    printf("\n");
}

int get_nbr_Vrtc(char* filePath){
    FILE *file;
    char line[MAXCHAR];
    char root[50];

    file = fopen(filePath, "r");
    if (file == NULL) perror("fopen in gm_write_dot");

    if (fgets(line, MAXCHAR, file) != NULL){
        return atoi(line);
    }
}

void *initializeVector(float *vector, float valIni, int size){
    printf("Initializing parameters\n");
    if (valIni == -1){
        float a = (float )1/(float)(nbr_Vertices + additionalVertices);
        for (int i = 0; i < size; i++) vector[i] = a;
    } else for (int i = 0; i < nbr_Vertices; i++) vector[i] = valIni;
}

void multiplyVectorByMatrix(array_Ty* matrix, const float *vector, float *result){
    for (int i = 0; i < nbr_Vertices; ++i) result[i] = 0;
    for (int i = 0; i < nbr_Vertices; ++i) {
        l = matrix[i].list;
        while (l != NULL) {
            result[l->sommet - 1] += vector[i] * l->valeur;
            l = l->next;
        }
    }
}

float multiplyVectorByVector(const float* X, const int* f){
    float sum = 0;
    for (int i = 0; i < nbr_Vertices; ++i){
        sum += X[i] * (float)f[i];
    }
    return sum;
}

void multiplyConstByMatrix(array_Ty* matrix, float val){
    for (int i = 0; i < nbr_Vertices; ++i) {
        l = matrix[i].list;
        while (l != NULL) {
            l ->valeur *= val;
            l = l->next;
        }
    }
}

void addConstToVector(float *vector, float val){
    for (int i = 0; i < nbr_Vertices; ++i){
        vector[i] += val;
    }
}

float getDiffVectors(int size, float v1[], float v2[]){
    float Diff = 0;
    for (int i = 0; i < size; ++i) {
        Diff += fabsf(v1[i]-v2[i]);
    }
    return Diff;
}

void copyVectors(int size, float *v1, float *v2){
    for(int i = 0; i < size; ++i){
        v1[i] = v2[i];
        v2[i] = 0;
    }
}

void convergence(struct array_Ty *Matrix, float * Vector, float *result, int *f, float epsilon, float alpha){
    int itr = 0;
    float inter = 0;
    float diff = 10;
    multiplyConstByMatrix(Matrix, alpha);
    while (diff > epsilon){
        multiplyVectorByMatrix(Matrix, Vector, result);
        inter = multiplyVectorByVector(Vector, f);
        inter *= alpha/(float )nbr_Vertices;
        inter += (1-alpha)/(float)nbr_Vertices;
        addConstToVector(result, inter);
        itr++;
        printf("******************* Iteration Num : %d\n", itr);
        diff = getDiffVectors(nbr_Vertices, Vector, result);
        printf("Epsilon = %f\n ", diff);
        copyVectors(nbr_Vertices, Vector, result);
    }
}



int main() {
    //---------------------Déclaration & initialisation-----------------------------------------------------------------
    clock_t t;
    char* filePath = "C:\\Users\\OsmOlr\\Desktop\\mod&sim_data\\wb-edu.txt";
    float alpha = 0.85f;
    nbr_Vertices = get_nbr_Vrtc(filePath);

    t = clock();
    //---------------------surfer aléatoire-----------------------------------------------------------------------------

    int *f = malloc(nbr_Vertices * sizeof(int));
    float *X = malloc(nbr_Vertices * sizeof(float ));
    float *result = malloc(nbr_Vertices * sizeof(float));

    array_Ty* Matrix = CreateMatrix(f, filePath);
    initializeVector(X, -1, nbr_Vertices);

    //------------------------------------------------------------------------------------------------------------------
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("\nLa création de structure de la matrice a pris %f seconds\n", time_taken);
    t = clock();
    //------------------------------------------------------------------------------------------------------------------

    convergence(Matrix, X, result, f, 0.000001f, alpha);

    //---------------------fin surfer aléatoire-------------------------------------------------------------------------
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("\nLa convergence a pris %f seconds\n", time_taken);

//    printf("Relevance vector : ");
//    PrintVector(X, nbr_Vertices);

}