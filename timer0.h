/*********************************************************************************************
* Fichero:		latido.h
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que usan el timer0 para generar el latido y programar callbacks
*********************************************************************************************/

#ifndef _TIMER0_H_
#define _TIMER0_H_
#include "debug.h"

/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/
void timer0_inicializar(void);
void timer0_set(int,int);
int timer0_get(int);

#endif /* _TIMER0_H_ */
