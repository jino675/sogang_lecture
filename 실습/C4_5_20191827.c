#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define free_m(x) do { if(x != NULL) free(x); x = NULL; } while(0)


enum LOR { left, right };
typedef enum LOR LOR;

struct branch {
    int index_feature;
    double branch_point;
};
typedef struct branch branch;

struct tree {
    struct tree* left_child;
    struct tree* right_child;

    int** data_left;
    int num_data_left;
    int** data_right;
    int num_data_right;

    branch* branch_current;
    int data_class;
    int tree_level;

    int* used_attribute;
};
typedef struct tree tree;

tree* head;
branch** temp_branch;
int** data_all;
int num_data;
int num_numeric;
int num_categoric;
int num_branch;
int index_class;
int tree_count;

void read_from_file_train()
{
    int i, j;
    head = (tree*)calloc(1, sizeof(tree));
    FILE* fp = fopen("train.txt", "r+");
    fscanf(fp, "%d %d %d", &num_data, &num_numeric, &num_categoric);
    printf("%d %d %d\n", num_data, num_numeric, num_categoric);
    index_class = num_numeric + num_categoric;
    data_all = (int**)malloc(sizeof(int*) * num_data);
    for (i = 0; i < num_data; i++) {
        data_all[i] = (int*)malloc(sizeof(int) * (num_numeric + num_categoric + 1));
        for (j = 0; j < (num_numeric + num_categoric + 1); j++) {
            fscanf(fp, "%d", &data_all[i][j]);
            printf("%d ", data_all[i][j]);
        }
        printf("\n");
    }
    temp_branch = (branch**)malloc(sizeof(branch*) * (num_data * num_numeric + num_categoric));
    head->used_attribute = (int*)calloc((num_numeric + num_categoric), sizeof(int));
    head->num_data_left = num_data;
    head->data_left = data_all;
    head->tree_level = 0;
    tree_count = 0;
    fclose(fp);

    return;
}

void sort(tree* node, LOR dir, int index)
{
    int i, j, min;
    int* temp;
    if (dir == left) {
        for (i = 0; i < node->num_data_left - 1; i++) {
            min = i;
            for (j = i + 1; j < node->num_data_left; j++) {
                if (node->data_left[j][index] < node->data_left[min][index]) {
                    min = j;
                }
            }
            if (min != i) {
                temp = node->data_left[min];
                node->data_left[min] = node->data_left[i];
                node->data_left[i] = temp;
            }
        }
    }
    else {
        for (i = 0; i < node->num_data_right - 1; i++) {
            min = i;
            for (j = i + 1; j < node->num_data_right; j++) {
                if (node->data_right[j][index] < node->data_right[min][index]) {
                    min = j;
                }
            }
            if (min != i) {
                temp = node->data_right[min];
                node->data_right[min] = node->data_right[i];
                node->data_right[i] = temp;
            }
        }
    }

    return;
}


void make_branch_available(tree* node, LOR dir)
{
    num_branch = 0;
    int i, j, pre_val, cur_val;
    //printf("branch_point_all\n\n");
    if (dir == left) {
        for (i = 0; i < num_numeric + num_categoric; i++) {
            if (node->used_attribute[i] == 1) continue;
            sort(node, dir, i);
            pre_val = node->data_left[0][i];
            for (j = 1; j < node->num_data_left; j++) {
                cur_val = node->data_left[j][i];
                if (pre_val != cur_val) {
                    temp_branch[num_branch] = calloc(1, sizeof(branch));
                    temp_branch[num_branch]->branch_point = ((double)pre_val + (double)cur_val) / 2;
                    temp_branch[num_branch]->index_feature = i;
                    //printf("branch_point[%d] : %lf\n", branch_all[num_branch]->index_feature, branch_all[num_branch]->branch_point);
                    num_branch++;
                    pre_val = cur_val;
                }
            }
        }
    }
    else {
        for (i = 0; i < num_numeric + num_categoric; i++) {
            if (node->used_attribute[i] == 1) continue;
            sort(node, dir, i);
            pre_val = node->data_right[0][i];
            for (j = 1; j < node->num_data_right; j++) {
                cur_val = node->data_right[j][i];
                if (pre_val != cur_val) {
                    temp_branch[num_branch] = calloc(1, sizeof(branch));
                    temp_branch[num_branch]->branch_point = ((double)pre_val + (double)cur_val) / 2;
                    temp_branch[num_branch]->index_feature = i;
                    //printf("branch_point[%d] : %lf\n", branch_all[num_branch]->index_feature, branch_all[num_branch]->branch_point);
                    num_branch++;
                    pre_val = cur_val;
                }
            }
        }
    }

    return;
}

