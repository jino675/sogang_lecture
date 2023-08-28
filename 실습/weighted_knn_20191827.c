#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define DATA_TYPE 1
#define TEST_TYPE 0
#define K 5
#define SWAP_S(x,y,temp) do{\
temp = x;\
x = y;\
y = temp;\
}while(0)

typedef struct {
    float distance;
    int type;
}dis;

void read_from_file(int* nd, int* nf, float*** data, int index);
void identify_class(int nd_d, int nd_t, int nf, float** data, float** test, int** output);
void ss_selection_sort(dis** arr, int nd_d, int nt);
void s_selection_sort(dis* arr);
void creat_output_file(int nd_t, int* output);

void main() {
    int nd_d, nd_t, nf, *output;
    float** data, **test;
    read_from_file(&nd_d, &nf, &data, DATA_TYPE);
    for(int i = 0; i<50; i++) printf("\n");
    read_from_file(&nd_t, &nf, &test, TEST_TYPE);
    for (int i = 0; i < 50; i++) printf("\n");
    identify_class(nd_d, nd_t, nf, data, test, &output);
    creat_output_file(nd_t, output);
    free(data[0]);
    free(data);
    free(test[0]);
    free(test);
    free(output);
}

void read_from_file(int* nd, int* nf, float*** data, int type) {
    int i, j, temp;
    FILE* fp;
    if (type) {
        fp = fopen("data.txt", "r+");
        fscanf(fp, "%d %d", nd, nf);
    }
    else {
        fp = fopen("test.txt", "r+");
        fscanf(fp, "%d", nd);
    }
    *data = malloc(sizeof(float*) * (*nd));
    (*data)[0] = malloc(sizeof(float) * (*nd) * (*nf + type));
    for (i = 1; i < (*nd); i++) {
        (*data)[i] = (*data)[0] + (i * (*nf + type));
    }
    for (i = 0; i < (*nd); i++) {
        for (j = 0; j < (*nf); j++) {
            fscanf(fp, "%f", &(*data)[i][j]);
        }
        if (type) {
            fscanf(fp, "%d", &temp);
            (*data)[i][j] = (float)temp;
        }
    }
    fclose(fp);
    for (i = 0; i < (*nd); i++) {
        for (j = 0; j < (*nf); j++) {
            printf("%.1f ", (*data)[i][j]);
        }
        if(type) printf("%.1f ", (*data)[i][j]);
        printf("\n");
    }
}

void identify_class(int nd_d, int nd_t, int nf, float** data, float** test, int** output) {
    int i, j, k, on;
    float sum;

    *output = malloc(sizeof(int) * nd_t);

    dis** L2 = malloc(sizeof(dis*) * nd_t);
    for (i = 0; i < nd_t; i++) {
        L2[i] = malloc(sizeof(dis) * nd_d);
    }
    dis* temp = malloc(sizeof(dis) * K);
    for (i = 0; i < nd_t; i++) {                                       //테스트 항목별 데이터 항목 간 거리 계산하기
        for (j = 0; j < nd_d; j++) {
            sum = 0;
            for (k = 0; k < nf; k++) {
                sum += (float)pow((data[j][k] - test[i][k]), 2);
            }
            L2[i][j].distance = sqrt(sum);
            L2[i][j].type = (int)data[j][k];
        }
    }
    for (i = 0;i < nd_t;i++) {                                          //가중치 계산하기
        ss_selection_sort(L2, nd_d, i);                                 //테스트 항목별 데이터 항목 간 거리 순위 정하기
        memset(temp, -1, sizeof(dis) * K);                              
        for (j = 0; j < 5; j++) {                                       //상위 5개를 클래스 별로 나눠서 거리의 인버스를 더하기
            on = 0;
            for (k = 0; k < j;k++) {
                if (L2[i][j].type == temp[k].type) {
                    temp[k].distance += 1 / (L2[i][j].distance);
                    on = 1;
                }
            }
            if (!on) {
                temp[j].distance = 1 / (L2[i][j].distance);
                temp[j].type = L2[i][j].type;
            }
        }
        s_selection_sort(temp);                                       //가장 가중치가 높은 클래스를 구한 뒤 결과값으로 저장
        (*output)[i] = temp[0].type;
    }

    for (i = 0; i < nd_t; i++) {
        free(L2[i]);
    }
    free(L2);
    free(temp);
}

void ss_selection_sort(dis** arr, int nd_d, int nt) {
    int j, k, mi;
    dis temp;
    for (j = 0;j < nd_d - 1;j++) {
        mi = j;
        for (k = j+1;k < nd_d;k++) {
            if (arr[nt][k].distance < arr[nt][mi].distance) {
                mi = k;
            }
        }
        SWAP_S(arr[nt][j], arr[nt][mi], temp);
    }
}

void s_selection_sort(dis* arr) {
    int j, k, max;
    dis temp;
    for (j = 0;j < K - 1;j++) {
        max = j;
        for (k = j+1;k < K;k++) {
            if (arr[k].distance > arr[max].distance) {
                max = k;
            }
        }
        SWAP_S(arr[j], arr[max], temp);
    }
}

void creat_output_file(int nd_t, int* output) {
    int i;
    FILE* fp = fopen("output.txt", "w+");
    for (i = 0; i < nd_t;i++) {
        fprintf(fp, "%d\n", output[i]);
    }
    for (i = 0; i < nd_t;i++) {
        printf("%d\n", output[i]);
    }
    fclose(fp);
}