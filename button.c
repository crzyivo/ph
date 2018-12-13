/*********************************************************************************************
* Fichero:	button.c
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "button.h"
#include "8led.h"
#include "44blib.h"
#include "44b.h"
#include "def.h"
#include "pilaDebug.h"
#include "timer2.h"


/*--- variables globales del módulo ---*/
/* int_count la utilizamos para sacar un número por el 8led.
  Cuando se pulsa un botón sumamos y con el otro restamos. ¡A veces hay rebotes! */

//Variable donde guardamos el ultimo boton pulsado
estado_button boton_pulsado = button_none;

//Funcion que se usará como callback
void (*funcion_callback)(estado_button);

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de funciones ---*/
void Eint4567_ISR(void)
{
	push_debug(4,timer2_leer());
	rINTMSK |= BIT_EINT4567; // deshabilitar interrupciones de botones

	funcion_callback(rEXTINTPND);

	rEXTINTPND = 0xf;         // borrar los bits en EXTINTPND
	rINTMSK &= ~BIT_EINT4567; // habilitar interrupciones de botones
	rI_ISPC |= BIT_EINT4567;  // marcar interrupción como atendida
}

void button_iniciar(void)
{
#ifndef EMU
	/* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
	rI_ISPC    = 0x3ffffff;	// Borra INTPND escribiendo 1s en I_ISPC
	rEXTINTPND = 0xf;       // Borra EXTINTPND escribiendo 1s en el propio registro
	rINTMOD    = 0x0;		// Configura las linas como de tipo IRQ
	rINTCON    = 0x1;	    // Habilita int. vectorizadas y la linea IRQ (FIQ no)

	/* Configuracion del puerto G */
	rPCONG  = 0xffff;        		// Establece la funcion de los pines (EINT0-7)
	rPUPG   = 0x0;                  // Habilita el "pull up" del puerto
	rEXTINT = rEXTINT | 0x22222222;   // Configura las lineas de int. como de flanco de bajada

	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	rI_ISPC    |= (BIT_EINT4567);
	rEXTINTPND = 0xf;
#endif
}
void button_empezar (void (*callback)(estado_button)){
#ifndef EMU
	/* Finalizar ISR */
	rINTMSK    &= ~(BIT_EINT4567); //habilitar interrupciones de botones
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	funcion_callback = callback;
	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (int)Eint4567_ISR;
#else
	callback(button_iz);
#endif
}

estado_button button_estado(){
#ifndef EMU
	if((rPDATG & 0x40) ==0){
		return button_iz;
	}
	if((rPDATG & 0x80) ==0){
		return button_dr;
	}
#endif
 return button_none;

}