double log_2(double x)
{
    if (x == 0) return 0;

    return log2(x);
}

void make_tree_child(tree* parent, LOR dir)
{
    int i, j;
    int num_true = 0;
    int num_false = 0;
    int num_true_up = 0;
    int num_false_up = 0;
    int num_true_down = 0;
    int num_false_down = 0;
    int num_up = 0;
    int num_down = 0;
    int num_up_max = 0;
    int num_down_max = 0;
    int current_index;
    int max_gain_index = 0;
    double max_gain = 0;
    int** temp_data_up;
    int** temp_data_down;
    double current_branch_point;
    double p_true, p_false, p_true_up, p_false_up, p_true_down, p_false_down, p_up, p_down;
    double pre_entropy, entropy_up, entropy_down, info_gain;
    tree* child = (tree*)calloc(1, sizeof(tree));
    if (dir == left) {
        for (i = 0; i < parent->num_data_left; i++) {
            if (parent->data_left[i][index_class] == 0) num_false++;
            if (parent->data_left[i][index_class] == 1) num_true++;
        }
        if (num_true == 0 || num_false == 0) {
            if (num_true == 0) child->data_class = 2;
            else if (num_false == 0) child->data_class = 1;
            child->tree_level = parent->tree_level + 1;
            parent->left_child = child;
            return;
        }
        make_branch_available(parent, dir);
        if (num_branch == 0) {
            child->data_class = (num_true > num_false) ? 1 : 2;
            parent->left_child = child;
            return;
        }
        p_true = (double)num_true / (double)parent->num_data_left;
        p_false = (double)num_false / (double)parent->num_data_left;
        pre_entropy = -(p_true * log_2(p_true) + p_false * log_2(p_false));
        for (i = 0; i < num_branch; i++) {
            num_true_up = 0;
            num_false_up = 0;
            num_true_down = 0;
            num_false_down = 0;
            num_up = 0;
            num_down = 0;
            current_index =temp_branch[i]->index_feature;
            current_branch_point = temp_branch[i]->branch_point;
            for (j = 0; j < parent->num_data_left; j++) {
                if (parent->data_left[j][current_index] > current_branch_point) {
                    if (parent->data_left[j][index_class] == 0) num_false_up++;
                    if (parent->data_left[j][index_class] == 1) num_true_up++;
                    num_up++;
                }
                if (parent->data_left[j][current_index] < current_branch_point) {
                    if (parent->data_left[j][index_class] == 0) num_false_down++;
                    if (parent->data_left[j][index_class] == 1) num_true_down++;
                    num_down++;
                }
            }
            if (num_up != 0) {
                p_true_up = (double)num_true_up / (double)num_up;
                p_false_up = (double)num_false_up / (double)num_up;
                p_up = (double)num_up / (double)parent->num_data_left;
                entropy_up = -p_up * (p_true_up * log_2(p_true_up) + p_false_up * log_2(p_false_up));
            }
            else entropy_up = 0;

            if (num_down != 0) {
                p_true_down = (double)num_true_down / (double)num_down;
                p_false_down = (double)num_false_down / (double)num_down;
                p_down = (double)num_down / (double)parent->num_data_left;
                entropy_down = -p_down * (p_true_down * log_2(p_true_down) + p_false_down * log_2(p_false_down));
            }
            else entropy_down = 0;

            info_gain = pre_entropy - (entropy_up + entropy_down);
            printf("branch[%d]_info_gain : %lf\n", i, info_gain);
            if (max_gain < info_gain) {
                max_gain = info_gain;
                max_gain_index = i;
                num_up_max = num_up;
                num_down_max = num_down;
            }
        }
        //printf("\nmax[%d]_info_gain : %lf\n", i, max_gain);
        for (i = 0; i < num_branch; i++) {
            if (i == max_gain_index) {
                continue;
            }
            free(temp_branch[i]);
        }
        temp_data_up = (int**)malloc(sizeof(int*) * num_up_max);
        for (i = 0, j = 0; i < parent->num_data_left; i++) {
            if (parent->data_left[i][temp_branch[max_gain_index]->index_feature] > temp_branch[max_gain_index]->branch_point) {
                temp_data_up[j] = parent->data_left[i];
                j++;
                continue;
            }
        }
        temp_data_down = (int**)malloc(sizeof(int*) * num_down_max);
        for (i = 0, j = 0; i < parent->num_data_left; i++) {
            if (parent->data_left[i][temp_branch[max_gain_index]->index_feature] < temp_branch[max_gain_index]->branch_point) {
                temp_data_down[j] = parent->data_left[i];
                j++;
                continue;
            }
        }
        child->data_left = temp_data_up;
        child->data_right = temp_data_down;
        child->num_data_left = num_up_max;
        child->num_data_right = num_down_max;
        child->branch_current = temp_branch[max_gain_index];
        child->tree_level = parent->tree_level + 1;
        child->used_attribute = (int*)calloc((num_numeric + num_categoric), sizeof(int));
        for (i = 0; i < num_numeric + num_categoric; i++) {
            if (i == child->branch_current->index_feature) {
                child->used_attribute[i] = 1;
                continue;
            }
            child->used_attribute[i] = parent->used_attribute[i];
        }
        parent->left_child = child;
    }
    else {
        for (i = 0; i < parent->num_data_right; i++) {
            if (parent->data_right[i][index_class] == 0) num_false++;
            if (parent->data_right[i][index_class] == 1) num_true++;
        }
        if (num_true == 0 || num_false == 0) {
            if (num_true == 0) child->data_class = 2;
            else if (num_false == 0) child->data_class = 1;
            child->tree_level = parent->tree_level + 1;
            parent->right_child = child;
            return;
        }
        make_branch_available(parent, dir);
        if (num_branch == 0) {
            child->data_class = (num_true > num_false) ? 1 : 2;
            parent->right_child = child;
            return;
        }
        p_true = (double)num_true / (double)parent->num_data_right;
        p_false = (double)num_false / (double)parent->num_data_right;
        pre_entropy = -(p_true * log_2(p_true) + p_false * log_2(p_false));
        for (i = 0; i < num_branch; i++) {
            num_true_up = 0;
            num_false_up = 0;
            num_true_down = 0;
            num_false_down = 0;
            num_up = 0;
            num_down = 0;
            current_index = temp_branch[i]->index_feature;
            current_branch_point = temp_branch[i]->branch_point;
            for (j = 0; j < parent->num_data_right; j++) {
                if (parent->data_right[j][current_index] > current_branch_point) {
                    if (parent->data_right[j][index_class] == 0) num_false_up++;
                    if (parent->data_right[j][index_class] == 1) num_true_up++;
                    num_up++;
                }
                if (parent->data_right[j][current_index] < current_branch_point) {
                    if (parent->data_right[j][index_class] == 0) num_false_down++;
                    if (parent->data_right[j][index_class] == 1) num_true_down++;
                    num_down++;
                }
            }
            if (num_up != 0) {
                p_true_up = (double)num_true_up / (double)num_up;
                p_false_up = (double)num_false_up / (double)num_up;
                p_up = (double)num_up / (double)parent->num_data_right;
                entropy_up = -p_up * (p_true_up * log_2(p_true_up) + p_false_up * log_2(p_false_up));
            }
            else entropy_up = 0;

            if (num_down != 0) {
                p_true_down = (double)num_true_down / (double)num_down;
                p_false_down = (double)num_false_down / (double)num_down;
                p_down = (double)num_down / (double)parent->num_data_right;
                entropy_down = -p_down * (p_true_down * log_2(p_true_down) + p_false_down * log_2(p_false_down));
            }
            else entropy_down = 0;

            info_gain = pre_entropy - (entropy_up + entropy_down);
            printf("branch[%d]_info_gain : %lf\n", i, info_gain);
            if (max_gain < info_gain) {
                max_gain = info_gain;
                max_gain_index = i;
                num_up_max = num_up;
                num_down_max = num_down;
            }
        }
        //printf("\nmax[%d]_info_gain : %lf\n", i, max_gain);
        for (i = 0; i < num_branch; i++) {
            if (i == max_gain_index) {
                continue;
            }
            free(temp_branch[i]);
        }
        temp_data_up = (int**)malloc(sizeof(int*) * num_up_max);
        for (i = 0, j = 0; i < parent->num_data_right; i++) {
            if (parent->data_right[i][temp_branch[max_gain_index]->index_feature] > temp_branch[max_gain_index]->branch_point) {
                temp_data_up[j] = parent->data_right[i];
                j++;
                continue;
            }
        }
        temp_data_down = (int**)malloc(sizeof(int*) * num_down_max);
        for (i = 0, j = 0; i < parent->num_data_right; i++) {
            if (parent->data_right[i][temp_branch[max_gain_index]->index_feature] < temp_branch[max_gain_index]->branch_point) {
                temp_data_down[j] = parent->data_right[i];
                j++;
                continue;
            }
        }
        child->data_left = temp_data_up;
        child->data_right = temp_data_down;
        child->num_data_left = num_up_max;
        child->num_data_right = num_down_max;
        child->branch_current = temp_branch[max_gain_index];
        child->tree_level = parent->tree_level + 1;        
        child->used_attribute = (int*)calloc((num_numeric + num_categoric), sizeof(int));
        for (i = 0; i < num_numeric + num_categoric; i++) {
            if (i == child->branch_current->index_feature) {
                child->used_attribute[i] = 1;
                continue;
            }
            child->used_attribute[i] = parent->used_attribute[i];
        }
        parent->right_child = child;
    }
    make_tree_child(child, left);
    make_tree_child(child, right);

    return;
}

