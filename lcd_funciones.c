/*
 * lcd_funciones.c
 *
 */
#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#include "Bmp.h"

enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2
};


/***********FUNCIONES AUXILIARES****************/
void Lcd_limpiar_casilla(int fila, int columna){
	INT16U top,left,bottom,right;

	top=fila*26+10+1;
	bottom=fila*26+10 - 1;
	left=columna*26+10+1;
	right=columna*26+10-1;

	LcdClrRect(left,top,right,bottom,WHITE);
}
//Función que dibuja un circulo en una casilla
void Lcd_dibujar_circulo(INT16U xPos, INT16U yPos,INT8U color){
	//Box
	Lcd_Draw_Box_Width(xPos+6,yPos+7,xPos+20,yPos+19,color,6);//1
	Lcd_Draw_Box(xPos+9,yPos+5,xPos+17,yPos+6,color);//2
	Lcd_Draw_Box(xPos+9,yPos+20,xPos+21,yPos+6,color);//3
	Lcd_Draw_Box(xPos+4,yPos+11,xPos+5,yPos+15,color);//4
	Lcd_Draw_Box(xPos+21,yPos+11,xPos+22,yPos+15,color);//5
	//horizontal
	Lcd_Draw_HLine(xPos+11,xPos+15,yPos+4,color,1);//6
	Lcd_Draw_HLine(xPos+11,xPos+15,yPos+22,color,1);//7
	Lcd_Draw_HLine(xPos+7,xPos+8,yPos+6,color,1);//8
	Lcd_Draw_HLine(xPos+7,xPos+8,yPos+20,color,1);//9
	Lcd_Draw_HLine(xPos+18,xPos+19,yPos+6,color,1);//10
	Lcd_Draw_HLine(xPos+18,xPos+19,yPos+20,color,1);//11
	Lcd_Draw_HLine(xPos+12,xPos+13,yPos+13,color,1);//12
	//vertical
	Lcd_Draw_VLine(yPos+9,yPos+10,xPos+5,color,1);//13
	Lcd_Draw_VLine(yPos+16,yPos+17,xPos+5,color,1);//14
	Lcd_Draw_VLine(yPos+9,yPos+10,xPos+21,color,1);//15
	Lcd_Draw_VLine(yPos+16,yPos+17,xPos+21,color,1);//16

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

	Lcd_DspAscII8x16(132,24,BLACK,"REVERSI"); //coordenada de abajo o de arriba ??

	//TODO:poner descripcion
	Lcd_DspAscII8x16(25,64,BLACK," ");
	/*
	 *
	 */

	Lcd_Draw_Box(27,182,288,24,BLACK);
	Lcd_DspAscII8x16(36,190,BLACK,"TOQUE LA PANTALLA PARA EMPEZAR");




}





void Lcd_inicio(){
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
	//iniciarTablero();
}

void Lcd_dibujarTablero(char * t[][8]){	//TODO: pintar tablero actual
	int h_init=10,v_init=10,i,j;
	Lcd_Draw_HLine(0,218,0,BLACK,1);
	Lcd_Draw_VLine(0,218,0,BLACK,1);
	INT8U * numero;	//TODO: con * o sin *??

	//Lcd_Clr(); TODO: clear??

	for(i=0;i<9;i++){
		Lcd_Draw_HLine(0,218,h_init,BLACK,1);
		Lcd_Draw_VLine(0,218,v_init,BLACK,1);
		itoa(i+1,numero,4);
		Lcd_DspAscII6x8(v_init+10,8,BLACK,numero);
		Lcd_DspAscII6x8(h_init+18,3,BLACK,numero);
		if(i<8){
			for(j=0; j<8; j++){

			}
		}

		h_init+=26; v_init+=26;
	}


	//Lcd_Draw_Box(10,40,310,230,14);
	Lcd_DspAscII8x16(10,223,BLACK,"Pulse para jugar");
}

void Lcd_pintar_ficha(int fila, int columna, INT8U color){
	INT16U xPos=fila*26+10 + 9;
	INT16U yPos=columna*26+10 + 5;

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
	Lcd_limpiar_casilla(filaIni,columnaIni);
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




