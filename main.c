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
	typedef enum{inicio,nueva_partida,eleccion_casilla,t_cancelacion,jugada,fin_partida}
	maquina_reversi;
 	//Estado actual de la maquina
	maquina_reversi estado_main=inicio;
	//LCD
	char yn;

/*--- funciones externas ---*/

extern void excepcion_dabt();
extern void excepcion_swi();
extern void excepcion_udef();

//extern void Lcd_Test();


void reversi_main();
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
	//timer_init();	    // Inicializacion del temporizador
	//button_iniciar();	// inicializamos los pulsadores. Cada vez que se pulse se ver� reflejado en el 8led
	Lcd_inicio();
	D8Led_init();       // inicializamos el 8led
	timer2_inicializar();
	timer0_inicializar();
	inicio_antirebotes();
	latido_inicializar();
	timer2_empezar();
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
	int pintar_lcd=0;
	int timer_set_cancel=0;
	unsigned int tiempo_patron_volteo=0;
	unsigned int tiempo_calculos=0;
	unsigned int t_calculos[2];
	int veces_patron_volteo=0;
	int tiempo_juego=-1;
	reversi8_init(); //Inicializa tableros
	int fila=0;
	int columna=0;
	while(1){
		latido_check();
		antirebotes_check();
		//Miramos el tiempo total de juego
		if(timer0_get(3)==0 && tiempo_juego!=-1){
			//Pintar tiempo nuevo
			tiempo_juego++;
			Lcd_tiempo_total(tiempo_juego);
			pintar_lcd=1;
			timer0_set(3,50);
		}
		switch(estado_main){
			case inicio:
				//Dibujar mensaje de entrada y esperar touchpad
				estado_main=nueva_partida;
				break;
			case nueva_partida:
				//Dibujar el trablero y el resto de la pantalla
				Lcd_dibujarTablero();
				tiempo_juego=0;
				Lcd_tiempo_total(tiempo_juego);
				timer0_set(3,50);
				Lcd_pintar_ficha(fila,columna,BLACK);
				pintar_lcd=1;
				estado_main=eleccion_casilla;
				break;
			case eleccion_casilla:
				if(get_estado_boton()==button_iz){
					//Muevo en columnas
					columna=get_elegido();
					//Lcd_pintar_ficha(fila,(columna-1)%8,BLACK);
					Lcd_pintar_ficha(fila,columna,BLACK);
					pintar_lcd=1;
				}if(get_estado_boton()==button_dr){
					//Muevo en filas
					fila=get_elegido();
					//Lcd_pintar_ficha(fila,(columna-1)%8,BLACK);
					Lcd_pintar_ficha(fila,columna,BLACK);
					pintar_lcd=1;
				}
				//Dibujo parpadeo de ficha fila/columna
				if(/*get_tp_centro*/0){
					estado_main=t_cancelacion;
				}
				break;
			case t_cancelacion:
				//Espero 2 segundos
				if(timer_set_cancel==0){
					timer0_set(2,100);
					timer_set_cancel=1;
				}
				//Compruebo tp
				//Si han pasado los 2 segundos continuo
				if(timer0_get(2)==0){
					estado_main=jugada;
				}
				break;
			case jugada:
				t_calculos[0]=timer2_leer();
				reversi8_jugada(fila,columna,&done,&mov,&fin); //Jugada
				t_calculos[1]=timer2_leer();
				tiempo_calculos += t_calculos[1] - t_calculos[0];
				tiempo_patron_volteo = get_tiempo_patron_volteo();
				//Actualizar tiempos
				Lcd_tiempo_acumulado(tiempo_patron_volteo,tiempo_calculos,veces_patron_volteo);

				//Pintar nuevo trablero con la jugada
				if(fin==1){
					estado_main=fin_partida;
					break;
				}
				pintar_lcd=1;
				estado_main=eleccion_casilla;
				break;
		}
		if(pintar_lcd){
			pintar_lcd=0;
			Lcd_Dma_Trans();

		}
	}
}


