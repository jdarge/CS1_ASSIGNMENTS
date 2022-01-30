// REQUIRED LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// LEAK DETECTOR
#include "leak_detector_c.h"

// COORDINATE STRUCT
typedef struct coord {
    int x;
    int y;
    double distance;
} coord;

// IMPORTANT FILE DATA
typedef struct fileData {
    // REPLACES GLOBAL ORIGIN COORDS
    int x;
    int y;

    int monster_count;
    int points_count;
    int t;// THRESHOLD
} fileData;

// DATA PROTOTYPES
fileData *ReadInit(FILE *in);//
coord **ReadData(FILE *in, fileData *store);//
int compareTo(coord *ptrPt1, coord *ptrPt2);//
int binSearch(coord *check, coord **points, int l, int r);//

// SORT PROTOTYPES
void sort(fileData *store, coord **points);//
void insertionSort(coord **points, fileData *store);//
void mergeSort(coord **points, int left_idx, int right_idx);//
void merge(coord **points, int left_idx, int midpoint_idx, int right_idx);//

// GENERAL PROTOTYPES
void display(FILE *out, coord **points, fileData *store);//
void freeData(coord **points, fileData *store);//
void findDistance(coord **points, fileData *store);//

int main(void) {

    // CHECKS FOR A MEMORY LEAK AT THE END OF THE PROGRAM
    atexit(report_mem_leak);

    // FILE DATA HANDLING
    FILE *in = fopen("in.txt", "r");
    FILE *out = fopen("out.txt", "w");
    fileData *store = ReadInit(in);
    coord **points = ReadData(in, store);

    findDistance(points, store);

    sort(store, points);

    display(out, points, store);

    fclose(in);
    fclose(out);

    freeData(points, store);
}

void display(FILE *out, coord **points, fileData *store) {

    int result;
    int total_size = store->monster_count + store->points_count;

    for (int i = 0; i < store->monster_count; i++) {
        // PRINTING ALL SORTED POINTS
        printf("%d %d\n", points[i]->x, points[i]->y);
        fprintf(out, "%d %d\n", points[i]->x, points[i]->y);
    }

    for (int i = store->monster_count; i < total_size; i++) {
        // BINARY SEARCH TO RETURN THE INDEX + 1 IF THERE"S A RESULT
        result = binSearch(points[i], points, 0, store->monster_count - 1);

        if (result) {// SOMETHING WAS FOUND

            printf(
                "%d %d found at rank %d\n",
                points[i]->x, points[i]->y, result
            );

            fprintf(
                out, "%d %d found at rank %d\n",
                points[i]->x, points[i]->y, result
            );

            continue;
        }

        printf(
            "%d %d not found\n",
            points[i]->x, points[i]->y
        );

        fprintf(
            out, "%d %d not found\n",
            points[i]->x, points[i]->y
        );
    }
}

void freeData(coord **points, fileData *store) {
    // FREES ALL REMAINING DYNAMICALLY ALLOCATED VARIABLES/STRUCTURES
    for (int i = 0; i < store->monster_count + store->points_count; i++) {
        free(points[i]);
    } free(points);

    free(store);
}

fileData *ReadInit(FILE *in) {
    // GATHERS THE FIRST 5 NUMBERS FROM THE TEXT FILE, STORES INSIDE OF "store"
    fileData *store = (fileData *) malloc(sizeof(fileData));

    fscanf(in, "%d %d", &store->x, &store->y);

    fscanf(
        in, "%d %d",
        &store->monster_count, &store->points_count
    );

    fscanf(in, "%d", &store->t);

    return store;
}

coord **ReadData(FILE *in, fileData *store) {
    // GATHERS TWO POINTS, STORES INSIDE OF "points" AT ELEMENT 'i'
    int size = store->monster_count + store->points_count;
    coord **points = (coord **) malloc(size * sizeof(coord *));

    for (int i = 0; i < size; i++) {
        points[i] = (coord *) malloc(sizeof(coord));
        fscanf(in, "%d %d", &(points[i]->x), &(points[i]->y));
    }

    return points;
}

