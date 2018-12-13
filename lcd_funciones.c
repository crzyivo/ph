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

/***********FUNCIONES AUXILIARES****************/
void limpiarCasilla(int fila, int columna){
	INT16U top,left,bottom,right;
	top=tablero[fila][columna].yCoord+1;
	bottom=tablero[fila+1][columna].yCoord - 1;
	left=tablero[fila][columna].xCoord+1;
	right=tablero[fila+1][columna+1].xCoord-1;


	LcdClrRect(left,top,right,bottom,WHITE);
	Lcd_Dma_Trans();
}

/* itoa:  convert n to characters in s
 *
 */
void itoa(int n, INT8U s[],int len)
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = len-2;
    while(i>=0){
    	s[i]=n%10+'0';
    	n/= 10;
    	i--;
    }
    if (sign < 0)
        s[i++] = '-';
    s[len-1] = '\0';

}



/************************************************/

void Lcd_pantalla_inicio(){

}





void Lcd_inicio(){
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
	iniciarTablero();
}

void Lcd_dibujarTablero(/*char tablero[dim][dim], int dim*/){	//TODO: establecer inicio, ahora solo pinta el tablero
	int h_init=10,v_init=10,i;
	Lcd_Draw_HLine(0,218,0,BLACK,1);
	Lcd_Draw_VLine(0,218,0,BLACK,1);
	INT8U numero;	//TODO: itoa

	for(i=0;i<9;i++){
		Lcd_Draw_HLine(0,218,h_init,BLACK,1);
		Lcd_Draw_VLine(0,218,v_init,BLACK,1);
		itoa(i+1,numero,4);
		Lcd_DspAscII6x8(v_init+10,8,BLACK,numero);
		Lcd_DspAscII6x8(h_init+18,3,BLACK,numero);
		h_init+=26; v_init+=26;
	}


	//Lcd_Draw_Box(10,40,310,230,14);
	Lcd_DspAscII8x16(10,223,BLACK,"Pulse para jugar");
}

void Lcd_pintar_ficha(int fila, int columna, INT8U color){
	INT16U xPos=tablero[fila][columna].xCoord + 9;
	INT16U yPos=tablero[fila][columna].yCoord + 5;
	INT8U* f;
	switch (color) {
		case BLACK:
			 f="X";
			break;
		case WHITE:
			 f="O";
			break;
		default:	//gris
			f="*";
			break;
	}
	Lcd_DspAscII8x16(xPos,yPos,BLACK,f);

}

void Lcd_mover_ficha(int filaIni, int columnaIni, int filaFin, int columnaFin, INT8U color){
	limpiarCasilla(filaIni,columnaIni);
	Lcd_pintar_ficha(filaFin,columnaFin,color);
}

void Lcd_tiempo_total(int tiempo){
	LcdClrRect(220,5,319,21,WHITE);
	INT8U stiempo[4];
	itoa(tiempo,stiempo,4);
	Lcd_DspAscII8x16(222,10,BLACK,stiempo);
}

void Lcd_tiempo_acumulado(unsigned int t_patron,unsigned int t_calc,int veces){
	LcdClrRect(220,23,319,39,WHITE); //Limpio t_calc
	LcdClrRect(220,41,319,57,WHITE); //Limpio t_patron
	LcdClrRect(220,60,319,76,WHITE); //Limpio veces

	INT8U st_calc[4];
	INT8U st_patron[4];
	INT8U sveces[4];

	itoa(t_patron,st_calc[4],4);
	itoa(t_patron,st_patron[4],4);
	itoa(t_patron,sveces[4],4);

	Lcd_DspAscII8x16(222,28,BLACK,st_calc);
	Lcd_DspAscII8x16(222,46,BLACK,st_patron);
	Lcd_DspAscII8x16(222,66,BLACK,sveces);
}




