/*********************************************************************************************
* Fichero:		timer.h
* Autor:
* Descrip:		funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

#ifndef _LATIDO_H_
#define _LATIDO_H_
#include "debug.h"

int switch_leds_l;
/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/
void latido_inicializar(void);
void espera_ticks(int,void (*funcion)(void));

#endif /* _LATIDO_H_ */
