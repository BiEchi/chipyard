#include <stdio.h>
#define M 3
#define N 5
void main()
{
    FILE *fp;
    int a[M][N], b[N][M], c[M][M];
    int i, j, k;
    fp = fopen("D:\\test.txt", "r");
    if (fp == NULL)
        printf("文件打开失败！\n");
    else
    {
        //读入A,就像正常写入一样，把scanf换成fscanf
        printf("读取矩阵A:\n");
        for (i = 0; i < M; i++)
            for (j = 0; j < N; j++)
                fscanf(fp, "%d", &a[i][j]);
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < N; j++)
                printf("%3d ", a[i][j]);
            printf("\n");
        }

        //读入B,就像正常写入一样，把scanf换成fscanf
        printf("读取矩阵B:\n");
        for (i = 0; i < N; i++)
            for (j = 0; j < M; j++)
                fscanf(fp, "%d", &b[i][j]);
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < M; j++)
                printf("%3d ", b[i][j]);
            printf("\n");
        }
        fclose(fp);
        //二维矩阵的乘法
        printf("矩阵A与B乘积：\n");
        for (i = 0; i < M; i++)
            for (j = 0; j < M; j++)
            {
                c[i][j] = 0;
                for (k = 0; k < N; k++)
                    c[i][j] += a[i][j] * b[j][i];
            }
        //输出
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < M; j++)
                printf("%5d ", c[i][j]);
            printf("\n");
        }
    }
}