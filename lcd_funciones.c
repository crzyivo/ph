/*
 * lcd_funciones.c
 *
 */
#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#include "Bmp.h"

struct tablero{
	INT16 xCoord, yCoord;
}tablero[8][8];

void iniciarTablero(){
	int i,j;
	for(i=0; i<8; i++){//fila
		for(j=0; j<8; j++){//columna
			tablero[i][j].yCoord=10+i*26;	//fila
			tablero[i][j].xCoord=10+j*26;	//columna
		}
	}
}

void Lcd_inicio(){	//TODO: establecer inicio, ahora solo pinta el tablero
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
	int h_init=10,v_init=10,i;
	Lcd_Draw_HLine(0,218,0,BLACK,1);
	Lcd_Draw_VLine(0,218,0,BLACK,1);
	for(i=0;i<9;i++){
		Lcd_Draw_HLine(0,218,h_init,BLACK,1);
		Lcd_Draw_VLine(0,218,v_init,BLACK,1);
		h_init+=26; v_init+=26;
	}


	//Lcd_Draw_Box(10,40,310,230,14);
	Lcd_DspAscII8x16(10,223,BLACK,"Line 8x16");
	Lcd_Dma_Trans();
}

void Lcd_pintar_ficha(int fila, int columna, INT8U color){
	Lcd_Draw_Box(tablero[fila][columna].yCoord+3,tablero[fila][columna]+3
			,tablero[fila][columna]+23,tablero[fila][columna]+23,color);
}




