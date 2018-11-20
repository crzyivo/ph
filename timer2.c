/*********************************************************************************************
* Fichero:		timer2.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version: 1.0
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "timer2.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales ---*/
volatile unsigned int timer2_num_int;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer2_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
/* Rutina de servicio de interrupción para timer2 */
void timer2_ISR(void)
{
	timer2_num_int++;

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 está definido en 44b.h y pone un uno en el bit 11 que correponde al Timer2
}

/* Función que inicializa el timer2, dejandolo listo para empezar la cuenta con timer2_empezar() */
void timer2_inicializar(void)
{
#ifndef EMU
	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER2); // habilitamos en vector de mascaras de interrupcion el Timer2 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER2 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER2 = (unsigned) timer2_ISR;

	/* Configura el Timer2 */
	rTCFG0 = rTCFG0 & 0xffff00ff; // Preescalado del timer2 en bits [15:8] de TCFG0. Dividimos entre 1 para el maximo valor

	/*****************************************
	 * Bits de seleccion del MUX timer 2:	 *
	 * 1/2 -> 0000	1/4-> 0001				 *
	 * 1/8 -> 0010	1/16-> 0011	1/32 -> 0100 *
	 *****************************************/
	rTCFG1 = rTCFG1 & 0xfffff0ff; // Selección del mux para el divisor de frecuencia, bits [11:8] para timer2.
	rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 0 ;// valor de comparación (valor original 12800)
	//Timer control register, para timer2 bits [15:12] -> [15] auto-reload, [14] output inverter, [13] manual update, [12] start/stop
	/* establecer update=manual (bit 13), inverter=off (0 en bit 14)*/
	rTCON = (rTCON & 0xffff0fff) | 0x00002000;
#endif
}

/* Funcion que inicia la cuenta mediante timer2*/
void timer2_empezar(void)
{
#ifndef EMU
	// Reiniciar variable contador
	timer2_num_int=0;
	//Reiniciar registro intermedio
	rTCNTO2 = 0;
	/* iniciar timer2 (bit 12) y auto-reload (bit 15)*/
	rTCON ^= 0x0000B000;
#endif
}
/* Funcion que obtiene el tiempo en microsegundos que se ha contado mediante el timer2.
 * Para obtener el tiempo sumaremos las veces que se ha realizado la resta con la fraccion de resta
 * que queda en el momento de leer. Este resultado le dividiremos la frecuencia de reloj en Mhz para obtener
 * microsegundos. En este caso la frecuencia es 64Mhz y le aplicamos un divisor 1/2, con lo que tendremos
 * que dividir entre 32.
 */
unsigned int timer2_leer()
{
#ifndef EMU
	return timer2_num_int * (rTCNTB2 * 0.03125)	//Cuentas completas
	        + ((rTCNTB2 - rTCNTO2) * 0.03125);	//Cuenta en el momento de lectura
#else
	timer2_num_int++;
	retrun timer2_num_int;
#endif
}

/* Funcion que para el timer y devuelve el tiempo transcurrido, pero no lo reinicia*/
unsigned int timer2_parar(void)
{
#ifndef EMU
	/*parar timer2, desactivamos bit 12 en TCON*/
	rTCON = rTCON & 0xffffEfff;
	return timer2_leer();
#endif
}

