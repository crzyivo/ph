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
		if(timer0_get(2)==0){
			D8Led_symbol(blink_number);
			blink=0;
		}
	}else{
		if(timer0_get()==0){
			D8Led_symbol(16);
			blink=1;
		}
	}
	D8Led_blink(blink_number);
}

//Funcion que genera el parpadeo usando el timer0 en latido.c
void D8Led_blink(int value)
{
#ifndef EMU
	blink_number=value;
	if(blink){
		//Estoy mostrando el 8 led apagado, lo dejo menos tiempo
		timer0_set(2,5);
		timer_callback();
	}else{
		// Estoy mostrando el numero en el 8 led,  lo dejo mas tiempo
		timer0_set(2,25);
		timer_callback();
	}

#endif
}
