/*********************************************************************************************
* Fichero:		latido.h
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que usan el timer0 para generar el latido y programar callbacks
*********************************************************************************************/

#ifndef _LATIDO_H_
#define _LATIDO_H_
#include "debug.h"

int switch_leds_l;
/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/
void latido_inicializar(void);
void latido_check();

#endif /* _LATIDO_H_ */
