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

//Comprobacion de coordenadas de tp
ULONG X_MIN_tp;
ULONG Y_MIN_tp;
ULONG X_MAX_tp;
ULONG Y_MAX_tp;

/*--- funciones externas ---*/

//extern void excepcion_dabt();
//extern void excepcion_swi();
//extern void excepcion_udef();

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


/*
 * Funci�n ejecutada al principio del programa para calibrar la pantalla t�ctil en la parte del tablero
 */
void calibrar(){
	volatile ULONG sdX = 1000;	//Valor m�nimo de la X calibrada
	volatile ULONG sdY = 1000;	//Valor m�nimo de la Y calibrada
	volatile ULONG iizqX = 0;	//Valor m�ximo de la X calibrada
	volatile ULONG iizqY = 0;	//Valor m�ximo de la Y calibrada
	ULONG tX=0;
	ULONG tY=0;
	int i;
			//Hacemos 5 medidas
		Lcd_texto_calibracion("Superior Derecha");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){DelayTime(1);}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<sdX){
			sdX=tX;
		}
		if (tY<sdY){
			sdY=tY;
		}
		Delay(5000);
		Lcd_texto_calibracion("Inferior Izquierda");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){DelayTime(1);}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX>iizqX){
			iizqX=tX;
		}
		if (tY>iizqY){
			iizqY=tY;
		}
		Delay(5000);
/*
		Lcd_texto_calibracion("Inferior Izquierda");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){DelayTime(1);}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<minX){
			minX=tX;
		}
		if (tY<maxY){
			maxY=tY;
		}
		Delay(5000);
		Lcd_texto_calibracion("Inferior Derecha");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){DelayTime(1);}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<minX){
			minX=tX;
		}
		if (tY<maxY){
			maxY=tY;
		}
		Delay(5000);
*/

	X_MIN_tp = (iizqX*36)/320;
	Y_MIN_tp = (iizqY*36)/260; //+Y?
	X_MAX_tp = (sdX*140)/320;
	Y_MAX_tp = (sdY*140)/260; //+Y?
}

//Funcion que comprueba que las coordenadas leidas de tp estan en el centro del tablero
int check_tp_centro(ULONG X, ULONG Y){
	return X>X_MIN_tp && Y>Y_MIN_tp && X< X_MAX_tp && Y<Y_MAX_tp;
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
	//calibrar();
	//excepcion_swi();

	//Entramos en modo usuario
//	int palabra;
//	asm("MRS %0 ,CPSR" : "=r"(palabra) );
//	palabra= (palabra & 0xffffff00)|0x10; //Modo usuario
//	asm("MSR CPSR_cxsf,%0" : : "r"(palabra));

	//Entramos en el bucle principal de juego
	reversi_main();

}


