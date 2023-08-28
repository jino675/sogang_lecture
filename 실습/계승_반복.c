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
    int i, res = 1;
    if (n == 0) return 1;
    for (i = 1; i <= n;i++) {
        res *= i;
    }
    return res;
}