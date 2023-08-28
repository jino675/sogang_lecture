#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50

typedef enum { head, entry } tagfield;
typedef struct {
    int row;
    int col;
    int value;
}entryNode;
struct matrixNode {
    struct matrixNode* down;
    struct matrixNode* right;
    tagfield tag;
    union {
        struct matrixNode* next;
        entryNode entry;
    }u;
};
typedef struct matrixNode* matrixPointer;
matrixPointer mread(FILE* fp)
{
    matrixPointer node, last, temp;
    int numrows, numcols, numterms, currentrow, row, col, value, hdmax, i;
    fscanf(fp, "%d %d", &numrows, &numcols);
    node = malloc(sizeof(struct matrixNode));
    node->tag = entry;
    node->u.entry.row = numrows;
    node->u.entry.col = numcols;

    hdmax = (numrows > numcols) ? numrows : numcols;
    if (hdmax != 0) {
        matrixPointer hdnode = malloc(sizeof(struct matrixNode) * hdmax);
        for (i = 0; i < hdmax; i++) {
            hdnode[i].tag = head;
            hdnode[i].right = &hdnode[i];
            hdnode[i].u.next = &hdnode[i];
        }
        numterms = 0;
        currentrow = 0;
        last = &hdnode[currentrow];
        for (row = 0; row < numrows; row++) {
            for (col = 0; col < numcols; col++) {
                fscanf(fp, "%d", &value);
                //printf("%d ", value);
                if (value != 0) {
                    if (row > currentrow) {
                        last->right = &hdnode[currentrow];
                        last = &hdnode[row];
                        currentrow = row;
                    }
                    temp = malloc(sizeof(struct matrixNode));
                    temp->tag = entry;
                    temp->u.entry.row = row;
                    temp->u.entry.col = col;
                    temp->u.entry.value = value;
                    last->right = temp;
                    last = temp;
                    hdnode[col].u.next->down = temp;
                    hdnode[col].u.next = temp;
                    numterms++;
                }
            }
            //printf("\n");
        }
        node->u.entry.value = numterms;
        last->right = &hdnode[currentrow];

        for (i = 0; i < hdmax - 1; i++) {
            hdnode[i].u.next->down = &hdnode[i];
            hdnode[i].u.next = &hdnode[i + 1];
        }
        hdnode[i].u.next->down = &hdnode[i];
        hdnode[i].u.next = node;
        node->right = &hdnode[0];

    }
    else {
        node->u.entry.value = 0;
        node->right = node;
    }
    return node;
}
void mwrite(matrixPointer node) {
    if (node == NULL) {
        printf("matrix doesn't exist.\n");
        return;
    }
    matrixPointer last;
    int numrows = node->u.entry.row;
    int numcols = node->u.entry.col;
    int numterms = node->u.entry.value;
    matrixPointer hdnode_current = node->right;
    int currentrow = 0;
    printf("\n\
numRows = %d, numCols = %d\n\
The matrix by row, column, and value:\n\
\n", numrows, numcols);
    if (numterms != 0) {
        while (currentrow < numrows) {
            last = hdnode_current;
            if (last->right == last) {
                currentrow++;
                hdnode_current = hdnode_current->u.next;
            }
            else {
                while (last->right != hdnode_current) {
                    last = last->right;
                    printf("%5d%5d%5d\n", last->u.entry.row, last->u.entry.col, last->u.entry.value);
                }
                currentrow++;
                hdnode_current = hdnode_current->u.next;
            }
        }
    }
    else printf("zero matrix\n");
    return;
}
void merase(matrixPointer* node)
{
    if (node == NULL) {
        printf("matrix doesn't exist.\n");
        return;
    }
    matrixPointer last, temp;
    matrixPointer hdnode_current = (*node)->right;
    int numrows = (*node)->u.entry.row;
    int numcols = (*node)->u.entry.col;
    int numterms = (*node)->u.entry.value;
    int currentrow = 0;
    if (numterms != 0) {
        while (currentrow < numrows) {
            last = hdnode_current;
            if (last->right == last) {
                currentrow++;
                hdnode_current = hdnode_current->u.next;
            }
            else {
                last = hdnode_current->right;
                while (last != hdnode_current) {
                    temp = last->right;
                    free(last);
                    last = temp;
                }
                currentrow++;
                hdnode_current = hdnode_current->u.next;
            }
        }
        hdnode_current = (*node)->right;
        free(hdnode_current);
    }
    free(*node);
    *node = NULL;
    return;
}
void main()
{
    FILE* A = fopen("A.txt", "r+");
    FILE* B = fopen("B.txt", "r+");
    matrixPointer a, b;
    a = mread(A); 
    b = mread(B);
    mwrite(a);
    mwrite(b);
    merase(&a);
    merase(&b);
    mwrite(a);
    mwrite(b);

    fclose(A);
    fclose(B);

    return;
}
