#include <stdio.h>
#include <stdlib.h>

int fac(int n);

int main() {
    int n;
    do {
        printf("enter n\n");
        scanf("%d", &n);
    } while (n < 1);
    printf("%d\n", fac(n));
    return 0;
}

int fac(int n) {
    if (n == 0) return 1;
    return n * fac(n - 1);
}