#include <stdio.h>
#include <stdlib.h>

long long f_seq(int i);

int main() {
	int n, i;
	do {
		printf("enter n\n");
		scanf(" %d", &n);
	} while (n < 1);

	long long* seq = (long long*)malloc(sizeof(long long) * (n + 1));
	
	for (i = 0; i <= n;i++) {
		seq[i] = f_seq(i);
	}
	for (i = 0; i <= n; i++) {
		printf("%lld ", seq[i]);
	}
	printf("\n");
	free(seq);
	return;
}
long long f_seq(int i) {
	if (i == 0) return 0;
	else if (i == 1) return 1;
	else return f_seq(i - 1) + f_seq(i - 2);
}