/*********************************************************************************************
* Fichero:		pilaDebug.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		gestion de la pila para debug
*********************************************************************************************/
	#include <stdint.h>

	//La pila almacena 100 eventos (IDevento,datos)
	volatile unsigned int * pila_fin=0xc7ff600;
	volatile unsigned int * pila_ini;
	volatile unsigned int *	cima;



#ifdef EMU
	//Cuenta el número de excepciones de cada tipo que se han
	// generado excErr[0]->DABT excErr[1]->UDEF excErr[2]->SWI
	//solo se declara si estamos en el simulador (indef)
	volatile unsigned int excErr[3];	// inicializar
#endif

	unsigned int time;

	void inicializar_pila(){
		pila_ini= 0xc7ff920;
		cima=pila_ini;
	}

	//0,1,2  --> excepciones
	//3 ->latido
	//4->boton
	void push_debug( unsigned int ID_evento, unsigned auxData){


		*cima=auxData;
		cima=cima-1;
		if(cima<pila_fin){
			cima=pila_ini;
		}
		*cima=ID_evento;
		cima=cima-1;
		if(cima<pila_fin){
			cima=pila_ini;
		}


	}
