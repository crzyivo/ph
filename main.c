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
	ULONG tX=0;
	ULONG tY=0;
	int i;
	for (i=0; i<5; i++){								//Hacemos 5 medidas
		Lcd_texto_calibracion("Superior Izquierda");		//Indica la esquina a pulsar
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
		Lcd_texto_calibracion("Superior Derecha");		//Indica la esquina a pulsar
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
	}

	X_MIN_tp = minX;
	Y_MIN_tp = minY;
	X_MAX_tp = maxX;
	Y_MAX_tp = maxY;
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
	int palabra;
	asm("MRS %0 ,CPSR" : "=r"(palabra) );
	palabra= (palabra & 0xffffff00)|0x10; //Modo usuario
	asm("MSR CPSR_cxsf,%0" : : "r"(palabra));

	//Entramos en el bucle principal de juego
	reversi_main();

}

void reversi_main(){

	//Variables usadas en reversi8_jugada
	int done=0;
	int mov=0;
	int fin=0;

	int pintar_una_vez=0; //Flag de cosas que solo se pintan una vez
	int pintar_lcd=0; //Flag para realizar Dma_Trans
	int parpadeo_ficha=0; //Flag para realizar el parpadeo de ficha gris
	int timer_set_cancel=0; //Flag para setear el tiempo de cancelacion una sola vez
	unsigned int tiempo_patron_volteo=0; //Tiempo acumulado en las llamadas a patron_volteo
	unsigned int tiempo_calculos=0; //Tiempo acumulado en las llamadas a reversi8_jugada
	unsigned int t_calculos[2]; //Variables para medir tiempos de timer2
	int veces_patron_volteo=0; //Numero de llamadas a patron_volteo, incluyendo llamadas recursivas
	int tiempo_juego=-1; //Tiempo total de juego, si -1 se desactiva

	int fila=0; //Fila de ficha elegida
	int columna=0; //Columna de ficha elegida

	//Comienzo del bucle de juego
	while(1){
		//Compruebo interrupcion de latido
		latido_check();

		//Compruebo si se ha tocado un boton
		antirebotes_check();

		//Miramos el tiempo total de juego
		if(timer0_get(3)==0 && tiempo_juego!=-1 && estado_main!=fin_partida){
			//Pintar tiempo nuevo
			tiempo_juego++;
			Lcd_tiempo_total(tiempo_juego);
			pintar_lcd=1;
			timer0_set(3,50);
		}
		//Miramos el estado de la maquina
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
				Lcd_dibujarTablero(tab); //Dibujo el tablero

				//Activo la cuenta del tiempo total de juego
				tiempo_juego=0;
				Lcd_tiempo_total(tiempo_juego);
				timer0_set(3,50);
				//Parpadeo de la ficha
				timer0_set(4,5);
				Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
				pintar_lcd=1;
				estado_main=eleccion_casilla;
				break;
			case eleccion_casilla:
				//Miro el tablero para repintar
				get_tablero(tab);
				if(get_estado_boton()==button_iz){
					//Muevo en columnas
					columna=get_elegido();
					Lcd_mover_ficha(tab,fila,columna,FICHA_NEGRA);
					pintar_lcd=1;
				}else if(get_estado_boton()==button_dr){
					//Muevo en filas
					fila=get_elegido();
					Lcd_mover_ficha(tab,fila,columna,FICHA_NEGRA);
					pintar_lcd=1;
				}
				//Dibujo parpadeo de ficha fila/columna
				if(timer0_get(4)==0 && parpadeo_ficha){
					//Si estoy encima de una ficha existente dibujo esa ficha
					if(tab[fila][columna]==FICHA_NEGRA){
						Lcd_pintar_ficha(fila,columna,FICHA_NEGRA);
					}else if(tab[fila][columna]==FICHA_BLANCA){
						Lcd_pintar_ficha(fila,columna,FICHA_BLANCA);
					}else{ //O el hueco vacio
						Lcd_limpiar_casilla(fila,columna);
					}
					timer0_set(4,5);
					parpadeo_ficha=0;
					pintar_lcd=1;
				}else if(timer0_get(4)==0){
					//Si no dibujo la ficha en gris
					Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
					timer0_set(4,5);
					parpadeo_ficha=1;
					pintar_lcd=1;

				}
			//Compruebo tp
			if(hayToque()){
				ULONG x_toque,y_toque;
				getXY(&x_toque,&y_toque);
				//Compruebo que se toca en el centro
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
				//Dibujo parpadeo de ficha fila/columna
				if(timer0_get(4)==0 && parpadeo_ficha){

					if(tab[fila][columna]==FICHA_NEGRA){
						Lcd_pintar_ficha(fila,columna,FICHA_NEGRA);
					}else if(tab[fila][columna]==FICHA_BLANCA){
						Lcd_pintar_ficha(fila,columna,FICHA_BLANCA);
					}else{
						Lcd_limpiar_casilla(fila,columna);
					}
					timer0_set(4,5);
					parpadeo_ficha=0;
					pintar_lcd=1;
				}else if(timer0_get(4)==0){
					Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
					timer0_set(4,5);
					parpadeo_ficha=1;
					pintar_lcd=1;

				}
				//Compruebo tp
				if(hayToque()){
					setEspera_tp();
					estado_main=eleccion_casilla;
				}
				//Compruebo botones
				else if(get_estado_boton()==button_iz || get_estado_boton()==button_dr){
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
				//Calculo del tiempo acumulado en reversi8_jugada
				tiempo_calculos += t_calculos[1] - t_calculos[0];
				//Obtengo el tiempo acumulado en patron_volteo de reversi8
				tiempo_patron_volteo = get_tiempo_patron_volteo();
				//Obtengo el numero de llamadas de patron_volteo de reversi8
				veces_patron_volteo = get_veces_patron_volteo();
				//Actualizar tiempos y profiling
				Lcd_tiempo_acumulado(tiempo_patron_volteo,tiempo_calculos,veces_patron_volteo);
				//Redibujo el tablero con la jugada de la maquina y la del humano
				get_tablero(tab);
				Lcd_dibujarTablero(tab);
				pintar_lcd=1;
				//Si el tablero esta acabado termino
				if(fin==1){
					estado_main=fin_partida;
				}else{
					estado_main=eleccion_casilla;
				}
				break;
			case fin_partida:
				if(pintar_una_vez==0){
					Lcd_Clr();
					get_tablero(tab);
					Lcd_dibujarTablero(tab);
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


