/*
 * reversi8_2018.h
 *
 *  Created on: 20/11/2018
 *      Author: Ivo
 */

#ifndef REVERSI8_2018_H_
#define REVERSI8_2018_H_
#include "debug.h"
enum { DIM=8 };
void reversi8_init();
int reversi8_jugada(char fila_h, char col_h);
void get_tablero(char [][DIM]);
void reversi_main();
unsigned int get_tiempo_patron_volteo();
int get_veces_patron_volteo();


#endif /* REVERSI8_2018_H_ */
