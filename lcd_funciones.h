/*
 * lcd_funciones.h
 *
 *  Created on: 04/12/2018
 *      Author: guest
 */

#ifndef LCD_FUNCIONES_H_
#define LCD_FUNCIONES_H_

void iniciarTablero();

void Lcd_inicio(void);
void Lcd_tiempo_total(int);
void Lcd_dibujarTablero(void);
void Lcd_pintar_ficha(int fila, int columna, INT8U color);

#endif /* LCD_FUNCIONES_H_ */
