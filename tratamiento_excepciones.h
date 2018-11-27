/*********************************************************************************************
* Fichero:		tratamiento_excepciones.h
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		funciones para tratar excepciones data_abort,undefined e swi
*********************************************************************************************/

#ifndef TRATAMIENTO_EXCEPCIONES_H_
#define TRATAMIENTO_EXCEPCIONES_H_
#include "debug.h"

void tratamiento_excepcion(void);
void inicializar_excepciones();

#endif /* TRATAMIENTO_EXCEPCIONES_H_ */
