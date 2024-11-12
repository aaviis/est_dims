#ifndef _FUNCTIONS_GP_H_
#define _FUNCTIONS_GP_H_
 
#include <stdio.h>
#include <stdlib.h>

extern void estandsave_corrint(double * data, char namefile[]);
extern void estandsave_boxc(double *data, const char *file_result);

extern void mnk(int num_m, int num_eps,int size, int res_m[], double res_ep[], double res_C[], const char *file_result2);
#endif