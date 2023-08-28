#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct medoid {
    float* array;
    int index;
    struct medoid* link;
};

int numdata, numfeature, K;
float** data;               //2���� ������ �迭
struct medoid* cluster;     //�� cluster�� �밡��
struct medoid** last;       //�� cluster�� last node pointer
int change;            //���� ���� �Ǻ���
int iteration;

void read_from_file()
{
    //�����͸� �о 2���� �迭�� ���� ��´�.
    FILE* fp = fopen("input.txt", "r+");
    fscanf(fp, "%d %d %d", &numdata, &numfeature, &K);
    data = malloc(sizeof(float*) * numdata);
    for (int i = 0; i < numdata; i++) {
        data[i] = malloc(sizeof(float) * numfeature);
        for (int j = 0; j < numfeature; j++) {
            fscanf(fp, "%f", &data[i][j]);
            //printf("%f ", data[i][j]);
        }
        //printf("\n");
    }
    //printf("\n");
    fclose(fp);
    cluster = malloc(sizeof(struct medoid) * K);
    last = malloc(sizeof(struct medoid*) * K);
    return;
}

void select_random_medoids()
{
    //�ӽ÷� k���� medoids�� �� �����͸� ���Ƿ� �����Ѵ�.
    int i, j, temp_index;
    struct medoid* temp;
    //int index[3] = {0,100,22 };
    i = 0;
    while (i < K) {
        temp_index = rand() % numdata;
        for (j = 0; j < i && cluster[j].link->index != temp_index; j++);
        if (j == i) {
            temp = malloc(sizeof(struct medoid));
            //temp->array = data[index[i]];
            //temp->index = index[i];
            temp->array = data[temp_index];
            temp->index = temp_index;
            temp->link = &cluster[i];
            cluster[i].index = 1;
            cluster[i].array = NULL;
            cluster[i].link = temp;
            last[i] = cluster[i].link;
            i++;
        }
    }

    printf("\nInitial medoids\n\n");
    printf("iteration : 0\n\n");
    for (int i = 0; i < K; i++) {
        printf("medoid[%d] : ", i);
        for (int j = 0; j < numfeature; j++) {
            printf("%f ", (cluster[i].link->array)[j]);
        }
        printf("%d\n\n", cluster[i].link->index);
    }
    return;
}

void assign_data_to_cluster()
{
    //��� �����Ϳ� k�� ������ medoids���� �Ÿ��� ����Ͽ� ���� ����� cluster�� �ش� �����͸� 
    //�Ҵ��Ѵ�.
    int i, j, w, min;
    double* sum = malloc(sizeof(double) * K);
    double temp_sum;
    for (i = 0; i < numdata; i++) {                                                     //��ü �����Ϳ� ���� �ݺ�
        for (j = 0; j < K && cluster[j].link->index != i; j++);                         //�ش� �����Ͱ� medoids�� �Ҵ��� ���������� �˻�
        if (j == K) {                                                                   //�Ҵ�� �� ���� �������� ���,
            min = 0;
            printf("data[%d] : ", i);
            for (int k = 0; k < numfeature; k++) {
                printf("%f ", data[i][k]);
            }
            printf("\n");
            for (j = 0; j < K; j++) {                                                   //��ü medoids�� ���� �ݺ�
                temp_sum = 0;
                for (w = 0; w < numfeature; w++) {                                      //��ü feature�� ���� �ݺ�
                    temp_sum += pow((data[i][w] - (cluster[j].link->array)[w]), 2);
                }
                sum[j] = sqrt(temp_sum);
                printf("medoids[%d] <- %f  ",j, sum[j]);
                if (sum[j] < sum[min]) {
                    min = j;
                }
            }
            printf("        data[%i] -> medoids[%d]\n\n",i,min);
            struct medoid* temp = malloc(sizeof(struct medoid));
            temp->array = data[i];
            temp->index = i;
            temp->link = &cluster[min];
            last[min]->link = temp;
            last[min] = temp;
            cluster[min].index++;
        }
    }
    free(sum);
    return;
}

