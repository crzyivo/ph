/*
 * lcd_funciones.c
 *
 */
#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#include "Bmp.h"
#include "lcd_funciones.h"

enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2,
FICHA_GRIS = 3
};

//Comprobacion de coordenadas de tp
ULONG X_MIN_tp;
ULONG Y_MIN_tp;
ULONG X_MAX_tp;
ULONG Y_MAX_tp;

void Lcd_texto_jugar();

/***********FUNCIONES AUXILIARES****************/
void Lcd_limpiar_casilla(int fila, int columna){
	INT16U top,left,bottom,right;	//TODO:repasar por que limpia el inicio de la fila/columna

	top=columna*26+10+1;
	bottom=(columna+1)*26+10 - 1;
	left=fila*26+10+1;
	right=(fila+1)*26+10-1;

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
 * int n: Numero a pasar a string
 * unsigned char s[]: Array de caracteres de longitud len
 * len: Longitud del array s, teniendo en cuenta que es n de digitos + el fin \0
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

/* itoa:  convert n to characters in s
 * unsigned int n: Numero a pasar a string
 * unsigned char s[]: Array de caracteres de longitud len
 * len: Longitud del array s, teniendo en cuenta que es n de digitos + el fin \0
 */
void utoa(unsigned int n, INT8U s[],int len){
    int i;
    i = len-2;
    while(i>=0){
    	s[i]=n%10+'0';
    	n/= 10;
    	i--;
    }
    s[len-1] = '\0';
}
/************************************************/

//Función encargada de pintar la pantalla de inicio con instrucciones
void Lcd_pantalla_inicio(){

	Lcd_DspAscII8x16(132,14,BLACK,"REVERSI");

	Lcd_DspAscII8x16(25,54,BLACK,"Coloca fichas negras para voltear");
	Lcd_DspAscII8x16(25,74,BLACK," fichas blancas.");
	Lcd_DspAscII8x16(25,94,BLACK,"Mueve la ficha con los botones:");
	Lcd_DspAscII8x16(35,114,BLACK,"Izquierdo => Columnas");
	Lcd_DspAscII8x16(35,134,BLACK,"Derecho => Filas");
	Lcd_DspAscII8x16(25,154,BLACK,"Toca en el centro de la pantalla");
	Lcd_DspAscII8x16(25,174,BLACK,"para acabar tu turno");

	Lcd_Draw_Box(10,40,310,200,BLACK);
	Lcd_DspAscII8x16(36,215,BLACK,"TOQUE LA PANTALLA PARA EMPEZAR");
}

//Función que inicializa la librería lcd_funciones
void Lcd_inicio(){
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
}

//Función que dibuja un tablero dado
void Lcd_dibujarTablero(char t[][8]){
	int h_init=10,v_init=10;
	volatile int i,j;
	LcdClrRect(0,0,218,218,WHITE);
	Lcd_Draw_HLine(0,218,0,BLACK,1);
	Lcd_Draw_VLine(0,218,0,BLACK,1);
	INT8U numero[2];	//TODO: con * o sin *??

	for(i=0;i<9;i++){
		Lcd_Draw_HLine(0,218,h_init,BLACK,1);
		Lcd_Draw_VLine(0,218,v_init,BLACK,1);

		if(i<8){
			itoa(i+1,numero,2);
			Lcd_DspAscII6x8(2,v_init+10,BLACK,numero);
			Lcd_DspAscII6x8(h_init+8,2,BLACK,numero);
			for(j=0; j<8; j++){
				if(t[i][j]!=CASILLA_VACIA){
					Lcd_pintar_ficha(i,j,t[i][j]);
				}
			}
		}

		h_init+=26; v_init+=26;
	}

	Lcd_texto_jugar();
}

//Función que pinta una ficha en una fila y columna dadas, en un color determinado
void Lcd_pintar_ficha(int fila, int columna,char color){
	INT16U xPos=fila*26+10 + 9;
	INT16U yPos=columna*26+10 + 5;

	char* f;
	switch (color) {
		case FICHA_NEGRA:
			 //Lcd_dibujar_circulo(xPos,yPos,BLACK);
			f="X";
			Lcd_DspAscII8x16(xPos,yPos,BLACK,f);
			break;
		case FICHA_BLANCA:
			//Lcd_dibujar_circulo(xPos,yPos,WHITE);
			f="O";
			Lcd_DspAscII8x16(xPos,yPos,BLACK,f);
			break;
		default:	//gris
			//Lcd_dibujar_circulo(xPos,yPos,LIGHTGRAY);
			f="*";
			Lcd_DspAscII8x16(xPos,yPos,BLACK,f);
			break;
	}
	//Lcd_DspAscII8x16(xPos,yPos,BLACK,f);

}

//Función que repinta una ficha existente ya dibujada en el tablero en fila y columna
//filaFin y columnaFin del mismo tablero
void Lcd_mover_ficha(char tablero[][8], int filaFin, int columnaFin, INT8U color){
	Lcd_dibujarTablero(tablero);
	Lcd_limpiar_casilla(filaFin,columnaFin);
	Lcd_pintar_ficha(filaFin,columnaFin,color);
}

//Funcion que pinta la leyenda del tiempo total transcurrido en el juego
void Lcd_tiempo_total(int tiempo){
	LcdClrRect(220,5,319,25,WHITE);
	Lcd_DspAscII6x8(221,5,BLACK,"Tiempo de juego");
	INT8U stiempo[4];
	itoa(tiempo,stiempo,4);
	Lcd_DspAscII8x16(222,14,BLACK,stiempo);
}

//Funcion que pinta las leyendas de tiempo acumulado en patron_volteo, tiempo total de calculo
// y numero de veces que se ha invocado a patron_volteo
void Lcd_tiempo_acumulado(unsigned int t_patron,unsigned int t_calc,int veces){
	LcdClrRect(220,32,319,62,WHITE); //Limpio t_calc
	LcdClrRect(220,65,319,94,WHITE); //Limpio t_patron
	LcdClrRect(220,97,319,126,WHITE); //Limpio veces

	INT8U st_calc[8];
	INT8U st_patron[8];
	INT8U sveces[8];

	utoa(t_patron,st_calc,8);
	utoa(t_calc,st_patron,8);
	itoa(veces,sveces,8);

	Lcd_DspAscII6x8(221,33,BLACK,"Tiempo calculo");
	Lcd_DspAscII8x16(222,44,BLACK,st_calc);

	Lcd_DspAscII6x8(221,65,BLACK,"Tiempo volteo");
	Lcd_DspAscII8x16(222,76,BLACK,st_patron);

	Lcd_DspAscII6x8(221,97,BLACK,"No de volteos");
	Lcd_DspAscII8x16(222,108,BLACK,sveces);
}

//Funcion que pinta el texto con instrucciones para el proceso de calibración,
//así como la zona que determinamos como centro del tablero para referencia
void Lcd_texto_calibracion(char* string){
	Lcd_Clr();
	Lcd_DspAscII8x16(3,3,BLACK,string);
	//TODO: Dibujar cuadro central
	Lcd_Draw_HLine(62,166,62,BLACK,1);
	Lcd_Draw_HLine(62,166,166,BLACK,1);
	Lcd_Draw_VLine(62,166,62,BLACK,1);
	Lcd_Draw_VLine(62,166,166,BLACK,1);
	Lcd_Dma_Trans();
}

//Función que muestra la leyenda durante la elección de ficha
void Lcd_texto_jugar(){
	LcdClrRect(10,223,223,239,WHITE);
	Lcd_DspAscII8x16(30,222,BLACK,"Pulse para jugar");
}

//Funcion que muestra la leyenda durante el proceso de cancelación tras seleccionar
//una ficha
void Lcd_texto_cancelar(){
	LcdClrRect(10,223,223,239,WHITE);
	Lcd_DspAscII8x16(30,222,BLACK,"Pulse para cancelar");
}

//Función que muestra la leyenda cuando se termina el juego
void Lcd_texto_fin(){
	LcdClrRect(9,220,319,225,WHITE);
	LcdClrRect(10,223,223,239,WHITE);
	Lcd_DspAscII8x16(10,223,BLACK,"FIN DE PARTIDA TOQUE PARA EMPEZAR");
}

/*
 * Función ejecutada al principio del programa para calibrar la pantalla táctil en la parte del tablero
 */
void calibrar(){
	volatile ULONG sdX = 1000;	//Valor mínimo de la X calibrada
	volatile ULONG sdY = 1000;	//Valor mínimo de la Y calibrada
	volatile ULONG iizqX = 0;	//Valor máximo de la X calibrada
	volatile ULONG iizqY = 0;	//Valor máximo de la Y calibrada
	ULONG tX=0;
	ULONG tY=0;
	int i;
		//Hacemos 5 medidas
		Lcd_texto_calibracion("Superior Derecha");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){DelayTime(1);}
		getXY(&tX,&tY);
		if (tX<sdX){
			sdX=tX;
		}
		if (tY<sdY){
			sdY=tY;
		}
		Delay(10000);
		setEspera_tp();
		Lcd_texto_calibracion("Inferior Izquierda");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){DelayTime(1);}
		getXY(&tX,&tY);
		if (tX>iizqX){
			iizqX=tX;
		}
		if (tY>iizqY){
			iizqY=tY;
		}
		Delay(10000);
		setEspera_tp();

	X_MIN_tp = (iizqX*62)/320;
	Y_MIN_tp = ((iizqY*62)/240)+240; //+Y?
	X_MAX_tp = (sdX*166)/320;
	Y_MAX_tp = ((sdY*166)/240)+240; //+Y?
}

//Funcion que comprueba que las coordenadas leidas de tp estan en el centro del tablero
int check_tp_centro(ULONG X, ULONG Y){
	int x_MIN_debug = X_MIN_tp;
	int y_MAX_debug = Y_MAX_tp;
	int x_MAX_debug = X_MAX_tp;
	int y_MIN_debug = Y_MIN_tp;

	return X>X_MIN_tp && Y>Y_MIN_tp && X< X_MAX_tp && Y<Y_MAX_tp;
}
