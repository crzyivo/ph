/*********************************************************************************************
* Fichero:	main.c
* Autor:
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/
/*--- ficheros de cabecera ---*/
#include "debug.h"
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "timer0.h"
#include "timer2.h"
#include "44blib.h"
#include "44b.h"
#include "latido.h"
#include "tp.h"
#include "botones_antirebotes.h"
#include "tratamiento_excepciones.h"
#include "reversi8_2018.h"
#include "tp.h"
#include "lcd.h"
#include "lcd_funciones.h"

/*--- variables globales ---*/
unsigned int time;
	//Maquina de estados
	typedef enum{fila_standby,fila_eleccion,columna_standby,columna_eleccion,jugada}
	maquina_reversi;
 	//Estado actual de la maquina
	maquina_reversi estado_main=fila_standby;
	//LCD
	char yn;

/*--- funciones externas ---*/

extern void excepcion_dabt();
extern void excepcion_swi();
extern void excepcion_udef();

//extern void Lcd_Test();


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





void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	//timer_init();	    // Inicializacion del temporizador
	//button_iniciar();	// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led
	D8Led_init();       // inicializamos el 8led
	timer2_inicializar();
	timer0_inicializar();
	inicio_antirebotes();
	latido_inicializar();
	time=timer2_leer();
	inicializar_excepciones();
	//excepcion_swi();
	reversi_main();

    /******************/
	/* user interface */
	/******************/
	Lcd_inicio();
	Lcd_Active_Clr();
	//TS_Test();

	while(1)
	 { }

	TS_close();

}

void reversi_main(){

	int done=0;
	int mov=0;
	int fin=0;
	reversi8_init(); //Inicializa tableros
	int fila=0;
	int columna=0;

	Lcd_Test();
	//TS_Test();
	char yn;
	while(1){
		latido_check();
		antirebotes_check();
		   //else break;
		   //TS_close();
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


