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
//#include "latido.h"

/*--- variables globales del módulo ---*/
int blink=0;
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

void timer_callback()
{
	if(blink){
		D8Led_symbol(blink_number);
		blink=0;
	}else{
		D8Led_symbol(16);
		blink=1;
	}
	D8Led_blink(blink_number);
}

void D8Led_blink(int value)
{
#ifndef EMU
	blink_number=value;
	if(blink){
		//Estoy mostrando el 8 led apagado, lo dejo menos tiempo
		espera_ticks(5,timer_callback);
	}else{
		// Estoy mostrando el numero en el 8 led,  lo dejo mas tiempo
		espera_ticks(25,timer_callback);
	}

#endif
}
