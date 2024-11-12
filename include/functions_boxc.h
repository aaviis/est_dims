#ifndef _FUNCTIONS_BOXC_H_
#define _FUNCTIONS_BOXC_H_
 
#include <stdio.h>
#include <stdlib.h>

//расчет умный - по номеру кубика, куда попал вектор
extern void estandsave_boxc(double *data, const char *file_result);

//расчет прямым перебором кубиков
extern void estandsave_boxc2(double *data, const char *file_result);


#endif