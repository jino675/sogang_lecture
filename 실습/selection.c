#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define MAX_SIZE 1001
#define SWAP(x, y, type) do {\
type temp = x;\
x = y;\
y = temp;\
}while(0)

void data_generator(int n, FILE * fp);
void data_reader(int** list, int* n, FILE * fp);
void selection_sort(int* list, int n);

void main() {
    char ans;
    int n, re;
    int* list = NULL;
    FILE* fp = NULL;
    double duration;
    time_t start = 0;
    int step = 10;
    do {
        printf("Do you have a data file? (Y/N)\n");
        scanf(" %c", &ans);
    } while (!(ans == 'N' || ans == 'n' || ans == 'Y' || ans == 'y'));
    printf("\nn     repetitions    time(¥ìs)\n");
    for (n = 10;n < MAX_SIZE; n += step) {
        re = 0;
        if (ans == 'N'|| ans == 'n') {
            data_generator(n, fp);
        }
        data_reader(&list, &n, fp);
        time(&start);
        do {
            selection_sort(list, n);
            re++;
        } while (difftime(time(NULL), start) < 1.);
        duration = 1e6*difftime(time(NULL), start) / re;
        printf("%d     %d        %.2lf\n", n, re, duration);
        free(list);
        if (ans == 'Y'|| ans == 'y') {
            break;
        }
        if (n == 100) step = 100;
    }
}

void data_generator(int n, FILE* fp) {
    fp = fopen("lab1.data", "w+");
    fprintf(fp, "%d ", n);
    for (int i = 0;i < n; i++) {
        fprintf(fp, "%d ", n-i);
    }
    fclose(fp);
}

void data_reader(int** list, int* n, FILE* fp) {
    fp = fopen("lab1.data", "r+");
    fscanf(fp, "%d ", n);
    *list = (int*)malloc(sizeof(int) * (*n));
    for (int i = 0;i < (*n); i++) {
        fscanf(fp, "%d ", &(*list)[i]);
    }
    fclose(fp);
}

void selection_sort(int* list, int n) {
    int i, j, min;

    for (i = 0;i < n - 1;i++) {
        min = i;
        for (j = i + 1;j < n;j++) {
            if (list[min] > list[j]) {
                min = j;
            }
        }
        SWAP(list[i], list[min], int);
    }
}

