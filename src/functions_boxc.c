#include <stdlib.h>
#include <stdio.h>
// #include < string.h >
#include <math.h>
#include <time.h>
#include "config.h"
#include <complex.h>

int save_d(const char *file_result, int m, long int num, double points, double eps, double cubs, double D)
{
	// сохраняем рассчитанные данные m, pow(num,m), points,eps,cubs,D

	FILE *fout = fopen(file_result, "a");
	// printf("Save to file %s , m=%i\n", file_result,m);
	if (fout == 0)
	{
		fprintf(stderr, "Error open file %s\n", file_result);
		return 1;
	}

	fprintf(fout, "%d;%f;%f;%f;%f;%f\n", m, pow(num, 1), points, eps, cubs, D);
	fclose(fout);
	return 1;
}

// расчет фрактальной размерности по методу Boxing-Counting
// наше множество помещаем в куб m-размерности, затем разбиваем его на единичные кубы,
// вытягиваем единичные кубы в линию и считаем номер куба, в который попадает наша точка множества
// создаем массив единичных кубов с отметками, сколько в нем попало множеств и сколько всего вышло кубов
void estandsave_boxc(double *data, const char *file_result)
{
	// m - размерность
	// pow(num,m) - общее количество кубиков
	// points - всего количество точек, проверочный столбец, его величина должна быть
	// равна количеству точек за минусом размерности вектора
	// eps - размер грани минимального куба, которым покрываем объект
	// cubs - количество кубиков, в которые попали наши элементы множества
	// D - рассчитанная фрактальная размерность

	int K = 25000;
	double d = 1.0;//размер куба, в котором вмещается наш объект
	double epss[3] = {0.01, 0.001, 0.0005};

	clock_t start = clock();

	for (int h = 0; h < 3; h++)
	{
		double eps = epss[h]; // 0.001; 0.0005;
		printf("\\---------epsilon = %lf---------\\\n", eps);

		for (int m = 1; m < 5; m++)
		{
			printf("\\---------m = %i---------\\\n", m);

			double D = 0;
			long int num_edge = floor(d / eps);			 // количество кубиков в ребре
			long int all_cubs = pow(d/eps, m); // всего количество кубиков
			long int num_x = 0;
			long int points = 0;
			long int cubs = 0;
			printf("all_cubs = %ld\n", all_cubs);

			// задаем массив всех кубиков в линейку
			//столько элементов в 1 размерности массива
			long int row_size = 10;
			//столько 2 размерности массива
			long int row_counts = floor(all_cubs/row_size);
    		long int element_size = sizeof(double);
			long int total_memory = row_counts * element_size;

			printf("row_size=%i, row_counts=%i, total_memory=%i\n", row_size, row_counts, total_memory);

			//Проверяем доступный объем памяти перед выделением
			if (total_memory / row_counts != element_size || total_memory <= 0) {
				printf("Error: Memory size is not valid\n");
			} else {
				printf("Memory size is valid\n");
			}

			double **x1s = (double **)malloc(row_counts * sizeof(double *));			
			// Проверяем, была ли память успешно выделена
			//для указателей на строки двумерного массива
			if (x1s == NULL) {
				printf("Error: Failed to allocate memory for x1s\n");
			}
			else {
				//printf("Memory has allocated successfully\n");
			}

			for (int i = 0; i < row_counts; i++) {
				x1s[i] = (double *)malloc(row_size * sizeof(double));
				if (x1s[i] == NULL) {
					printf("Error: Failed to allocate memory for rows of x1s\n");
					// Освобождаем память, выделенную ранее
					for (int j = 0; j < i; j++) {
						free(x1s[j]);
					}
					free(x1s);
				}
				else {
					//printf("Memory for rows has allocated successfully\n");
				}
			}

			// перебираем все элементы множества
			for (int i = m-1; i < K; i++)
			{
				if (m == 1)
				{
					// строим вектор нужной нам размерности
					double x = data[i];
					//printf("i = %i, x = %0.30lf\n",i,data[i]);
					// отмечаем, куда попадает вектор, в какой кубик - номер кубика, в который попадает вектор
					num_x = floor(x / eps);
					//printf("num_x = %ld, x = %.30lf, i=%i\n", num_x, x, i);
				}
				if (m == 2)
				{
					double x = data[i-1];
					double y = data[i];
					num_x = floor(x/eps) + num_edge*floor(y/eps); //номер ячейки
					//printf("num_x = %ld, x = %.30lf, i=%i\n", num_x, x, i);
				}
				if (m == 3)
				{
					double x = data[i-2];
					double y = data[i-1];
					double z = data[i];
					num_x = floor(x/eps) + num_edge*floor(y/eps) + pow(num_edge,2)*floor(z/eps); //номер ячейки
					// printf("i = %i, all_cubs=%ld, num_x=%ld, x = %lf,y = %lf,z = %lf\n",i,all_cubs,num_x,x,y,z);
					// printf("x1s[%ld] = %lf", x1s[1]);
				}
				if (m == 4)
				{
					double x = data[i-3];
					double y = data[i-2];
					double z = data[i-1];
					double w = data[i];
					num_x = floor(x/eps) + num_edge*floor(y/eps) + pow(num_edge,2)*floor(z/eps) + pow(num_edge,3)*floor(w/eps); 
					// printf("num_x = %ld, x = %.30lf, i=%i\n", num_x, x, i);
				}

				long int q = num_x - floor(num_x/row_size)*row_size;
				//printf("num_x= %ld, q = %ld, x1s=%lf\n", num_x, q, x1s[m-1][q]);

				
				// отмечаем, если кубик еще не содержит вектора, тогда +1 кубик в нашей общей сумме кубиков
				if (x1s[m-1][q] < 1)
				{
					cubs += 1;
				}
				
				// записываем, что в этом кубике +1 вектор
				x1s[m-1][q] += 1;

				// а тут просто проверяем сумму всех переборов, их должно быть
				// по размеру множества минус размерность
				// printf("x1s[%ld] = %lf, points = %ld\n", num_x, x1s[num_x], points);
				points += 1;
			}
			// считаем фрактальную размерность
			// всего кубиков с векторами на единичный размер ребра кубика
			//printf("cubs:%ld, ep:%lf\n", cubs, eps);

			D = -log(cubs) / log(eps);
			printf("m:%i, all_cubs:%ld, points:%ld, num:%ld, D:%lf\n", m, all_cubs, points, cubs, D);
			save_d(file_result, m, all_cubs, points, eps, cubs, D);
			printf("%s\n", file_result);
			for (int i = 0; i < row_counts; i++) {
				for (int j = 0; j < i; j++) {
			 			free(x1s[j]);
			 		}
			 		free(x1s);
			}		
		}
	}

	

	clock_t stop = clock();
	double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
	printf("Time elapsed all in ms: %f \n", elapsed);
}