void find_new_medoids()
{
    int i, j, w, min;
    double* sum;
    double temp_sum;
    struct medoid* current;
    struct medoid* temp;
    struct medoid** clear = malloc(sizeof(struct medoid*) * K);
    change = 0;
    for (i = 0; i < K; i++) {                                                           //��ü medoids�� ���� �ݺ�
        sum = calloc(cluster[i].index, sizeof(double));                          //�� medoids ������ŭ sum �迭 ����
        min = 0;
        j = 0;
        current = cluster[i].link;
        clear[i] = current;
        while (current != &cluster[i]) {                                                //�� medoids ���빰 ������ŭ �ݺ�
            for (temp = cluster[i].link; temp != &cluster[i]; temp = temp->link) {      //current�� ������ medoids ���빰 ������ŭ �ݺ�
                if (temp == current) continue;

                temp_sum = 0;
                for (w = 0; w < numfeature; w++) {                                      //numfeature ��ŭ �ݺ�
                    temp_sum += pow((current->array)[w] - (temp->array)[w], 2);
                }
                sum[j] += sqrt(temp_sum);
            }
            //printf("\n\n%d : %f  \n\n", j, sum[j]);
            if (sum[j] < sum[min]) {
                min = j;
                clear[i] = current;
            }
            current = current->link;
            j++;
        }
        if (min != 0) change++;
        free(sum);
    }
    if (change != 0) {                                                        //���� ������ �߻��Ѵٸ� ��� node ���� �� ����
        for (i = 0; i < K; i++) {
            for (current = cluster[i].link; current != &cluster[i]; ) {
                if (current == clear[i]) {
                    current = current->link;
                    continue;
                }
                temp = current->link;
                free(current);
                current = temp;
            }
            clear[i]->link = &cluster[i];
            cluster[i].index = 1;
            cluster[i].link = clear[i];
            last[i] = cluster[i].link;
        }
        printf("\niteration : %d , change : %d\n\n", iteration, change);
        for (int i = 0; i < K; i++) {
            printf("medoids[%d] : ", i);
            for (int j = 0; j < numfeature; j++) {
                printf("%f ", (cluster[i].link->array)[j]);
            }
            printf("%d\n\n", cluster[i].link->index);
        }
    }

    free(clear);
    return;
}

void print_clusters()
{
    //clusters ���
    int i, j;
    struct medoid* last;

    printf("\n\n\n\n\nfinal medoids\n\niteration : %d\n\n", iteration-1);
    for (int i = 0; i < K; i++) {
        printf("medoids[%d] : ", i);
        for (int j = 0; j < numfeature; j++) {
            printf("%f ", (cluster[i].link->array)[j]);
        }
        printf("%d\n\n", cluster[i].link->index);
    }

    for (i = 0; i < K; i++) {
        printf("\ncluster[%d]    numterms = %d\n\n", i, cluster[i].index);
        for (last = cluster[i].link ; last != &cluster[i] ; last = last->link) {
            for (j = 0; j < numfeature; j++) {
                printf("%f ", (last->array)[j]);
            }
            printf("    index = %d\n", last->index);
        }
        printf("\n");
    }
    return;
}

void write_to_file()
{
    FILE* fp = fopen("output.txt", "w+");
    struct medoid* temp;
    int i,j;
    for (i = 0; i < K; i++) {
        fprintf(fp, "%d\n", i);
        for (temp = cluster[i].link; temp != &cluster[i]; temp = temp->link) {
            for (j = 0; j < numfeature; j++) {
                fprintf(fp, "%f ", temp->array[j]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return;
}

void free_all()
{
    struct medoid* temp;
    struct medoid* current;
    int i;
    for (i = 0; i < K; i++) {
        for (current = cluster[i].link; current != &cluster[i]; ) {
            temp = current->link;
            free(current);
            current = temp;
        }
    }
    free(cluster);
    free(last);

    for (i = 0; i < numdata; i++) {
        free(data[i]);
    }
    free(data);
    return;
}

void main()
{
    srand(1000);
    read_from_file();
    select_random_medoids();
    iteration = 1;
    do {
        assign_data_to_cluster();
        find_new_medoids();
        iteration++;
    } while (change);
    print_clusters();
    write_to_file();
    free_all();
    
    return;
}