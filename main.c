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

//Tablero
char tab[8][8];

/*--- funciones externas ---*/

//extern void excepcion_dabt();
//extern void excepcion_swi();
//extern void excepcion_udef();

extern void modo_usuario();

//extern void Lcd_Test();


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
	timer2_parar();
}

void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	Lcd_inicio();
	TS_init();
	D8Led_init();       // inicializamos el 8led
	timer2_inicializar();
	timer0_inicializar();
	inicio_antirebotes();
	latido_inicializar();
	timer2_empezar();
	time=timer2_leer();
	inicializar_excepciones();
	//Calibramos pantalla
	calibrar();
	//excepcion_swi();

	//Entramos en modo usuario
//	int palabra;
//	asm("MRS %0 ,CPSR" : "=r"(palabra) );
//	palabra= (palabra & 0xffffff00)|0x10; //Modo usuario
//	asm("MSR CPSR_cxsf,%0" : : "r"(palabra));

	//	modo_usuario();
	//Entramos en el bucle principal de juego
	reversi_main();

}