// расчет фрактальной размерности по методу Boxing-Counting
//прямой пребор - попадает ли точка множества в конкретный единичный кубик или нет только для размерности m = 2
void estandsave_boxc2(double *data, const char *file_result)
{
	// m - размерность
	// pow(num,m) - общее количество кубиков
	// points - всего количество точек, проверочный столбец, его величина должна быть
	// равна количеству точек за минусом размерности вектора
	// eps - размер грани минимального куба, которым покрываем объект
	// cubs - количество кубиков, в которые попали наши элементы множества
	// D - рассчитанная фрактальная размерность

	double *eps = (double *)malloc(sizeof(double) * 5);
	eps[0] = 0.01;
	eps[1] = 0.005;
	eps[2] = 0.001;
	eps[3] = 0.0005;
	eps[4] = 0.0001;

	double min_x = 0.0; // 1.0
	double max_x = 1.0; // 10.0
	double min_y = 0.0; // 1.0
	double max_y = 1.0; // 10.0

	int m = 2;

	int i, j;
	double count_cubs = 0;
	double all_cubs = 0;
	double d = 1;
	
	double temp_x, temp_y;
	int K = 25000;
	int lag = 1;

	double step;
	for (j = 0; j < 3; j++)
	{
		temp_x = min_x;
		temp_y = min_y;

		step = eps[j];
		double all_cubs_est = pow(d/step,m);
		//printf("ep = %f, all_cubs_est=%lf\n", step,all_cubs_est);

		while (temp_x < max_x)
		{
			temp_y = 0;
			while (temp_y < max_y)
			{
				all_cubs += 1;
				// printf("x_%f \t y_%f\n",temp_x,temp_y);

				for (i = lag*(m-1); i < K; i++)
				{
					double x_i = data[i];
					double y_i = data[i-lag];
					all_cubs +=1;

					// printf("%lf;%lf", &data[i][0], &data[i][1]);
					if ((x_i >= temp_x - step) && (x_i < temp_x))
						if ((y_i >= temp_y - step) && (y_i < temp_y))
						{
							// printf("x(%lf,%lf) лежит в x[%f,%f] и y[%f,%f]\n",x_i,y_i,temp_x-step,temp_x,temp_y-step,temp_y);
							count_cubs += 1;
						}
				}				
				temp_y += step;
			}
			temp_x += step;
		}

		double d = -log(N) / log(step);
		printf("ep=%lf, all_cubs=%lf,all_cubs_est=%lf, count_cubs =%lf, D = %lf\n", step, all_cubs,all_cubs_est, count_cubs, d);
		printf(file_result);
	}

}