/*********************************************************************************************
* Fichero:		tratamiento_excepciones.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		funciones para tratar excepciones data_abort,undefined e swi
*********************************************************************************************/


#include "tratamiento_excepciones.h"
#include "44b.h"
#include "44blib.h"
#include "8led.h"
#include "pilaDebug.h"
#include "timer2.h"

unsigned int tiempo=0;
#ifdef EMU
	//Cuenta el número de excepciones de cada tipo que se han
	// generado excErr[0]->DABT excErr[1]->UDEF excErr[2]->SWI
	//solo se declara si estamos en el simulador (indef)
	volatile unsigned int excErr[3];	// inicializar
#endif

/*
 * Bibliografia:
 * 	https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html
 * 	http://www.ethernut.de/en/documents/arm-inline-asm.html
 *
 */


void tratamiento_excepcion( void ) __attribute__((interrupt ( "UNDEF" )));
void tratamiento_excepcion( void ) __attribute__((interrupt ( "SWI" )));
void tratamiento_excepcion( void ) __attribute__((interrupt ( "ABORT" )));

// Códigos de excepción: 0->DABT 1->UDEF 2->SWI
void tratamiento_excepcion(void){	//pasar codigo error??
	int cod_Exc=-1;
	int palabra,estadoSWI,modo=0;
	 asm("MRS %0 ,CPSR" : "=r"(palabra) );
	 palabra= palabra & 0xffffff7f;
	 asm("MSR CPSR_c,%0" : : "r"(palabra));
	 modo= palabra & 0x1F;

	 switch(modo){
	 	case 23:	//DABT 10111 (23)	0x17
	 		cod_Exc=0;
	 		 break;
	 	case 27: 	//UDEF 11011 (27)	0x1B
	 		cod_Exc=1;
	 		break;
	 	case 19:	//SWI (modo supervisor (10011)) (19) 0x13
	 		cod_Exc=2;
	 		break;

	 }


	/*Dos formas (simulador y placa)*/

#ifndef EMU
	 latido_inicializar();
	 unsigned int stamp= timer2_leer();
	 push_debug(cod_Exc,stamp);	//TODO: revisar push de tiempo
	 D8Led_blink(cod_Exc);

#else//Simulador

	if(cod_Exc >= 0 && cod_Exc <= 2){
		excErr[cod_Exc]+=1;
	}
#endif
	while(1){
	}

}


void inicializar_excepciones() {
	pISR_UNDEF = (unsigned) tratamiento_excepcion;
	pISR_SWI = (unsigned) tratamiento_excepcion;
	pISR_DABORT = (unsigned) tratamiento_excepcion;
}






