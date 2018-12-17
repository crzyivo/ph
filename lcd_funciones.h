/*
 * lcd_funciones.h
 *
 *  Created on: 04/12/2018
 *      Author: guest
 */

#ifndef LCD_FUNCIONES_H_
#define LCD_FUNCIONES_H_




void Lcd_inicio(void);

/***********FUNCIONES AUXILIARES****************/

void Lcd_dibujarTablero(char *);
void Lcd_dibujar_circulo(INT16U,INT16U,INT8U);
void Lcd_limpiar_casilla(int fila, int columna);
/************************************************/


void Lcd_pintar_ficha(int, int, char);
void Lcd_mover_ficha(int, int, int, int, INT8U);
void Lcd_tiempo_total(int);
void Lcd_tiempo_acumulado(unsigned int,unsigned int,int);
void Lcd_texto_calibracion(char*);
void Lcd_texto_cancelar();
void Lcd_texto_fin();


#endif /* LCD_FUNCIONES_H_ */
