#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    char input[102];

    int k, i, j, count1, count2;
    int temp_p, temp_k, temp_r;
    int n_prime = 0;

    struct prime {
        int number;
        struct prime* l_link;
        struct prime* r_link;
    };

    count1 = 0;
    for (i = 0; i < 102; i++) {
        if ((input[i] = getchar()) == 32 || i == 101) {
            input[i] = 0;
            break;
        }
        count1++;
    }

    scanf("%d", &k);
    struct prime* head = malloc(sizeof(struct prime));
    struct prime* last = head;
    struct prime* temp;
    struct prime* temp_t;
    for (i = 2; i < k; i++) {
        temp = malloc(sizeof(struct prime));
        temp->number = i;
        temp->r_link = head;
        temp->l_link = last;
        last->r_link = temp;
        head->l_link = temp;
        last = temp;
    }
    for (temp = head->r_link; temp != head; temp = temp->r_link) {
        for (temp_t = temp->r_link; temp_t != head; ) {
            if ((temp_t->number % temp->number) == 0) {
                temp_t->l_link->r_link = temp_t->r_link;
                temp_t->r_link->l_link = temp_t->l_link;
                last = temp_t;
                temp_t = temp_t->r_link;
                free(last);
            }
            else temp_t = temp_t->r_link;
        }
    }

    for (temp = head->r_link; temp != head ; temp = temp->r_link) {
        count2 = 1;
        temp_k = temp->number;

        while (temp_k >= 10) {
            temp_k /= 10;
            count2++;
        }
        temp_k = temp->number;
        temp_p = 0;
        temp_r = 0;
        for (j = 0; j < count2; j++) {
            temp_p += (input[j] - 48) * (int)pow(10, count2 - j - 1);
        }
        for (j = count2; j < count1 + 1; j++) {
            temp_r *= 10;
            if (temp_p >= temp_k) {
                temp_r += temp_p / temp_k;
                temp_p %= temp_k;
            }
            if (j != count1) {
                temp_p *= 10;
                temp_p += input[j] - 48;
            }
        }
        if (!temp_p) {
            printf("BAD %d\n", (temp_k < temp_r) ? temp_k : temp_r);
            return 0;
        }
    }
    printf("GOOD\n");
    for (temp = head->r_link; temp != head; ) {
        temp->l_link->r_link = temp->r_link;
        temp->r_link->l_link = temp->l_link;
        last = temp;
        temp = temp->r_link;
        free(last);
    }
    free(head);

    return 0;
}