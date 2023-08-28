#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main() {
    int i, j, n, count;
    printf("enter n\n");
    scanf("%d", &n);
    int* x = (int*)malloc(sizeof(int) * n);
    int* y = (int*)malloc(sizeof(int) * n);
    srand(time(0));
    for (i = 0; i < n; i++) {
        y[i] = (int)(rand()%20);
    }
    for (i = 0; i < n-1; i++) {
        if (y[i] < 0) continue;
        count = 0;
        x[count] = i;
        count++;
        for (j = i+1; j < n; j++) {
            if (y[i] == y[j]) {
                x[count] = j;
                y[j] *= -1;
                count++;
            }
        }
        if (count > 1) {
            for (j = 0; j < count; j++) {
                printf("%d ", x[j]);
            }
            printf("\n\n-----------------%d----------------\n\n", y[i]);
        }
    }
}

