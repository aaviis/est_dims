#include <stdlib.h>
#include <stdio.h>
// #include < string.h >
#include <math.h>
#include <time.h>
#include "config.h"



// ступенчатая функция Хевисайда
double get_func_theta(double x)
{
	double res = 1;
	if (x < 0)
	{
		res = 0;
	}
	return res;
}

void get_vector(double *a, int n, int num_vi, int m, int p, double **arr, size_t *arr_len)
{
	double *result = malloc(m);
	int t;
	int stop = n - 1 - num_vi - m * p;
	if (stop > 0)
	{
		if (result)
		{
			// элементы одного вектора
			for (int mi = 0; mi < m; mi++)
			{
				t = n - 1 - num_vi - mi * p;
				result[mi] = a[t];
				// printf("%d=%.3f ", t,result[mi]);
			}
		}
	}
	*arr = result;
	*arr_len = m;
}

// рассчет корреляционного интеграла по методу Грассбергера-Прокаччиа
// т.е. рассматриваем множество точек в m-мерном пространстве как
// фазовый портрет аттрактора и оцениваем его размерность
void estandsave_corrint(double *a, const char *file_result)
{
	// for (int j = 0; j < N; j++) {
	// 	printf("Test %i d: %lf\n", j, a[j]);
	// 	//printf("Test %i d: %e\n", N-1, a[N-1]);
	// }
	// расчет количества значений эпсилон
	// int num = ceil(log(MAX_EPS/MIN_EPS)/log(SPEED_EPS));
	int num = ceil((MAX_EPS - MIN_EPS) / SPEED_EPS);
	printf("Number of epsilon: %i\n", num);
	double eps[num];
	int i;
	double j = MIN_EPS;
	for (i = 0; i < num; i++)
	{
		j = j + SPEED_EPS;
		eps[i] = j;
		if (j > MAX_EPS)
		{
			break;
		}
		//printf("eps: %f\n", eps[i]);
	}

	int num_m = ceil((MAX_M - MIN_M) / SPEED_M);
	printf("Number of m: %i\n", num_m+1);
	i = 0;
	int ms[num_m];
	for (i = 0; i <= num_m; i++)
	{
		ms[i] = MIN_M + i * SPEED_M;
		// printf("ms: %f\n", ms[i]);
	}
	double ep;
	int num_all = (num_m + 1) * num;
	//printf("Number lines: %i\n", num_all);

	int res_m[num_all];
	double res_ep[num_all];
	double res_C[num_all];

	int msgi = 0;	   // количество строк для файла
	int res_count = 0; // суммарное число строк в итоге, для проверки

	for (int mi = 0; mi <= num_m; mi++)
	{

		int m = ms[mi];
		printf("Estimating... m = %i\n", m);
		double M = N + 1 - m * P; // количество векторов заданной размерности
		//printf("Count of vectors fixed dimension m = %d: M = %lf\n", m, M);
		if (M > 2)
		{
			for (int epi = 0; epi < num; epi++)
			{
				ep = eps[epi];

				int k = 0; // перебор двух векторов
				int h = 0;

				double summ = 0;
				double C = 0;
				// проводим суммирование по всем парам точек нашего фрактального
				// множества с радиус-векторами v1, v2
				for (k = 0; k < M; k++)
				{
					for (h = 0; h < M; h++)
					{
						//printf("k =%i \t h = %i\n", k, h);
						if (k != h)
						{
							// считаем дистанцию между векторами
							double dist = 0;

							int t1, t2;
							double v1, v2;
							int stop1 = N + 1 - h - m * P;
							int stop2 = N + 1 - k - m * P;
							//printf("stop1 =%i \t stop2 = %i\n", stop1, stop2);
							// тут проверка на вылет индекса в отрицательную область
							if ((stop1 > 0) & (stop2 > 0))
							{
								// элементы вектора
								for (int w = 0; w < m; w++)
								{
									//printf("w\n");
									t1 = N - 1 - k - w * P;
									//printf("t1=%i\n",t1);
									t2 = N - 1 - h - w * P;
									//printf("t2=%i\n",t2);
									v1 = a[t1];
									v2 = a[t2];									
									dist += (v1 - v2) * (v1 - v2);
									//printf("\t t1 = %i, t2 = %i \n", t1, t2);
									//printf("\t v1 = %.30lf, v2 = %.30lf \n", v1, v2);
								}
							}
							//printf("k=%d, h=%d : dist: %.30f  sqrt(dist): %.30f\n", k, h, dist, sqrt(dist));
							double s = ep - sqrt(dist);
							double t = get_func_theta(s);
							summ += t;

							break;
						}
					}
				}

				double mpow = M * (M - 1);
				// парный корреляционный интеграл, определяет число пар точек n,m, для
				// которых расстояние между ними меньше, чем эпсилон -
				// т.е. поделенная на M^2 вероятность того, что две наугад взятые точки
				// разделены расстоянием меньшим, чем эпсилон
				C = summ / mpow;

				// printf("summ = %lf, M = %lf, mpow = %lf, C = %e\n", summ, M, mpow, C);
				// printf("%d, ep = %.3f,  C = %.30f\n", m, ep, C);
				// сохраняем данные в массивы
				res_m[msgi] = m;
				res_ep[msgi] = ep;
				res_C[msgi] = C;

				//printf("!!! m = %i, ep = %f,  C = %0.30lf\n", res_m[msgi], res_ep[msgi], res_C[msgi]);

				msgi += 1;
				res_count += 1;
			}
		}
	}

	// сохранить расчет размерности, логарифм(эпсилон) и логарифм(корр интеграла)
	FILE *fout;
	fout = fopen(file_result, "w");
	printf("Save to file %s\n", file_result);
	if (fout == 0)
	{
		fprintf(stderr, "Error open file ", file_result, "\n");
	}

	fseek(fout, 0, SEEK_END);
	msgi = 0;
	// fprintf(fout, "m;ep;C\n");
	for (msgi = 0; msgi < res_count; msgi++)
	{		
		//if (isinf(log_C)) 			
		fprintf(fout, "%i;%0.10lf;%0.30lf;\n", res_m[msgi], log(res_ep[msgi]), log(res_C[msgi]));
	}
	fclose(fout);
}