void ready_for_classification(tree* node)
{
    int i;
    if (node == NULL || node->left_child == NULL) return;
    ready_for_classification(node->left_child);
    ready_for_classification(node->right_child);
    if (node == head) {
        for (i = 0; i < node->num_data_left; i++) {
            free_m(node->data_left[i]);
        }
    }
    else {
        free_m(node->data_right);
        node->num_data_right = 0;
    }
    free_m(node->used_attribute);
    free_m(node->data_left);
    node->num_data_left = 0;

    return;
}

void read_from_file_test()
{
    int i, j;
    FILE* fp = fopen("test.txt", "r+");
    fscanf(fp, "%d %d %d", &num_data, &num_numeric, &num_categoric);
    printf("%d %d %d\n", num_data, num_numeric, num_categoric);
    index_class = num_numeric + num_categoric;
    data_all = (int**)malloc(sizeof(int*) * num_data);
    for (i = 0; i < num_data; i++) {
        data_all[i] = (int*)malloc(sizeof(int) * (num_numeric + num_categoric + 1));
        for (j = 0; j < (num_numeric + num_categoric); j++) {
            fscanf(fp, "%d", &data_all[i][j]);
            printf("%d ", data_all[i][j]);
        }
        printf("\n");
    }
    head->num_data_left = num_data;
    head->data_left = data_all;
    fclose(fp);

    return;
}

