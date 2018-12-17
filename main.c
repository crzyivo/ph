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

enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2,
FICHA_GRIS = 3
};



/*--- variables globales ---*/
unsigned int time;
	//Maquina de estados
	typedef enum{inicio,nueva_partida,eleccion_casilla,t_cancelacion,jugada,fin_partida}
	maquina_reversi;
 	//Estado actual de la maquina
	maquina_reversi estado_main=inicio;
	//LCD
	char yn;
	//Tablero
	char * tab[][8];
ULONG X_MIN_tp;
ULONG Y_MIN_tp;
ULONG X_MAX_tp;
ULONG Y_MAX_tp;
/*--- funciones externas ---*/

//extern void excepcion_dabt();
//extern void excepcion_swi();
//extern void excepcion_udef();

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


/*
 * Función ejecutada al principio del programa para calibrar la pantalla táctil en la parte del tablero
 */
void calibrar(){
	Lcd_Clr();				//Limpiamos la pantalla
	volatile ULONG minX = 1000;	//Valor mínimo de la X calibrada
	volatile ULONG minY = 1000;	//Valor mínimo de la Y calibrada
	volatile ULONG maxX = 1000;	//Valor máximo de la X calibrada
	volatile ULONG maxY = 1000;	//Valor máximo de la Y calibrada
	volatile ULONG tX=0;
	volatile ULONG tY=0;
	int i;
	for (i=0; i<5; i++){								//Hacemos 5 medidas
		Lcd_texto_calibracion("Superior Izquierda");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<minX){
			minX=tX;
		}
		if (tY<maxY){
			maxY=tY;
		}
		Lcd_texto_calibracion("Superior Derecha");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<minX){
			minX=tX;
		}
		if (tY<maxY){
			maxY=tY;
		}

		Lcd_texto_calibracion("Inferior Izquierda");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<minX){
			minX=tX;
		}
		if (tY<maxY){
			maxY=tY;
		}

		Lcd_texto_calibracion("Inferior Derecha");		//Indica la esquina a pulsar
		//Esperamos que se pulse la pantalla tactil
		while(!hayToque()){}
		setEspera_tp();
		getXY(&tX,&tY);
		if (tX<minX){
			minX=tX;
		}
		if (tY<maxY){
			maxY=tY;
		}
	}

	X_MIN_tp = minX;
	Y_MIN_tp = minY;
	X_MAX_tp = maxX;
	Y_MAX_tp = maxY;
}

int check_tp_centro(ULONG X, ULONG Y){
	return X>X_MIN_tp && Y>Y_MIN_tp && X< X_MAX_tp && Y<Y_MAX_tp;
}

