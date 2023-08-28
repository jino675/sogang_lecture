#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 100

void read_from_file(char* string, char* pat);
void fail(char* string, char* pat, char* failure);
int pmatch(char* string, char* pat, char* failure);

void main() {
    char string[MAX_SIZE], pat[MAX_SIZE], failure[MAX_SIZE];

    read_from_file(string, pat);
    fail(string, pat, failure);
    int pos = pmatch(string, pat, failure);
    if (pos >= 0) printf("starting position of pat in string is %d\n\n", pos);
    else printf("%d (no match)\n\n",pos);
    return;
}

void read_from_file(char* string, char* pat) {
    FILE* fp = fopen("kmp.txt", "r+");
    if (fp == NULL) {
        fprintf(stderr, "\"%s\" doesn't exsist\n","kmp.txt");
        exit(1);
    }
    fscanf(fp, "%s\n%s", string, pat);
    printf("%s\n%s\n\n", string, pat);
    fclose(fp);
}

void fail(char* string, char* pat, char* failure) {
    int n = strlen(pat);
    int i = 0, j;
    failure[0] = 0;
    for (j = 1; j < n;j++) {
        while ((pat[j] != pat[i]) && (i > 0)) {
            i = failure[i - 1];
        }
        if (pat[j] == pat[i]) {
            failure[j] = i + 1;
            i++;
        }
        else {
            failure[j] = 0;
        }
    }
}

int pmatch(char* string, char* pat, char* failure) {
    int n = strlen(string) - strlen(pat);
    int s = 0;
    int i = 0;
    while (s <= n) {
        while (string[s + i] == pat[i]) {
            i++;
        }
        if (i >= strlen(pat)) {
            return s;
        }
        if ((string[s + i] != pat[i]) && (i > 0)) {
            s = s + i - failure[i - 1];
            i = failure[i - 1];
        }
        else if ((string[s + i] != pat[i]) && (i == 0)) {
            s++;
        }
    }
    return -1;
}