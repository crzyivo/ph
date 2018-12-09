/*
 * reversi8_2018.h
 *
 *  Created on: 20/11/2018
 *      Author: Ivo
 */

#ifndef REVERSI8_2018_H_
#define REVERSI8_2018_H_
#include "debug.h"

void reversi8_init();
void reversi8_jugada(char fila_h, char col_h, int* done, int* move, int* fin);
void get_tablero(char* tablero);


#endif /* REVERSI8_2018_H_ */