// функция расчета мнк по корреляционному интегралу 1 проход
// при фиксированных p и m
void mnk(int num_m, int num_eps, int size, int res_m[], double res_ep[], double res_C[], const char *file_result2)
{
	// Выделение памяти под массивы a и b
	int *mm = (int *)malloc(num_m * sizeof(int));
	double *a = (double *)malloc(num_m * sizeof(double));
	double *b = (double *)malloc(num_m * sizeof(double));

	clock_t start = clock();
	int i = 0; 
	// считаем МНК по линии корр интеграла
	for (int j = 0; j < num_m; j++)
	{
		double xy = 0;
		double sumx = 0;
		double sumy = 0;
		double x2 = 0;
		double x;
		double y;
		int m = res_m[i];
		
		while(res_m[i] == m)
		{					
			x = res_ep[i];
			y = res_C[i];
			//printf("m = %i, %0.15lf; \t %0.15lf\n", m, x, y);
			xy = xy + x * y;
			sumx = sumx + x;
			sumy = sumy + y;

			x2 = x2 + x * x;

			i++;
		}
		double b1 = num_eps * xy - sumx * sumy;
		double b2 = num_eps * x2 - sumx * sumx;
		mm[j] = m;
		b[j] = b1 / b2;

		a[j] = (sumy - (b[j]) * sumx) / num_eps;
		//printf("m = %i, a[%d] = %0.15lf; \t b[%d] = %0.15lf\n", m, j, a[j], j, b[j]);
	
		if (i == size) {
			//printf("End of file");
		}
	}
	clock_t stop = clock();
	double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
	printf("Time elapsed all in ms: %f \n", elapsed);

	// сохранить расчеты
	FILE *fout = fopen(file_result2, "w");
	printf("Save to file %s\n", file_result2);
	if (fout == 0)
	{
		fprintf(stderr, "Error open file ", file_result2, "\n");
	}

	fseek(fout, 0, SEEK_END);
	for (int k = 0; k < num_m; k++)
	{
		fprintf(fout, "%i;%lf;%lf\n", mm[k], a[k], b[k]);
	}
	fclose(fout);
}
