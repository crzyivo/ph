/*********************************************************************************************
* Fichero:	8led.c
* Autor:
* Descrip:	Funciones de control del display 8-segmentos
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "44b.h"
#include "44blib.h"
#include "timer0.h"

/*--- variables globales del módulo ---*/
int blink_in=0;
int blink_out=0;
int blink_number=blank;
/* tabla de segmentos */
static int
Symbol[size_8led] = { cero, uno, dos, tres, cuatro, cinco, seis, siete, ocho, nueve, A, B, C, D, E, F, blank};

/*--- código de las funciones ---*/
void D8Led_init(void)
{
#ifndef EMU
	/* Estado inicial del display con todos los segmentos iluminados
	   (buscar en los ficheros de cabecera la direccion implicada) */
	LED8ADDR = (unsigned char) cero;
#endif
}

void D8Led_symbol(int value)
{
#ifndef EMU
	/* muestra el Symbol[value] en el display (analogo al caso anterior) */
	if ((value >= 0) && (value < size_8led)) {
		LED8ADDR = (unsigned char) Symbol[value];
	}
#endif
}

//Funcion que genera el parpadeo usando el timer0 en latido.c
void D8Led_blink(int value)
{
#ifndef EMU
	blink_number=value;
	if(timer0_get(2)==0 && blink_in){
		D8Led_symbol(blink_number);
		blink_in=0;
		timer0_set(2,25);
		blink_out=1;
	}
	else if(timer0_get(2)==0 && blink_out){
		D8Led_symbol(16);
		blink_out=0;
	}
	if(!blink_in && !blink_out){
		//Estoy mostrando el 8 led apagado, lo dejo menos tiempo
		timer0_set(2,5);
		blink_in=1;
	}

#endif
}