void findDistance(coord **points, fileData *store) {
    // THIS FUNCTION FINDS THE DISTANCE FROM THE PROVIDED ORIGIN
    for (int i = 0; i < store->monster_count + store->points_count; i++)
        points[i]->distance = sqrt(
                pow((points[i]->x - store->x), 2) +
                pow((points[i]->y - store->y), 2)
        );
}

int compareTo(coord *ptrPt1, coord *ptrPt2) {
    // RETURNS 1 IF P1 > P2
    // RETURN -1 IF P1 < P2
    // RETURNS 0 IF P1 == P2
    if (ptrPt1->distance < ptrPt2->distance)
        return -1;
    if (ptrPt1->distance > ptrPt2->distance)
        return 1;

    if (ptrPt1->x < ptrPt2->x)
        return -1;
    if (ptrPt1->x > ptrPt2->x)
        return 1;

    if (ptrPt1->y < ptrPt2->y)
        return -1;
    if (ptrPt1->y > ptrPt2->y)
        return 1;

    // THE TWO POINTS ARE EQUIVALENT
    return 0;
}

void sort(fileData *store, coord **points) {
    //CHECKS TO SEE WHICH SORT ALGO SHOULD BE USED
    if (store->t >= store->monster_count)
        return insertionSort(points, store);

    mergeSort(points, 0, store->monster_count - 1);
}

void insertionSort(coord **points, fileData *store) {

    int i, j;

    for (i = 1; i < store->monster_count; i++) {
        coord *item = points[i];
        for (j = i - 1; j >= 0; j--) {
            coord *next_item = points[j];
            //if : NEXT_ITEM > ITEM
            if (compareTo(next_item, item) == 1) {
                points[j + 1] = next_item;
                continue;
            }
            break;
        }
        points[j + 1] = item;
    }
}

void mergeSort(coord **points, int left_idx, int right_idx) {
    if (left_idx >= right_idx) return;

    int mid_point = (left_idx + right_idx) / 2;

    mergeSort(points, left_idx, mid_point);
    mergeSort(points, mid_point + 1, right_idx);

    merge(points, left_idx, mid_point, right_idx);
}

void merge(coord **points, int left_idx, int midpoint_idx, int right_idx) {

    int i, j, k;

    int left_size = midpoint_idx - left_idx + 1;
    int right_size = right_idx - midpoint_idx;

    coord **Left_Array = (coord **) malloc(left_size * sizeof(coord *));
    coord **Right_Array = (coord **) malloc(right_size * sizeof(coord *));

    //TEMP LEFT SIDED ARRAY
    for (i = 0; i < left_size; i++)
        Left_Array[i] = points[left_idx + i];

    //TEMP RIGHT SIDED ARRAY
    for (j = 0; j < right_size; j++)
        Right_Array[j] = points[midpoint_idx + 1 + j];

    i = 0, j = 0, k = left_idx;

    while (i < left_size && j < right_size) {
        //CHECKING TO SEE IF LEFT < RIGHT
        if (compareTo(Left_Array[i], Right_Array[j]) != 1) {
            points[k++] = Left_Array[i++];
            continue;
        }
        //LEFT < RIGHT
        points[k++] = Right_Array[j++];
    }

    while (i < left_size)
        points[k++] = Left_Array[i++];

    while (j < right_size)
        points[k++] = Right_Array[j++];

    free(Left_Array);
    free(Right_Array);
}

int binSearch(coord *check, coord **points, int l, int r) {

    if (r >= l) {

        int mid = (l + r) / 2;

        // MID POINTS == CHECK
        if (!compareTo(points[mid], check))
            return mid + 1;

        // MID POINTS > CHECK
        if (compareTo(points[mid], check) == 1)
            return binSearch(check, points, l, mid - 1);

        // MID POINTS < CHECK
        if (compareTo(points[mid], check) == -1)
            return binSearch(check, points, mid + 1, r);

        return 1;
    }

    return 0;
}

