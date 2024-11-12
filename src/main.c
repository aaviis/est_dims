#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "main.h"
#include "functions_gp.h"
#include "functions_boxc.h"
#include <malloc.h>
#include <time.h>
#include "config.h"
#include <math.h>
#include <string.h>

double *data;

int *res_m;
double *res_ep;
double *res_C;

// читаем точки исследуемого множества
double *read_data(const char *filename, int *size)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("File cannot be opened.\n");
		exit(EXIT_FAILURE);
	}

	// Определяем размер данных в файле
	int count = 0;
	double value;
	while (fscanf(file, "%lf", &value) == 1)
	{
		count++;
	}

	// Возвращаем указатель в начало файла
	rewind(file);

	// Выделяем память под массив
	double *data = (double *)malloc(count * sizeof(double));
	if (data == NULL)
	{
		printf("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	// Считываем данные в массив
	int i = 0;
	while (fscanf(file, "%lf", &data[i]) == 1)
	{
		i++;
	}

	fclose(file);

	*size = count; // Возвращаем размер массива
	return data;   // Возвращаем указатель на массив
}

// читаем расчеты корреляционного интеграла
void read_data_for_mnk(const char *filename, int **res_m, double **res_ep, double **res_C, int *size)
{
	FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file.\n");
        exit(1);
    }
	int count = 0;
	int numInfinity = 0; // Counter for infinity values

	// Подсчет количества строк с бесконечными значениями
    char buffer[2560];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
		char *token = strtok(buffer, ";");        
        token = strtok(NULL, ";");       
		token = strtok(NULL, ";");        

        if (strcmp(token, "-inf") == 0) {
			numInfinity++;
           
        } else {
			count++;
		}
    }
	printf("Skipped %d rows with '-inf' values.\n", numInfinity);


// Выделение памяти для массивов результатов с учетом исключенных строк    
    *res_m = (int *)malloc(count * sizeof(int));
    *res_ep = (double *)malloc(count * sizeof(double));
    *res_C = (double *)malloc(count * sizeof(double));

    if (*res_m == NULL || *res_ep == NULL || *res_C == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

	// Перемещение указателя файла на начало
    fseek(file, 0, SEEK_SET);

    // Заполнение массивов результатов без строк с бесконечными значениями
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ";");
        int m = atoi(token);
        token = strtok(NULL, ";");
        double ep = strcmp(token, "-inf") == 0 ? -INFINITY : atof(token);
        token = strtok(NULL, ";");
        double C = strcmp(token, "-inf") == 0 ? -INFINITY : atof(token);

        if (!isinf(ep) && !isinf(C)) {
            (*res_m)[i] = m;
            (*res_ep)[i] = ep;
            (*res_C)[i] = C;
            i++;
        }
    }
	*size = count;
    fclose(file);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Please provide a valid argument: 'corrint', 'mnk', 'boxc'\n");
		return 1;
	}
	// Создание имени файла на основе аргумента пользователя и N
	char filename_result[50];
	snprintf(filename_result, sizeof(filename_result), "%scorrint_%s_%i.txt", DIR, FILE_NAMEPART, N);

	if (strcmp(argv[1], "corrint") == 0)
	{
		int size;
		double *data = read_data(FILE_DATA, &size);
		clock_t start = clock();
		printf("\\---------Estimates CorrInt---------\\\n");

		// рассчет корреляционного интеграла
		estandsave_corrint(data, filename_result);
		
		clock_t stop = clock();
		double elapsed = (double)(stop - start) / CLOCKS_PER_SEC;
		// ms: double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

		int hours = (int)(elapsed / 3600);						// Получаем количество часов
		int minutes = (int)((elapsed - hours * 3600) / 60);		// Получаем количество минут
		double seconds = elapsed - hours * 3600 - minutes * 60; // Получаем количество секунд
		printf("Time elapsed: %d hours, %d minutes, %.3f seconds\n", hours, minutes, seconds);
	}

	if (strcmp(argv[1], "boxc") == 0)
	{
		int size;
		double *data = read_data(FILE_DATA, &size);
		// for (int i = 40; i < 50; i++) {
		// 	printf("data[%i] = %0.30lf\n",i,data[i]);
		// }
		printf("\\---------Estimates Box-Counting---------\\\n");
		char filename_result[50];
		snprintf(filename_result, sizeof(filename_result), "%sboxc_%s_%i.txt", DIR, FILE_NAMEPART, N);
		estandsave_boxc(data, filename_result);


	}

	if (strcmp(argv[1], "mnk") == 0)
	{
		printf("\\---------Estimates a,b for fix m, p---------\\\n");
		int num_m = ceil((MAX_M - MIN_M) / SPEED_M) + 1;
		int num_ep = ceil((MAX_EPS - MIN_EPS) / SPEED_EPS);
		//int n = num_m * num_ep;

		int *res_m;
		double *res_ep, *res_C;
		int size;

		read_data_for_mnk(filename_result, &res_m, &res_ep, &res_C, &size);

		// printf("Data read from file:\n");
		// for (int i = 0; i < size; i++) {
		// 	printf("%d;%.10lf;%.30lf\n", res_m[i], res_ep[i], res_C[i]);
		// }
		
		char filename_result_mnk[50];
		snprintf(filename_result_mnk, sizeof(filename_result_mnk), "%smnk_%s_%i.txt", DIR, FILE_NAMEPART, N);

		mnk(num_m, num_ep, size, res_m, res_ep, res_C, filename_result_mnk);
	}

	return 0;
}

//>mingw32-make
//est_corrint