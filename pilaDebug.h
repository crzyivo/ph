/*********************************************************************************************
* Fichero:		pilaDebug.h
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		gestion de la pila para debug
*********************************************************************************************/

#ifndef PILADEBUG_H_
#define PILADEBUG_H_
#include "debug.h"

void inicializar_pila();
void push_debug(unsigned int ID_evento, unsigned auxData);

#endif /* PILADEBUG_H_ */
