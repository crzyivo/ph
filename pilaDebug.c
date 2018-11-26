/*
 * pilaDebug.c
 */


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


	void push_debug(/*uint32_t */ unsigned int ID_evento, /*uint32_t*/  unsigned auxData){
		unsigned int timer;//=timer2_leer();
		unsigned int timeDef=0;

			*SPD=ID_evento;
			SPD=SPD-4;
			*SPD=auxData;
			SPD=SPD-4	;
			if (SPD < rINI) {SPD=rFIN;}

	}
