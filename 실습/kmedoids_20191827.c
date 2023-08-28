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
float** data;               //2차원 데이터 배열
struct medoid* cluster;     //각 cluster의 대가리
struct medoid** last;       //각 cluster의 last node pointer
int change;            //수렴 여부 판별기
int iteration;

void read_from_file()
{
    //데이터를 읽어서 2차원 배열에 값을 담는다.
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
    //임시로 k개의 medoids가 될 데이터를 임의로 선택한다.
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
    //모든 데이터와 k개 각각의 medoids와의 거리를 계산하여 가장 가까운 cluster에 해당 데이터를 
    //할당한다.
    int i, j, w, min;
    double* sum = malloc(sizeof(double) * K);
    double temp_sum;
    for (i = 0; i < numdata; i++) {                                                     //전체 데이터에 대해 반복
        for (j = 0; j < K && cluster[j].link->index != i; j++);                         //해당 데이터가 medoids에 할당한 데이터인지 검사
        if (j == K) {                                                                   //할당된 적 없는 데이터일 경우,
            min = 0;
            printf("data[%d] : ", i);
            for (int k = 0; k < numfeature; k++) {
                printf("%f ", data[i][k]);
            }
            printf("\n");
            for (j = 0; j < K; j++) {                                                   //전체 medoids에 대해 반복
                temp_sum = 0;
                for (w = 0; w < numfeature; w++) {                                      //전체 feature에 대해 반복
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
    for (i = 0; i < K; i++) {                                                           //전체 medoids에 대해 반복
        sum = calloc(cluster[i].index, sizeof(double));                          //각 medoids 개수만큼 sum 배열 생성
        min = 0;
        j = 0;
        current = cluster[i].link;
        clear[i] = current;
        while (current != &cluster[i]) {                                                //각 medoids 내용물 개수만큼 반복
            for (temp = cluster[i].link; temp != &cluster[i]; temp = temp->link) {      //current를 제외한 medoids 내용물 개수만큼 반복
                if (temp == current) continue;

                temp_sum = 0;
                for (w = 0; w < numfeature; w++) {                                      //numfeature 만큼 반복
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
    if (change != 0) {                                                        //만약 변동이 발생한다면 모든 node 해제 후 세팅
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
    //clusters 출력
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