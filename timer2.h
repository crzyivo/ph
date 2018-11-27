/*********************************************************************************************
* Fichero:		timer2.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Cabeceras de funciones de timer2
* Version: 1.0
*********************************************************************************************/

#ifndef _TIMER2_H_
#define _TIMER2_H_
#include "debug.h"

/*--- declaracion de funciones visibles del módulo timer2.c/timer2.h ---*/
void timer2_inicializar(void);

void timer2_empezar(void);

unsigned int timer2_parar(void);

unsigned int timer2_leer();

#endif /* _TIMER2_H_ */
