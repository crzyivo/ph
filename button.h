/*********************************************************************************************
* Fichero:	button.h
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "debug.h"

typedef enum{
	button_none = 0,button_iz = 4,button_dr = 8
} estado_button;
/*--- declaracion de funciones visibles del módulo button.c/button.h ---*/
void button_iniciar(void);
void button_empezar(void (*callback)(estado_button));
estado_button button_estado(void);

#endif /* _BUTTON_H_ */
