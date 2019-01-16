/*********************************************************************************************
* Fichero:		timer0.h
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que gestionan el timer0
*********************************************************************************************/

#ifndef _TIMER0_H_
#define _TIMER0_H_
#include "debug.h"

/*--- declaracion de funciones visibles del módulo timer0.c/timer0.h ---*/
void timer0_inicializar(void);
void timer0_set(int,int);
int timer0_get(int);

#endif /* _TIMER0_H_ */
