/*********************************************************************************************
* Fichero:		latido.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que usan el timer0 para generar el latido y programar callbacks
*********************************************************************************************/

/*--- ficheros de cabecera ---*/

#include "44b.h"
#include "44blib.h"
#include "latido.h"
#include "led.h"
#include "pilaDebug.h"
#include "timer2.h"


/*--- variables globales ---*/
int n_set = 5;
/**
 * Contadores asignados hasta ahora
 * 0 -> latido
 * 1 -> botones_antirebotes
 * 2 -> parpadeo 8led/tiempos de espera en main
 * 3 -> tiempo de juego
 * 4 -> parpadeo ficha
 */
int contadores[] = {0,0,0,0,0} ;
int set[] = {0,0,0,0,0};

//Funcion de callback para las esperas de retardo
void (*funcion_callback)(void);

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer0_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
/* Rutina de servicio de interrupción para timer2 */
void timer0_ISR(void)
{
#ifndef EMU
	int i;
	for(i=0;i<5;i++){
		int aux_set = set[i];
		if(set[i]==1){
			int aux = contadores[i];
			contadores[i]--;
			if(contadores[i]==0){
				set[i]=0;
			}
			if(contadores[0]==1){
				led1_on();
			}
		}
	}
	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/

	rI_ISPC |= BIT_TIMER0; // BIT_TIMER0 está definido en 44b.h y pone un uno en el bit 11 que correponde al Timer0
#endif

}

/* Función que inicializa el timer2, dejandolo listo para empezar la cuenta con timer2_empezar() */
void timer0_inicializar(void)
{
#ifndef EMU
	led1_off();
	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER0); // habilitamos en vector de mascaras de interrupcion el Timer2 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER2 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER0 = (unsigned) timer0_ISR;

	/* Configura el Timer0 */
	rTCFG0 = (rTCFG0 & 0xffffff00) | 0x00000009; // Preescalado del timer2 en bits [7:0] de TCFG0. Dividimos entre 1 para el maximo valor

	/*****************************************
	 * Bits de seleccion del MUX timer 0:	 *
	 * 1/2 -> 0000	1/4-> 0001				 *
	 * 1/8 -> 0010	1/16-> 0011	1/32 -> 0100 *
	 *****************************************/
	rTCFG1 &= (rTCFG1 & 0xfffffff0) | 0x00000004; // Selección del mux para el divisor de frecuencia, bits [3:0] para timer2.
	rTCNTB0 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB0 = 64535 ;// valor de comparación (valor original 12800)
	//Timer control register, para timer0 bits [3:0] -> [3] auto-reload, [2] output inverter, [1] manual update, [0] start/stop
	/* establecer update=manual (bit 1), inverter=off (0 en bit 2)*/
	rTCON = (rTCON & 0xfffffff0) | 0x00000002;

	//Iniciamos el latido
	rTCON ^= 0x0B;
	//rTCON = 0x09;
#endif
}

void timer0_set(int i, int cuenta){
	contadores[i]=cuenta;
	set[i]=1;
}

int timer0_get(int i){
	return contadores[i];
}

