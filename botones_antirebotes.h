/*********************************************************************************************
* Fichero:		botones_antirebotes.h
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que usan button.c y latido.c para eliminar los rebotes de botones
*********************************************************************************************/
#ifndef BOTONES_ANTIREBOTES_H
#define BOTONES_ANTIREBOTES_H
#include "debug.h"

void inicio_antirebotes();
void antirebotes_check();
int get_elegido();
int get_estado_boton();
void reset_button_count();

#endif