void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	//timer_init();	    // Inicializacion del temporizador
	//button_iniciar();	// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led
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
	int pintar_una_vez=0; //Flag de cosas que solo se pintan una vez
	int pintar_lcd=0;
	int parpadeo_ficha=0;
	int timer_set_cancel=0;
	unsigned int tiempo_patron_volteo=0;
	unsigned int tiempo_calculos=0;
	unsigned int t_calculos[2];
	int veces_patron_volteo=0;
	int tiempo_juego=-1;
	int fila=0,filaAntigua;
	int columna=0,columnaAntigua;
	while(1){
		latido_check();
		antirebotes_check();
		//Miramos el tiempo total de juego
		if(timer0_get(3)==0 && tiempo_juego!=-1 && estado_main!=fin_partida){
			//Pintar tiempo nuevo
			tiempo_juego++;
			Lcd_tiempo_total(tiempo_juego);
			pintar_lcd=1;
			timer0_set(3,50);
		}
		switch(estado_main){
			case inicio:
				//Dibujar mensaje de entrada y esperar touchpad
				if(pintar_una_vez==0){
					Lcd_pantalla_inicio();
					pintar_lcd=1;
					pintar_una_vez++;
				}
				if(hayToque()){
					setEspera_tp();
					pintar_una_vez=0;
					estado_main=nueva_partida;
				}
				break;
			case nueva_partida:
				//Dibujar el trablero y el resto de la pantalla
				reversi8_init(); //Inicializa tableros
				get_tablero(tab);
				Lcd_dibujarTablero(tab);
				tiempo_juego=0;
				Lcd_tiempo_total(tiempo_juego);
				timer0_set(3,50);
				timer0_set(4,5); //Parpadeo de la ficha
				Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
				pintar_lcd=1;
				estado_main=eleccion_casilla;
				break;
			case eleccion_casilla:
				if(get_estado_boton()==button_iz){
					//Muevo en columnas
					columna=get_elegido();
					Lcd_mover_ficha(fila,(columna-1)%8,fila,columna,FICHA_NEGRA);
					pintar_lcd=1;
				}else if(get_estado_boton()==button_dr){
					//Muevo en filas
					fila=get_elegido();
					Lcd_mover_ficha((fila-1)%8,columna,fila,columna,FICHA_NEGRA);
					pintar_lcd=1;
				}else{
					//Dibujo parpadeo de ficha fila/columna
					if(timer0_get(4)==0 && parpadeo_ficha){
						//Lcd_pintar_ficha(fila,columna,WHITE);
						Lcd_limpiar_casilla(fila,columna);
						timer0_set(4,5);
						parpadeo_ficha=0;
						pintar_lcd=1;
					}else if(timer0_get(4)==0){
						Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
						timer0_set(4,5);
						parpadeo_ficha=1;
						pintar_lcd=1;
					}

				}

				if(hayToque()){
					ULONG x_toque,y_toque;
					getXY(&x_toque,&y_toque);
					if(check_tp_centro(x_toque,y_toque)){
						estado_main=t_cancelacion;
					}
					setEspera_tp();
				}
				break;
			case t_cancelacion:
				//Espero 2 segundos
				if(timer_set_cancel==0){
					timer0_set(2,100);
					timer_set_cancel=1;
					Lcd_texto_cancelar();
					pintar_lcd=1;
				}
				//Compruebo tp
				if(hayToque()){
					setEspera_tp();
					estado_main=eleccion_casilla;
				}
				//Si han pasado los 2 segundos continuo
				else if(timer0_get(2)==0){
					estado_main=jugada;
				}
				break;
			case jugada:
				t_calculos[0]=timer2_leer();
				reversi8_jugada(fila,columna,&done,&mov,&fin); //Jugada
				t_calculos[1]=timer2_leer();
				tiempo_calculos += t_calculos[1] - t_calculos[0];
				tiempo_patron_volteo = get_tiempo_patron_volteo();
				veces_patron_volteo = get_veces_patron_volteo();
				//Actualizar tiempos
				Lcd_tiempo_acumulado(tiempo_patron_volteo,tiempo_calculos,veces_patron_volteo);
				char* tablero_post_jugada;
				get_tablero(tablero_post_jugada);
				//TODO: Pintar nuevo trablero con la jugada
				Lcd_dibujarTablero(tablero_post_jugada);

				if(fin==1){
					estado_main=fin_partida;
					break;
				}
				pintar_lcd=1;
				estado_main=eleccion_casilla;
				break;
			case fin_partida:
				if(pintar_una_vez==0){
					Lcd_Clr();
					char *tablero_fin;
					get_tablero(tablero_fin);
					Lcd_dibujarTablero(tablero_fin);
					Lcd_tiempo_total(tiempo_juego);
					Lcd_tiempo_acumulado(tiempo_patron_volteo,tiempo_calculos,veces_patron_volteo);
					Lcd_texto_fin();
					pintar_lcd=1;
				}
				if(hayToque()){
					setEspera_tp();
					fin=0;
					done=0;
					mov=0;
					pintar_una_vez=0;
					fila=0;
					columna=0;
					tiempo_juego=-1;
					tiempo_patron_volteo=0;
					tiempo_calculos=0;
					veces_patron_volteo=0;
					estado_main=inicio;
				}

		}
		if(pintar_lcd){
			pintar_lcd=0;
			Lcd_Dma_Trans();

		}
	}
}