void classificate(tree* parent, LOR dir)
{
    int i, j;
    int num_up = 0;
    int num_down = 0;
    double branch_point;
    int index_feature;
    int** temp_data_up = NULL;
    int** temp_data_down = NULL;
    tree* child;
    if (dir == left) {
        child = parent->left_child;
        if (parent->data_left == NULL) return;
        if (child->left_child == NULL || child->right_child == NULL) {
            for (i = 0; i < parent->num_data_left; i++) {
                parent->data_left[i][index_class] = child->data_class % 2;
            }
            return;
        }
        branch_point = child->branch_current->branch_point;
        index_feature = child->branch_current->index_feature;
        for (i = 0; i < parent->num_data_left; i++) {
            if (parent->data_left[i][index_feature] > branch_point) num_up++;
            if (parent->data_left[i][index_feature] <= branch_point) num_down++;
        }
        if (num_up != 0) {
            temp_data_up = (int**)malloc(sizeof(int*) * num_up);
            for (i = 0, j = 0; i < parent->num_data_left; i++) {
                if (parent->data_left[i][index_feature] > branch_point) {
                    temp_data_up[j] = parent->data_left[i];
                    j++;
                }
            }
        }
        else temp_data_up = NULL;
        if (num_down != 0) {
            temp_data_down = (int**)malloc(sizeof(int*) * num_down);
            for (i = 0, j = 0; i < parent->num_data_left; i++) {
                if (parent->data_left[i][index_feature] <= branch_point) {
                    temp_data_down[j] = parent->data_left[i];
                    j++;
                }
            }
        }
        else temp_data_down = NULL;

        child->data_left = temp_data_up;
        child->data_right = temp_data_down;
        child->num_data_left = num_up;
        child->num_data_right = num_down;
    }    
    else {
        child = parent->right_child;
        if (parent->data_right == NULL) return;
        if (child->left_child == NULL || child->right_child == NULL) {
            for (i = 0; i < parent->num_data_right; i++) {
                parent->data_right[i][index_class] = child->data_class % 2;
            }
            return;
        }
        branch_point = child->branch_current->branch_point;
        index_feature = child->branch_current->index_feature;
        for (i = 0; i < parent->num_data_right; i++) {
            if (parent->data_right[i][index_feature] > branch_point) num_up++;
            if (parent->data_right[i][index_feature] <= branch_point) num_down++;
        }
        if (num_up != 0) {
            temp_data_up = (int**)malloc(sizeof(int*) * num_up);
            for (i = 0, j = 0; i < parent->num_data_right; i++) {
                if (parent->data_right[i][index_feature] > branch_point) {
                    temp_data_up[j] = parent->data_right[i];
                    j++;
                }
            }
        }
        else temp_data_up = NULL;
        if (num_down != 0) {
            temp_data_down = (int**)malloc(sizeof(int*) * num_down);
            for (i = 0, j = 0; i < parent->num_data_right; i++) {
                if (parent->data_right[i][index_feature] <= branch_point) {
                    temp_data_down[j] = parent->data_right[i];
                    j++;
                }
            }
        }
        else temp_data_down = NULL;

        child->data_left = temp_data_up;
        child->data_right = temp_data_down;
        child->num_data_left = num_up;
        child->num_data_right = num_down;
    }
    classificate(child, left);
    classificate(child, right);

    return;
}

