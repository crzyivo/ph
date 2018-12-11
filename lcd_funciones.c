/*
 * lcd_funciones.c
 *
 */
#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#include "Bmp.h"
void itoa(int n, INT8U s[],int len);
void Lcd_inicio(){	//tablero
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
	Lcd_DspAscII8x16(10,223,BLACK,"linea 8x16");
	Lcd_Dma_Trans();


}

void escribe_tiempo_total(int tiempo){
	LcdClrRect(220,5,320,21,WHITE);
	Lcd_Dma_Trans();
	INT8U stiempo[4];
	itoa(tiempo,stiempo,4);
	Lcd_DspAscII8x16(222,10,BLACK,stiempo);
	Lcd_Dma_Trans();
}

/* itoa:  convert n to characters in s
 * https://en.wikibooks.org/wiki/C_Programming/stdlib.h/itoa
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


