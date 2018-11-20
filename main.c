/*********************************************************************************************
* Fichero:	main.c
* Autor:
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

//#define EMU
/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "timer2.h"
#include "44blib.h"
#include "44b.h"
#include "latido.h"
#include "botones_antirebotes.h"
#include "reversi8_2018.h"
/*--- variables globales ---*/
	volatile unsigned int * rINI=0xc7ff600;
	volatile unsigned int * rFIN=0xc7ff700;
	volatile unsigned int * SPD=0xc7ff700;

#ifdef EMU
	//Cuenta el número de excepciones de cada tipo que se han
	// generado excErr[0]->DABT excErr[1]->UDEF excErr[2]->SWI
	//solo se declara si estamos en el simulador (indef)
	volatile unsigned int excErr[3];	// inicializar
#endif

	unsigned int time;

	//Maquina de estados
	typedef enum{fila_standby,fila_eleccion,columna_standby,columna_eleccion,jugada}
	maquina_reversi;
 	//Estado actual de la maquina
	maquina_reversi estado_main=fila_standby;

/*--- codigo de funciones ---*/

extern void excepcion_dabt();


void reverse_main();
//////////////////////////////////////////////////////////////////////
//	Funcion que comprueba el correcto funcionamiento del timer2
//	para ello realiza varias mediciones usando la funcion Delay,
//  comprobandose a mano que los resultados son ranozablemente aproximados
//////////////////////////////////////////////////////////////////////
void test_timer2(){
	unsigned int tiempos[8];	//Array para anotar tiempos del timer2
	unsigned int tiempo_medido_1ms = 0;
	unsigned int tiempo_medido_10ms = 0;
	unsigned int tiempo_medido_1s = 0;
	unsigned int tiempo_medido_10s = 0;
	timer2_empezar();

	tiempos[0] = timer2_leer();
	Delay(10); //1 ms
	tiempos[1] = timer2_leer();
	tiempo_medido_1ms = tiempos[1]-tiempos[0];

	tiempos[2] = timer2_leer();
	Delay(100); //10 ms
	tiempos[3] = timer2_leer();
	tiempo_medido_10ms = tiempos[3]-tiempos[2];

	tiempos[4] = timer2_leer();
	Delay(10000); //1 s
	tiempos[5] = timer2_leer();
	tiempo_medido_1s = tiempos[5]-tiempos[4];

	tiempos[6] = timer2_leer();
	Delay(100000); //10 s
	tiempos[7] = timer2_leer();
	tiempo_medido_10s = tiempos[7]-tiempos[6];
}


// Códigos de excepción: 0->DABT 1->UDEF 2->SWI
void tratamiento_excepcion(/*uint32_t*/ unsigned int cod_Exc, unsigned int dir_Inst){


	/*Dos formas (simulador y placa)-> revisar ifdef */

#ifndef EMU
	//hacer parpadear
	D8Led_blink(cod_Exc);

#endif


#ifdef EMU//Simulador

	if(cod_Exc >= 0 && cod_Exc <= 2){
		excErr[cod_Exc]+=1;
	}
#endif

}

void push_debug(/*uint32_t */ unsigned int ID_evento, /*uint32_t*/  unsigned auxData){
	unsigned int timer;//=timer2_leer();
	unsigned int timeDef=0;

	if (ID_evento == 0){//irq
		timeDef=timer-time;
		*SPD=ID_evento;
		SPD=SPD-4;
		*SPD=timeDef;
		SPD=SPD-4	;
		if (SPD < rINI) {SPD=rFIN;}
	}else{
		*SPD=ID_evento;
		SPD=SPD-4;
		*SPD=auxData;
		SPD=SPD-4	;
		if (SPD < rINI) {SPD=rFIN;}

	}
}

void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	//timer_init();	    // Inicializacion del temporizador
	//button_iniciar();	// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led
	D8Led_init();       // inicializamos el 8led
	timer2_inicializar();
	inicio_antirebotes();
	latido_inicializar();
	time=timer2_leer();
//	excepcion_dabt();

	/* Valor inicial de los leds */
	//leds_off();
	//test_timer2();
	//Dejamos los leds apagados
	//led1_on();
	//Iniciar reversi8;
	//reversi8();
	//button_empezar(button_callback);
	//Codigo para cambiar los leds segun el timer
	reversi_main();

}

void reversi_main(){
	int done=0;
	int mov=0;
	int fin=0;
	reversi8_init(); //Inicializa tableros
	while(1){
		int fila=0;
		int columna=0;
		switch(estado_main){
			case fila_standby:
				D8Led_symbol(15);
				if(get_estado_boton()==button_iz){
					estado_main=fila_eleccion;
				}
				break;
			case fila_eleccion:
				fila=get_elegido();
				if(fila != -1){
					estado_main=columna_standby;
				}
				break;
			case columna_standby:
				D8Led_symbol(12);
				if(get_estado_boton()==button_iz){
					estado_main=columna_eleccion;
				}
				break;
			case columna_eleccion:
				columna=get_elegido();
				if(columna != -1){
					estado_main=jugada;
				}
				break;
			case jugada:
				reversi8_jugada(fila,columna,&done,&mov,&fin);
				estado_main=fila_standby;
				break;
		}
	}
}