void store_to_file()
{
    int i, j;
    FILE* fp = fopen("results.txt", "w+");
    for (i = 0; i < num_data; i++) {
        for (j = 0; j < num_numeric + num_categoric + 1; j++) {
            fprintf(fp, "%d ", data_all[i][j]);
            printf("%d ", data_all[i][j]);
        }
        fprintf(fp, "\n");
        printf("\n");
    }
    fclose(fp);

    return;
}

void clear_memory(tree* node)
{
    int i;
    if (node == NULL) {
        return;
    }
    if (node->left_child == NULL) {
        free_m(node);
        return;
    }    
    clear_memory(node->left_child);
    clear_memory(node->right_child);
    if (node == head) {
        for (i = 0; i < node->num_data_left; i++) {
            free_m(node->data_left[i]);
        }
        free_m(temp_branch);
    }
    else {
        free_m(node->data_right);
        free_m(node->branch_current);
    }
    free_m(node->data_left);
    free_m(node);

    return;
}

void print_tree(tree* node)
{
    if (node == NULL) return;
    print_tree(node->left_child);
    print_tree(node->right_child);

    printf("\n\
tree level : %d\n\
num_data_left : %d\n\
num_data_right : %d\n\
data_class : %d\n", node->tree_level, node->num_data_left, node->num_data_right,
node->data_class);
    if (node->branch_current != NULL) {
        printf("branch_index_feature : %d\n\
branch_point : %lf\n\n", node->branch_current->index_feature, node->branch_current->branch_point);
    }
    else {
        printf("branch_index_feature : X\n\
branch_point : X\n\n");
    }
    printf("data_left : ");
    if (node->left_child != NULL && node->data_left != NULL && node->left_child->data_class != 0) {
        for (int i = 0; i < node->num_data_left; i++) {
            printf("%d ", node->data_left[i][0]);
        }
        printf("\n===>>> data_class : %d", node->left_child->data_class % 2);
    }
    printf("\n\ndata_right : ");
    if (node->right_child != NULL && node->data_right != NULL && node->right_child->data_class != 0) {
        for (int i = 0; i < node->num_data_right; i++) {
            printf("%d ", node->data_right[i][0]);
        }
        printf("\n===>>> data_class : %d", node->right_child->data_class % 2);
    }
    printf("\n\n");
    tree_count++;

    return;
}

void main()
{
    printf("========================train data set=============================\n");
    read_from_file_train();
    make_tree_child(head, left);
    printf("\n\n\n\n\n\n\n\n\n\n\n");

    printf("========================test data set=============================\n");
    ready_for_classification(head);
    read_from_file_test();
    classificate(head, left);
    printf("\n\n\n\n\n\n\n\n\n\n\n");

    printf("========================result data set=============================\n");
    store_to_file();
    printf("\n\n\n\n\n\n\n\n\n\n\n");
    print_tree(head);
    clear_memory(head);
    printf("tree_count : %d\n\n", tree_count);

    return;
}