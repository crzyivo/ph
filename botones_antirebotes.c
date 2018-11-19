/*
 * botones_antirebotes.c
 *
 *  Created on: 06/11/2018
 *      Author: Ivo
 */

#include "botones_antirebotes.h"
#include "button.h"
#include "latido.h"
#include "8led.h"

/* int_count la utilizamos para sacar un número por el 8led.
  Cuando se pulsa un botón sumamos y con el otro restamos. ¡A veces hay rebotes! */
static unsigned int int_count = 0;
//Estados de la maquina
typedef enum{sin_pulsar,ret_inicio,monitorizacion,ret_salida}
estado_botones_antirebotes;

int trp = 5;
int trd = 5;
estado_button boton_pulsado_antirebotes = button_none;
estado_botones_antirebotes maquina = sin_pulsar;

int trp_realizado=0;
int trd_realizado=0;
void callback_antirebotes(estado_button);
void callback_espera();
void incrementa();

void callback_antirebotes(estado_button e){
	boton_pulsado_antirebotes = e;
	estado_botones_antirebotes aux = maquina;
	 switch(maquina){
	 case sin_pulsar: //Se ha pulsado un boton por primera vez
		 espera_ticks(trp,callback_espera); //Esperamos un retardo
		 maquina = ret_inicio;
		 break;
	 case ret_salida:
		 incrementa();
		 espera_ticks(trd,callback_espera);
		 break;
	 }
}

void callback_espera(){
	switch(maquina){
	case ret_inicio:
		trp_realizado=1;
		maquina=monitorizacion;
		espera_ticks(1,callback_espera);
		break;
	case monitorizacion:
		if(button_estado() == button_none){ //He levantado
			maquina = ret_salida;
			callback_antirebotes(boton_pulsado_antirebotes); //Realizo la logica necesaria y vuelvo a esperar en timer
		}else{
			espera_ticks(1,callback_espera);
		}
		break;
	case ret_salida:
		maquina = sin_pulsar;
		button_empezar(callback_antirebotes);
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
				int_count++; // incrementamos el contador
				break;
			case button_dr:
				int_count--; // decrementamos el contador
				break;
			default:
				break;
		}
		D8Led_symbol(int_count & 0x000f); // sacamos el valor por pantalla (módulo 16)
}
