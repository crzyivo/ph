/*********************************************************************************************
* Fichero:		botones_antirebotes.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Funciones que usan button.c y latido.c para eliminar los rebotes de botones
*********************************************************************************************/

#include "botones_antirebotes.h"
#include "button.h"
#include "timer0.h"
#include "8led.h"

//Valor que se muestra en el 8led
static unsigned int int_count_iz = 0;
static unsigned int int_count_dr = 0;

//Estados de la maquina
typedef enum{sin_pulsar,ret_inicio,monitorizacion,ret_salida}
estado_botones_antirebotes;

//Retardos
int trp = 5;
int trd = 15;

//Boton pulsado
estado_button boton_pulsado_antirebotes = button_none;

//Estado actual en el que se encuentra la maquina
estado_botones_antirebotes maquina = sin_pulsar;

//Valor que se fija al pulsar el boton derecho
int elegido=-1;

//Valores que indican si se han cumplido los retardos
int trp_realizado=0;
int trd_realizado=0;
#ifdef EMU
	volatile char num=0,ready=0;
#endif
void callback_antirebotes(estado_button);
void incrementa();
// Espera a que ready valga 1.
// CUIDADO: si el compilador coloca esta variable en un registro, no funcionará.
// Hay que definirla como "volatile" para forzar a que antes de cada uso la cargue de memoria

void esperar_num(volatile char *r){
    while (*r == 0) {};  // bucle de espera de respuestas hasta que el se modifique el valor de ready (hay que hacerlo manualmente)

    *r = 0;  //una vez que pasemos el bucle volvemos a fijar ready a 0;
}

void callback_antirebotes(estado_button e){
	boton_pulsado_antirebotes = e;
	estado_botones_antirebotes aux = maquina;
	 switch(maquina){
	 case sin_pulsar: //Se ha pulsado un boton por primera vez
		 trp_realizado=0;
		 trd_realizado=0;
		 elegido=-1;
		 timer0_set(1,trp); //Esperamos un retardo
		 maquina = ret_inicio;
		 break;
	 }
}

void antirebotes_check(){
	switch(maquina){
	case ret_inicio:
		if(timer0_get(1)==0){
			trp_realizado=1;
			maquina=monitorizacion;
		}
		break;
	case monitorizacion:
		if(button_estado() == button_none){ //He levantado
			maquina = ret_salida;
			timer0_set(1,trd); //Realizo la logica necesaria y vuelvo a esperar en timer
		}
		break;
	case ret_salida:
		if(timer0_get(1)==0){
			 trd_realizado=1;
			 incrementa();
			 maquina = sin_pulsar;
			 button_empezar(callback_antirebotes);
		}
		break;
	}
}

void inicio_antirebotes(){
	button_iniciar();
	button_empezar(callback_antirebotes);
}

void incrementa(){
	switch (boton_pulsado_antirebotes)
		{
			case button_iz:
				D8Led_symbol(12);
				int_count_iz=(int_count_iz+1)%8; // incrementamos el contador
				elegido=int_count_iz;
				D8Led_symbol(int_count_iz & 0x000f); // sacamos el valor por pantalla (módulo 16)
				break;
			case button_dr:
				D8Led_symbol(15);
				int_count_dr=(int_count_dr+1)%8; // incrementamos el contador
				elegido=int_count_dr;
				D8Led_symbol(int_count_dr & 0x000f); // sacamos el valor por pantalla (módulo 16)
				break;
			default:
				break;
		}

}

int get_elegido(){
#ifndef EMU
	if(trp_realizado && trd_realizado){
		return elegido;
	}else{
		return -1;
	}
#else
	int dir = &num;
	int dir_r = &ready;
	esperar_num(&ready);
	ready=0;
	return num;
#endif
}

int get_estado_boton(){
#ifndef EMU
	return boton_pulsado_antirebotes;
#else
	return button_iz;
#endif
}
