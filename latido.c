/*********************************************************************************************
* Fichero:		timer2.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version: 1.0
*********************************************************************************************/

/*--- ficheros de cabecera ---*/

#include "44b.h"
#include "44blib.h"
#include "latido.h"

/*--- variables globales ---*/
volatile unsigned int n_interrupt=0;
int switch_leds_l = 0;

//Variables para la gestion de esperas de rebotes
int en_espera = 0;
int cuenta_espera = 0;
int espera = 51; //Como solo se cuentan 50 interrupciones, a 51 nunca llega

//Funcion de callback para las esperas de retardo
void (*funcion_callback)(void);

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void latido_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
/* Rutina de servicio de interrupción para timer2 */
void latido_ISR(void)
{
#ifndef EMU
	switch_leds_l ++;
	if(en_espera){ // Si estoy esperando ticks, aumento la cuenta
		cuenta_espera++;
	}
	if(cuenta_espera == espera && en_espera ){ //Compruebo si he esperado los ticks suficientes
		//Reseteo los contadores y llamo a la funcion de callback
		espera=51;
		en_espera=0;
		cuenta_espera=0;
		funcion_callback();
	}
	int aux=switch_leds_l;

	if(switch_leds_l == 24){
		led1_on();
	}
	if(switch_leds_l == 25){
		led1_off();
	}

	if(switch_leds_l==49){
		led1_on();
	}
	if(switch_leds_l==50){
		led1_off();
		switch_leds_l=0;
	}
	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/

	rI_ISPC |= BIT_TIMER0; // BIT_TIMER0 está definido en 44b.h y pone un uno en el bit 11 que correponde al Timer0
#endif

}

/* Función que inicializa el timer2, dejandolo listo para empezar la cuenta con timer2_empezar() */
void latido_inicializar(void)
{
#ifndef EMU
	led1_off();
	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER0); // habilitamos en vector de mascaras de interrupcion el Timer2 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER2 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER0 = (unsigned) latido_ISR;

	/* Configura el Timer0 */
	rTCFG0 = 0x09; // Preescalado del timer2 en bits [7:0] de TCFG0. Dividimos entre 1 para el maximo valor

	/*****************************************
	 * Bits de seleccion del MUX timer 0:	 *
	 * 1/2 -> 0000	1/4-> 0001				 *
	 * 1/8 -> 0010	1/16-> 0011	1/32 -> 0100 *
	 *****************************************/
	rTCFG1 &= 0xfffffff4; // Selección del mux para el divisor de frecuencia, bits [3:0] para timer2.
	rTCNTB0 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB0 = 64535 ;// valor de comparación (valor original 12800)
	//Timer control register, para timer0 bits [3:0] -> [3] auto-reload, [2] output inverter, [1] manual update, [0] start/stop
	/* establecer update=manual (bit 1), inverter=off (0 en bit 2)*/
	rTCON |= 0x2;

	//Iniciamos el latido
	rTCON ^= 0x0B;
	//rTCON = 0x09;
#endif
}

void espera_ticks(int ticks,void (*callback_espera)()){
	en_espera=1;
	funcion_callback = callback_espera;
	espera=ticks;
#ifdef EMU
	funcion_callback();
#endif
}

