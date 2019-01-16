/*********************************************************************************************
* Fichero:		latido.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que usan el timer0 para generar el latido y programar callbacks
*********************************************************************************************/

/*--- ficheros de cabecera ---*/

#include "44b.h"
#include "44blib.h"
#include "latido.h"
#include "timer0.h"
#include "pilaDebug.h"
#include "timer2.h"

//Variables para la gestion de esperas de rebotes
int en_espera = 0;
int cuenta_espera = 0;
int espera = 51; //Como solo se cuentan 50 interrupciones, a 51 nunca llega

/*--- codigo de las funciones ---*/
/* Rutina de servicio de interrupción para timer2 */
void latido_check(void)
{
#ifndef EMU

	int tick=timer0_get(0);

	if(tick == 26){
		led1_on();
		push_debug(3,timer2_leer());
	}
	if(tick == 25){
		led1_off();
	}

	if(tick==1){
		led1_on();
		push_debug(3,timer2_leer());
	}
	if(tick==0){
		led1_off();
		timer0_set(0,50);
	}
#endif

}

/* Función que inicializa el timer2, dejandolo listo para empezar la cuenta con timer2_empezar() */
void latido_inicializar(void)
{
#ifndef EMU
	led1_off();
	timer0_set(0,50);
#endif
}

