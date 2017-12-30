/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Proyecto: EL MUNDO DE WUMPUS									  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** Autores: Sebastian Caballa y Cristhian Llamocca 				  *****/
/***** Fecha: Noviembre 2017 										  *****/
/**************************************************************************/
/**************************************************************************/

//**********	 LIBRERÍAS	 **********//
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "JUEGO.h"
#include <stdlib.h>
#include <time.h>
#include "EEPROM.h"
#include "UART.h"
#include "I2C.h"
#include "RTC.h"
//**********	 CONSTANTES	   **********//
#define F 0
#define V 1

#define MODO_PRUEBA 0	//1: Mapa On //0: Mapa Off

uint8_t NCAgujero2;
uint8_t NCWumpus = 2;
uint8_t NCTesoro = 2;
uint8_t NCAgujero = 2;

//**********	MENSAJES	**********//
const uint8_t MensajeMejores[] = "Los mejores tiempos son: ";
const uint8_t MensajeNuevoRecord[] = "Felicidades! Hay un nuevo record :)";
const uint8_t MensajeSolicitaNickName[] = "Ingrese Nick Name: ";
uint8_t nicknameInicio[] = {'-','-','-','-','-','-','-',0};

//**********ARREGLOS GLOBLALES**********//
	//**Blocks para EEPROM**//
const uint16_t Block[15] = {
	/*MenorTiempo1*/ 	0x00,
	/*MenorTiempo2*/ 	0x01,
	/*MenorTiempo3*/ 	0x02,
	/*MenorTiempo4*/ 	0x03,
	/*MenorTiempo5*/ 	0x04,
	/*NickName1*/	  	0x05,
	/*NickName2*/ 		0x06,
	/*NickName3*/ 		0x07,
	/*NickName4*/ 		0x08,
	/*NickName5*/    	0x09,
	/*Fecha1*/    		0x0A,
	/*Fecha2*/    		0x0B,
	/*Fecha3*/    		0x0C,
	/*Fecha4*/    		0x0D,
	/*Fecha5*/    		0x0E,
};
	//**Offsets para Bloques**//
const uint8_t Offset[16] = {
	/*Offset0*/ 	0x0,
	/*Offset1*/ 	0x1,
	/*Offset2*/ 	0x2,
	/*Offset3*/ 	0x3,
	/*Offset4*/ 	0x4,
	/*Offset5*/   	0x5,
	/*Offset6*/   	0x6,
	/*Offset7*/   	0x7,
	/*Offset8*/   	0x8,
	/*Offset9*/   	0x9,
	/*Offset10*/	0xA,
	/*Offset11*/	0xB,
	/*Offset12*/	0xC,
	/*Offset13*/	0xD,
	/*Offset14*/	0xE,
	/*Offset15*/	0xF
};

//**********	DIRECTIVAS	**********//
uint8_t N_Casillero_Aventurero(uint8_t arreglo[]);
void MUESTRA_MUNDO_WUMPUS(uint8_t arreglo[]);
void Actualiza_hedor(uint8_t NCWumpus, uint8_t arreglo[]);
void Actualiza_brisa(uint8_t NCAgujero, uint8_t arreglo[]);
void ActualizaTesoroWumpusAgujeroHedorBrisa(uint8_t NCTesoro, uint8_t NCWumpus, uint8_t NCAgujero, uint8_t arreglo[]);
void GeneraElementosJuego(uint8_t arreglo[], uint8_t NCelementos[]);
void GENERAR_MOSTRAR_ELEMENTOS(uint8_t arreglo[], uint8_t NCelementos[] );
void DisparaFlecha(uint8_t Posicion, uint8_t NCelementos[], uint8_t arreglo[]);
void ESPERA_COMANDO_ACTUALIZAR_ARREGLO(uint8_t arreglo[], uint8_t NCelementos[]);
uint8_t NumAleatorio1a4(void);
void Move_Wumpus_ActualizaArreglo(uint8_t NCelementos[], uint8_t arreglo[]);
uint8_t Analisis_Tesoro_CasillaActual(uint8_t arreglito[]);
uint8_t Evaluar_Reglas(uint8_t arreglo[]);
uint8_t Evaluar_Ganar(uint8_t tieneTesoro, uint8_t arreglo[]);
void Mostrar_Percepciones_CasillaAventurero(uint8_t arreglo[]);
void INTERFAZ_INICIO_JUEGO(void);
void JUEGO(void);
void ObtenerRecords(void);
void ObtenerMenoresTiempos(void);
void ObtenerNickNames(void);
void ObtenerFechas(void);
void VisualizaMenoresTiempos(void);
void InicializaRecord(void);;
void GuardarNuevoRecord(uint16_t tiempo, uint8_t nicknameZ[], uint8_t numeroRecord);
uint8_t NumeroRecord(uint16_t);
void AnalisisDePartida(uint16_t tiempo);

//**********	ARREGLOS	**********//
uint8_t MenoresTiempos[5][8];
uint8_t NickNames[5][8];
uint8_t Fechas[5][10];

uint8_t nicknameZ1[]= {'S','e','b','a','s',0};
uint8_t nicknameZ2[]= {'X','i','o','m','i',0};
uint8_t nicknameZ3[]= {'N','i','e','v','e',0};
uint8_t nicknameZ4[]= {'J','a','n','g','o',0};
uint8_t nicknameZ5[]= {'R','o','n','a','l',0};

uint8_t ArregloFecha[10];
uint8_t ArregloHora[10];



//****PROGRAMA PRINCIPAL****//
void main(void) {
	//**Inicializacion**//
	RTC_inicializacion();
	I2C_inicialization();
	UART_inicializacion();
	EEPROM_inicializacion();
	//**Configuración de hora del módulo RTC**//
	//SetTimeDate(30,32,8,4,5,11,14);
	while(1){
		/*
		//Inicialización con nicknames previos en la fecha actual
		GuardarNuevoRecord(012,  nicknameZ1, 1);
		GuardarNuevoRecord(100,  nicknameZ2, 2);
		GuardarNuevoRecord(200,  nicknameZ3, 3);
		GuardarNuevoRecord(300,  nicknameZ4, 4);
		GuardarNuevoRecord(400,  nicknameZ5, 5);
		*/
		//Limpieza de pantalla
		UART_TransmiteByte('\f');
		//Se muestra interfaz de juego inicial (manual, juego y records)
		INTERFAZ_INICIO_JUEGO();
		//Jugada
		JUEGO();
	}
}

/*
	//EEPROM//
		//Si es la primera jugada, se puede inicializar el arreglo a cero en todos los mejores records
		//InicializaRecord();
		GuardarNuevoRecord(012,  nicknameZ1, 1);
		GuardarNuevoRecord(100,  nicknameZ2, 2);
		GuardarNuevoRecord(200,  nicknameZ3, 3);
		GuardarNuevoRecord(300,  nicknameZ4, 4);
		GuardarNuevoRecord(400,  nicknameZ5, 5);
		ObtenerRecords();
		VisualizaMenoresTiempos();
		//Se analiza y graba (si es necesario)
		AnalisisDePartida(nuevoTiempo);
		VisualizaMenoresTiempos();
		//FIN//
		//RTC//
		ObtenerHoraActual(ArregloHora);
		UART_TransmiteCadena(ArregloHora);
		UART_SiguienteLinea();
		ObtenerFechaActual(ArregloFecha);
		UART_TransmiteCadena(ArregloFecha);
		UART_SiguienteLinea();UART_SiguienteLinea();
		i = ObtenerHoraActualSegundos();
		for(i=0;i<2500000;i++);
	//FIN//
*/

//********			FUNCIONES			********//
/**************************************************************************/
/***** 			 		N_Casillero_Aventurero() 			 		  *****/
/**************************************************************************/
/***** Esta función detecta la ubicación del casillero en el que se	  *****/
/***** encutra el jugador. 											  *****/
/**************************************************************************/
uint8_t N_Casillero_Aventurero(uint8_t arreglo[]){
	uint8_t i;	//Número de casilla
	uint8_t Aventurero;
	uint8_t pos = 20;
	for(i=0;i<16;i++){
		Aventurero = ( (arreglo[i] & (1<<5)) >>5 ) ;
		if(Aventurero == 1){
			pos = i;
		}
	}
	return pos;
}

/**************************************************************************/
/***** 			 		MUESTRA_MUNDO_WUMPUS() 			  		 	  *****/
/**************************************************************************/
/***** La función genera la cuadrícula de juego y ubica los elementos.*****/
/**************************************************************************/
void MUESTRA_MUNDO_WUMPUS(uint8_t arreglo[]){
	GENERA_CUADRICULA();
	MUESTRA_ELEMENTOS_TABLERO(arreglo);
	UART_SiguienteLinea();
}

/**************************************************************************/
/***** 			 			Actualiza_hedor() 			 			  *****/
/**************************************************************************/
/***** Se actualiza las casillas en las que se percibe el hedor según *****/
/***** la posición del Wumpus. 										  *****/
/**************************************************************************/
void Actualiza_hedor(uint8_t NCWumpus,uint8_t arreglo[]){
	if( (NCWumpus==4) || (NCWumpus==8)){
		arreglo[NCWumpus+4] |= (1<<2);
		arreglo[NCWumpus-4] |= (1<<2);
		arreglo[NCWumpus+1] |= (1<<2);
	}
	else if(NCWumpus == 12){
		arreglo[NCWumpus-4] |= (1<<2);
		arreglo[NCWumpus+1] |= (1<<2);
	}
	else if(NCWumpus == 15){
		arreglo[NCWumpus-4] |= (1<<2);
		arreglo[NCWumpus-1] |= (1<<2);
	}
	else if( (NCWumpus==7) || (NCWumpus==11) ){
		arreglo[NCWumpus+4] |= (1<<2);
		arreglo[NCWumpus-4] |= (1<<2);
		arreglo[NCWumpus-1] |= (1<<2);
	}
	else if( (NCWumpus==13) || (NCWumpus==14)){
		arreglo[NCWumpus-1] |= (1<<2);
		arreglo[NCWumpus-4] |= (1<<2);
		arreglo[NCWumpus+1] |= (1<<2);
	}
	else if( (NCWumpus==5)||(NCWumpus==6)||(NCWumpus==9)||(NCWumpus==10) ){
		arreglo[NCWumpus+4] |= (1<<2);
		arreglo[NCWumpus-4] |= (1<<2);
		arreglo[NCWumpus+1] |= (1<<2);
		arreglo[NCWumpus-1] |= (1<<2);
	}
	else if( (NCWumpus==1) || (NCWumpus==2)){
		arreglo[NCWumpus-1] |= (1<<2);
		arreglo[NCWumpus+4] |= (1<<2);
		arreglo[NCWumpus+1] |= (1<<2);
	}
	else if(NCWumpus == 3){
		arreglo[NCWumpus-1] |= (1<<2);
		arreglo[NCWumpus+4] |= (1<<2);
	}
	else if(NCWumpus == 0){
		arreglo[NCWumpus+4] |= (1<<2);
		arreglo[NCWumpus+1] |= (1<<2);
	}
}

/**************************************************************************/
/***** 			 			Actualiza_brisa() 			 			  *****/
/**************************************************************************/
/***** Se actualiza las casillas en las que se percibe el brisa según *****/
/***** la posición de los agujeros. 								  *****/
/**************************************************************************/
void Actualiza_brisa(uint8_t NCAgujero,uint8_t arreglo[]){
	if(NCAgujero == 8){
		arreglo[NCAgujero+4] |= (1<<0);
		arreglo[NCAgujero-4] |= (1<<0);
		arreglo[NCAgujero+1] |= (1<<0);
	}
	else if(NCAgujero == 12){
		arreglo[NCAgujero-4] |= (1<<0);
		arreglo[NCAgujero+1] |= (1<<0);
	}
	else if(NCAgujero == 15){
		arreglo[NCAgujero-4] |= (1<<0);
		arreglo[NCAgujero-1] |= (1<<0);
	}
	else if( (NCAgujero==7) || (NCAgujero==11) ){
		arreglo[NCAgujero+4] |= (1<<0);
		arreglo[NCAgujero-4] |= (1<<0);
		arreglo[NCAgujero-1] |= (1<<0);
	}
	else if( (NCAgujero==13) || (NCAgujero==14)){
		arreglo[NCAgujero-1] |= (1<<0);
		arreglo[NCAgujero-4] |= (1<<0);
		arreglo[NCAgujero+1] |= (1<<0);
	}
	else{
		arreglo[NCAgujero+4] |= (1<<0);
		arreglo[NCAgujero-4] |= (1<<0);
		arreglo[NCAgujero+1] |= (1<<0);
		arreglo[NCAgujero-1] |= (1<<0);
	}
}

/**************************************************************************/
/***** 			ActualizaTesoroWumpusAgujeroHedorBrisa()  			  *****/
/**************************************************************************/
/***** Se actualizan las posiciones del tesoro, el Wumpus, agujeros   *****/
/***** el hedor y la brisa.											  *****/
/**************************************************************************/
void ActualizaTesoroWumpusAgujeroHedorBrisa(uint8_t NCTesoro,uint8_t NCWumpus,uint8_t NCAgujero,uint8_t arreglo[]){
	arreglo[NCTesoro] |= (1<<4);
	arreglo[NCWumpus] |= (1<<3);
	arreglo[NCAgujero] |= (1<<1);
	arreglo[0] |= (1<<5);

	Actualiza_hedor(NCWumpus,arreglo);
	Actualiza_brisa(NCAgujero,arreglo);
}

/**************************************************************************/
/***** 						GeneraElementosJuego()  				  *****/
/**************************************************************************/
/***** Posiciona de forma aleatoria los elementos del juego en mapa.  *****/
/**************************************************************************/
void GeneraElementosJuego(uint8_t arreglo[],uint8_t NCelementos[]){
	uint8_t i;
	uint8_t NCTesoro,NCWumpus,NCAgujero,NCAgujero2,NCAventurero;

	NCAventurero = 0;
	NCTesoro = rand() % 10 + 1 + 5;
	NCWumpus = rand() % 10 + 1 + 5;
	NCAgujero = rand() % 10 + 1 + 5;
	NCAgujero2 = rand() % 10 + 1 + 5;

	for(i=0;i<2;i++){
		if(i < 1){
			while(NCTesoro == NCWumpus){
				NCWumpus = rand() % 10 + 1 + 5;
			}
			while(NCTesoro == NCAgujero){
			NCAgujero = rand() % 10 + 1 + 5;
			}
			ActualizaTesoroWumpusAgujeroHedorBrisa(NCTesoro,NCWumpus,NCAgujero,arreglo);
		}
		else{
			while( (NCAgujero2 == NCTesoro) || (NCAgujero2 == NCAgujero) ){
				NCAgujero2 = rand() % 10 + 1 + 5;
			}
			ActualizaTesoroWumpusAgujeroHedorBrisa(NCTesoro,NCWumpus,NCAgujero2,arreglo);
		}
	}
	NCelementos[0]= NCAventurero  ;
	NCelementos[1]= NCWumpus  ;
	NCelementos[2]= NCTesoro  ;
	NCelementos[3]= NCAgujero  ;
	NCelementos[4]= NCAgujero2  ;

}

/**************************************************************************/
/***** 					GENERAR_MOSTRAR_ELEMENTOS()  				  *****/
/**************************************************************************/
/***** Función genera los elementos del juego y los muestra, si se    *****/
/***** está en el modo de prueba del juego.							  *****/
/**************************************************************************/
void GENERAR_MOSTRAR_ELEMENTOS(uint8_t arreglo[],uint8_t NCelementos[] ){
	GeneraElementosJuego(arreglo,NCelementos);
	if(MODO_PRUEBA == 1){
		MUESTRA_MUNDO_WUMPUS(arreglo);
	}
}

/**************************************************************************/
/***** 							DisparaFlecha()  					  *****/
/**************************************************************************/
/***** Espera el comando para lanza flecha (t) en la dirección sele_  *****/
/***** ccionada. Si la flecha cayó en el Wumpus, murió y ya no se	  *****/
/***** mostrará en el juego.	 									  *****/
/**************************************************************************/
void DisparaFlecha(uint8_t Posicion,uint8_t NCelementos[],uint8_t arreglo[]){
	uint8_t i;
	uint8_t muertoW, Direccion, CFlecha1, CFlecha2, CFlecha3, NCWumpus;
	muertoW = F;
	NCWumpus = NCelementos[1];
	NCelementos[5] = 1;

	/*do{
		UART_TransmiteCadena("Eliga una direccion correcta\n\r");
		Direccion = UART_EsperaByte();
	}while(  !( (Direccion=='w')||(Direccion=='s')||(Direccion=='a')||(Direccion=='d') ) );*/

	Direccion = UART_EsperaByte();
	while(  !( (Direccion=='w')||(Direccion=='s')||(Direccion=='a')||(Direccion=='d') ) ){
		UART_TransmiteCadena("Eliga una direccion correcta\n\r");
		Direccion = UART_EsperaByte();
	}
		if(Direccion == 'w'){
			if( (Posicion==0)||(Posicion==1)||(Posicion==2)||(Posicion==3) ){
				CFlecha1 = Posicion + 4;
				CFlecha2 = Posicion + 8;
				CFlecha3 = Posicion + 12;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2)||(NCWumpus==CFlecha3) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==0)||(Posicion==1)||(Posicion==2)||(Posicion==3) ){
				CFlecha1 = Posicion + 4;
				CFlecha2 = Posicion + 8;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==0)||(Posicion==1)||(Posicion==2)||(Posicion==3) ){
				CFlecha1 = Posicion + 4;
				if( NCWumpus == CFlecha1 ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==0)||(Posicion==1)||(Posicion==2)||(Posicion==3) ){
			}
		}
		else if(Direccion == 's'){
			if( (Posicion==12)||(Posicion==13)||(Posicion==14)||(Posicion==15) ){
				CFlecha1 = Posicion - 4;
				CFlecha2 = Posicion - 8;
				CFlecha3 = Posicion - 12;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2)||(NCWumpus==CFlecha3) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==8)||(Posicion==9)||(Posicion==10)||(Posicion==11) ){
				CFlecha1 = Posicion - 4;
				CFlecha2 = Posicion - 8;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==4)||(Posicion==5)||(Posicion==6)||(Posicion==7) ){
				CFlecha1 = Posicion -4;
				if( NCWumpus == CFlecha1 ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==0)||(Posicion==1)||(Posicion==2)||(Posicion==3) ){
			}
		}
		else if(Direccion == 'a'){
			if( (Posicion==3)||(Posicion==7)||(Posicion==11)||(Posicion==15) ){
				CFlecha1 = Posicion - 1;
				CFlecha2 = Posicion - 2;
				CFlecha3 = Posicion - 3;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2)||(NCWumpus==CFlecha3) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==2)||(Posicion==4)||(Posicion==8)||(Posicion==12) ){
				CFlecha1 = Posicion - 1;
				CFlecha2 = Posicion - 2;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==1)||(Posicion==5)||(Posicion==9)||(Posicion==13) ){
				CFlecha1 = Posicion - 1;
				if( NCWumpus == CFlecha1 ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==0)||(Posicion==4)||(Posicion==8)||(Posicion==12) ){
			}
		}
		else if(Direccion == 'd'){
			if( (Posicion==0)||(Posicion==4)||(Posicion==8)||(Posicion==12) ){
				CFlecha1 = Posicion +1;
				CFlecha2 = Posicion +2;
				CFlecha3 = Posicion +3;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2)||(NCWumpus==CFlecha3) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1]=16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==1)||(Posicion==5)||(Posicion==9)||(Posicion==13) ){
				CFlecha1 = Posicion + 1;
				CFlecha2 = Posicion + 2;
				if( (NCWumpus==CFlecha1)||(NCWumpus==CFlecha2) ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==2)||(Posicion==6)||(Posicion==10)||(Posicion==14) ){
				CFlecha1 = Posicion + 1;
				if( NCWumpus==CFlecha1 ){
					muertoW = V;
					UART_TransmiteCadena("El Wumpus ha muerto\n\r");
					UART_SiguienteLinea();
					UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
					uint8_t datoRx = UART_EsperaByte();
					NCelementos[1] = 16;
					for(i=0;i<16;i++){
						arreglo[i] &= ~(1<<3);
					}
				}
			}
			else if( (Posicion==3)||(Posicion==7)||(Posicion==11)||(Posicion==15) ){
			}
		}
	if(muertoW == F){
		UART_TransmiteCadena("El Wumpus no ha muerto\n\r");
		UART_SiguienteLinea();
		UART_TransmiteCadena("	Presione cualquier tecla para continuar"); UART_SiguienteLinea();
		uint8_t datoRx = UART_EsperaByte();
	}

}

/**************************************************************************/
/***** 				ESPERA_COMANDO_ACTUALIZAR_ARREGLO() 	 		  *****/
/**************************************************************************/
/***** La función espera la introducción de un comando (w, a, s, d, f *****/
/***** ) para actualizar las posicones de los elementos del juego.    *****/
/**************************************************************************/
void ESPERA_COMANDO_ACTUALIZAR_ARREGLO(uint8_t arreglo[], uint8_t NCelementos[]){
	uint8_t Posicion;
	uint8_t Comando;

	Posicion = N_Casillero_Aventurero(arreglo);

	do{
	Comando = UART_EsperaByte();
	if(Comando == 'w'){
		if( !( (Posicion==12)||(Posicion==13)||(Posicion==14)||(Posicion==15) ) ){
			arreglo[Posicion] &= ~0x20;
			Posicion = Posicion + 4;
			arreglo[Posicion] |= 0x20;
		}
	}
	else if(Comando == 's'){
		if( !( (Posicion==0)||(Posicion==1)||(Posicion==2)||(Posicion==3) ) ){
			arreglo[Posicion] &= ~0x20;
			Posicion = Posicion - 4;
			arreglo[Posicion] |= 0x20;
		}
	}
	else if(Comando == 'a'){
		if( !( (Posicion==0)||(Posicion==4)||(Posicion==8)||(Posicion==12) ) ){
			arreglo[Posicion] &= ~0x20;
			Posicion = Posicion -1;
			arreglo[Posicion] |= 0x20;
		}
	}
	else if(Comando == 'd'){
		if( !( (Posicion==3)||(Posicion==7)||(Posicion==11)||(Posicion==15) ) ){
			arreglo[Posicion] &= ~0x20;
			Posicion = Posicion + 1;
			arreglo[Posicion] |= 0x20;
		}
	}
	else if(Comando == 'f'){
		if(NCelementos[5] == 0){
			UART_TransmiteCadena("Indique direccion de flecha\n\r");
			DisparaFlecha(Posicion,NCelementos,arreglo);
		}
		else{
			UART_TransmiteCadena("Flecha ya usada\n\r");
			Comando = 'q';
		}
	}
	}while( !((Comando=='w')||(Comando=='s')||(Comando=='a')||(Comando=='d')||(Comando=='f') ) );
}

/**************************************************************************/
/***** 			 				NumAleatorio1a4() 	 				  *****/
/**************************************************************************/
/***** La función genera un número, entre 1 y 4, de forma aleatoria.  *****/
/**************************************************************************/
uint8_t NumAleatorio1a4(){
	uint8_t N_aleatorio;
	do{
		N_aleatorio = rand() % 10 + 1;
		if( (N_aleatorio >= 5) && (N_aleatorio <= 8) ){
			N_aleatorio = N_aleatorio - 4;
 		}
		else if( (N_aleatorio==9) || (N_aleatorio==10) ){
			N_aleatorio = N_aleatorio - 6;
		}
	}while( !( (N_aleatorio >= 1) && (N_aleatorio <=4) ) );
	return N_aleatorio;
}

/**************************************************************************/
/***** 			 		Move_Wumpus_ActualizaArreglo() 	 			  *****/
/**************************************************************************/
/***** Esta función mueve al Wumpus y actualiza las casillas de per_  *****/
/***** cepción de hedor.  											  *****/
/**************************************************************************/
void Move_Wumpus_ActualizaArreglo(uint8_t NCelementos[], uint8_t arreglo[]){
	uint8_t i;
	uint8_t Direccion = 0;
	uint8_t PreNCWumpus = 0;
	uint8_t NCWumpus;
	uint8_t NCTesoro;

	NCWumpus = NCelementos[1];
	NCTesoro = NCelementos[2];

	if( !(NCWumpus==0) ){
		//Borramos al Wumpus del casillero
		arreglo[NCWumpus] &= ~(1<<3);
		//Limpiamos hedor para ello ponemos a 0 todos los bits del arreglo
		for(i=0;i<16;i++){
			arreglo[i] &= ~(1<<2);
		}
		do{
			Direccion = NumAleatorio1a4();
			if( (NCWumpus == 1) || (NCWumpus == 2) ){
				while( !(Direccion != 4) ){
					Direccion = NumAleatorio1a4();
				}
			}
			else if(NCWumpus == 3){
				while( !( (Direccion != 2) && (Direccion != 4)  ) ){
					Direccion = NumAleatorio1a4();
				}
			}
			else if( (NCWumpus == 4) || (NCWumpus == 8) ){
				while( !(Direccion != 1) ){
					Direccion = NumAleatorio1a4();
				}
			}
			else if( (NCWumpus == 7) || (NCWumpus == 11) ){
				while( !(Direccion != 2) ){
					Direccion = NumAleatorio1a4();
				}
			}
			else if(NCWumpus == 12){
				while( !( (Direccion != 1) && (Direccion != 3) ) ){
					Direccion = NumAleatorio1a4();
				}
			}
			else if( (NCWumpus == 13) || (NCWumpus == 14) ){
				while( !(Direccion != 3) ){
					Direccion = NumAleatorio1a4();
				}
			}

			else if(NCWumpus == 15){
				while( !( (Direccion != 2) && (Direccion != 3) ) ){
					Direccion = NumAleatorio1a4();
				}
			}
			switch(Direccion){
				case 1 :
					PreNCWumpus = NCWumpus - 1;
					break;
				case 2 :
					PreNCWumpus = NCWumpus + 1;
					break;
				case 3 :
					PreNCWumpus = NCWumpus + 4;
					break;
				case 4 :
					PreNCWumpus = NCWumpus - 4;
			}
		}while( (PreNCWumpus == 0) || (PreNCWumpus == NCTesoro) );

		NCWumpus = PreNCWumpus;
		NCelementos[1] = NCWumpus;
		arreglo[NCWumpus] |= (1<<3);
		Actualiza_hedor(NCWumpus, arreglo);
	}
}


/**************************************************************************/
/***** 			 	Analisis_Tesoro_CasillaActual() 	 			  *****/
/**************************************************************************/
/***** Se analiza si en se alcanzó el tesoro. De ser así, la función  *****/
/***** retorna un "1".  											  *****/
/**************************************************************************/
uint8_t Analisis_Tesoro_CasillaActual(uint8_t arreglito[]){
	uint8_t Posicion_Avent,Tesoro;

	Posicion_Avent = N_Casillero_Aventurero(arreglito);
	Tesoro = Obtener_Bitx_Datop_Arreglo(arreglito,Posicion_Avent,4);
	return Tesoro;
}

/**************************************************************************/
/***** 			 			Evaluar_Reglas() 			 			  *****/
/**************************************************************************/
/***** Se analiza si el jugador sigue vivo o no (si se encontró al	  *****/
/***** Wumpus o si cayó en un aguero). Si sigue vivo, se retorna "1". *****/
/**************************************************************************/
uint8_t Evaluar_Reglas(uint8_t arreglo[]){
	uint8_t Vive = V;
	uint8_t Posicion_A,agujero,Wumpus;

	Posicion_A = N_Casillero_Aventurero(arreglo);
	agujero = ( (arreglo[Posicion_A] & (1<<1)) >>1 );
	Wumpus =  ( (arreglo[Posicion_A] & (1<<3)) >>3 );

	if(agujero==1){
		Vive = F;
		UART_TransmiteCadena("Caiste en un agujero\n\r");
	}
	else if(Wumpus==1){
		Vive = F;
		UART_TransmiteCadena("El Wumpus te alcanzo\n\r");
	}
	return Vive;
}

/**************************************************************************/
/***** 			 			Evaluar_Ganar() 			 			  *****/
/**************************************************************************/
/***** Se analiza si la partida se ganó. Si la partida fue exitosa,	  *****/
/***** se retorna un "1" y se muestra el mensaje "Ganaste.			  *****/
/**************************************************************************/
uint8_t Evaluar_Ganar(uint8_t tieneTesoro,uint8_t arreglo[]){
	uint8_t Ganar=F;
	uint8_t Posicion_Aven;

	Posicion_Aven = N_Casillero_Aventurero(arreglo);

	if( (Posicion_Aven == 0) && (tieneTesoro == 1) ){
		Ganar = V;
		UART_TransmiteCadena("Ganaste\n\r");
	}
	return Ganar;
}//fin función

/**************************************************************************/
/***** 			 Mostrar_Percepciones_CasillaAventurero() 			  *****/
/**************************************************************************/
/***** La función detecta la casilla en la que el jugador se ubica	  *****/
/***** y muestra las percepciones del aventurero.	  				  *****/
/**************************************************************************/
void Mostrar_Percepciones_CasillaAventurero(uint8_t arreglo[]){
	uint8_t hayElemento = F;
	uint8_t CasillaA,brisa,hedor,Wumpus,agujero,Tesoro;

	CasillaA = N_Casillero_Aventurero(arreglo);
	brisa   = Obtener_Bitx_Datop_Arreglo(arreglo, CasillaA, 0);
	hedor   = Obtener_Bitx_Datop_Arreglo(arreglo, CasillaA, 2);
	Wumpus  = Obtener_Bitx_Datop_Arreglo(arreglo, CasillaA, 3);
	agujero = Obtener_Bitx_Datop_Arreglo(arreglo, CasillaA, 1);
	Tesoro  = Obtener_Bitx_Datop_Arreglo(arreglo, CasillaA, 4);

	if(!(Wumpus & agujero)){
		UART_SiguienteLinea();
		UART_TransmiteCadena("	<w> : Moverse hacia arriba"); 		UART_SiguienteLinea();
		UART_TransmiteCadena("	<s> : Moverse hacia abajo"); 		UART_SiguienteLinea();
		UART_TransmiteCadena("	<a> : Moverse hacia la izquierda"); UART_SiguienteLinea();
		UART_TransmiteCadena("	<d> : Moverse hacia la derecha"); 	UART_SiguienteLinea();
		UART_TransmiteCadena("	<f> : Lanzar flecha"); 				UART_SiguienteLinea();
		UART_TransmiteCadena("Informacion : ");
	}
	if(brisa){
		UART_TransmiteCadena("Se siente una brisa ");
		hayElemento = V;
	}
	if(hedor){
		UART_TransmiteCadena("Se percibe hedor ");
		hayElemento = V;
	}
	if(Wumpus){
		UART_TransmiteCadena("Estas con el Wumpus ");
		hayElemento = V;
	}
	if(agujero){
		UART_TransmiteCadena("Estas en un agujero ");
		hayElemento = V;
	}
	if(Tesoro){
		UART_TransmiteCadena("Encontraste el Tesoro ");
		hayElemento = V;
	}

	if(hayElemento == F){
		UART_TransmiteCadena("Casilla limpia\n\r");
	}
	else{
		UART_SiguienteLinea();
	}
}

/**************************************************************************/
/***** 					 INTERFAZ_INICIO_JUEGO() 			  		  *****/
/**************************************************************************/
/***** Esta función presenta la interfaz de incio del juego. Se mues_ *****/
/***** tran los comandos para acceder a manual, records y juego.	  *****/
/**************************************************************************/
void INTERFAZ_INICIO_JUEGO(){
	uint8_t i;
	uint8_t letra;
	UART_TransmiteByte('\f');
	UART_TransmiteCadena("Bienvenido al Mundo de Wumpus");	   UART_SiguienteLinea();
	UART_TransmiteCadena("	x : Mostrar los mejores tiempos"); UART_SiguienteLinea();
	UART_TransmiteCadena("	z : Manual"); 					   UART_SiguienteLinea();
	UART_TransmiteCadena("	y : Empezar el juego"); 		   UART_SiguienteLinea();
	do{
		letra = UART_EsperaByte();
		if(letra == 'x'){
			UART_TransmiteByte('\f'); UART_SiguienteLinea();
			ObtenerRecords();
			VisualizaMenoresTiempos();
			for(i=0;i<9;i++){ UART_SiguienteLinea(); }
			UART_TransmiteCadena("Presione cualquier tecla para regresar");
			uint8_t datoRx = UART_EsperaByte();
			UART_TransmiteByte('\f');
			UART_TransmiteCadena("Bienvenido al Mundo de Wumpus");		UART_SiguienteLinea();
			UART_TransmiteCadena("	x : Mostrar los mejores tiempos");	UART_SiguienteLinea();
			UART_TransmiteCadena("	z : Manual");						UART_SiguienteLinea();
			UART_TransmiteCadena("	y : Empezar el juego");				UART_SiguienteLinea();
		}
		if(letra == 'z'){
			UART_TransmiteByte('\f'); UART_SiguienteLinea();
			UART_TransmiteCadena("Reglas y Consideraciones:"); UART_SiguienteLinea(); UART_SiguienteLinea();
			UART_TransmiteCadena("	Aventurero, entra al Mundo de Wumpus en busqueda del tesoro y sal");  UART_SiguienteLinea();
			UART_TransmiteCadena("	por donde entraste antes de que te coma. La entada se encuentra en"); UART_SiguienteLinea();
			UART_TransmiteCadena("	la esquina inferior derecha. Cada paso tuyo es un paso del Wumpus."); UART_SiguienteLinea();
			UART_TransmiteCadena("	Ten mucho cuidado! Hay agujeros en los que puedes caer. En las ca-"); UART_SiguienteLinea();
			UART_TransmiteCadena("	sillas adyacentes a agujeros, se percibe una BRISA; en las adyacen"); UART_SiguienteLinea();
			UART_TransmiteCadena("	-tes al Wumpus, HEDOR."); 					   UART_SiguienteLinea(); UART_SiguienteLinea();
			UART_TransmiteCadena("	o : Agujero"); 											 			  UART_SiguienteLinea();
			UART_TransmiteCadena("	b : Brisa"); 											 			  UART_SiguienteLinea();
			UART_TransmiteCadena("	W : Wumpus"); 											 			  UART_SiguienteLinea();
			UART_TransmiteCadena("	h : Hedor"); 											 			  UART_SiguienteLinea();
			UART_TransmiteCadena("	A : Aventurero"); 											 		  UART_SiguienteLinea();
			UART_TransmiteCadena("	T : Tesoro"); 											 			  UART_SiguienteLinea();
			for(i=0;i<9;i++){ UART_SiguienteLinea(); }
			UART_TransmiteCadena("	Presione cualquier tecla para regresar");
			uint8_t datoRx = UART_EsperaByte();
			UART_TransmiteByte('\f');
			UART_TransmiteCadena("Bienvenido al Mundo de Wumpus"); 		UART_SiguienteLinea();
			UART_TransmiteCadena("	x : Mostrar los mejores tiempos");	UART_SiguienteLinea();
			UART_TransmiteCadena("	z : Manual"); 						UART_SiguienteLinea();
			UART_TransmiteCadena("	y : Empezar el juego"); 			UART_SiguienteLinea();
		}
	}while( !(letra == 'y') );
	for(i=0;i<27;i++){ UART_SiguienteLinea(); }
}

/**************************************************************************/
/***** 							 JUEGO() 					  		  *****/
/**************************************************************************/
/***** Muestra y posiciona los elementos del tablero, espera comando  *****/
/***** durante la partida y mueve al Wumpus durante el juego.		  *****/
/**************************************************************************/
void JUEGO(){
	//**Variables**//
	uint8_t i;
	uint32_t TiempoInicial, TiempoFinal;
	uint32_t reiniciar;		//tecla para reiniciar el juego
	uint16_t nuevoTiempo;	//Tiempo que tardo el último jugador en esta partida
	uint8_t Vivo, flagTesoro_Casilla, flagTieneTesoro, Ganar;
	//**Inicialización**//
	uint8_t MiArreglo[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t NCelementos[6]={0,0,0,0,0,0};
	uint8_t EstadoWumpus = 0;
	uint8_t VivoWumpus = V;

	UART_TransmiteByte('\f');

	GENERAR_MOSTRAR_ELEMENTOS(MiArreglo,NCelementos);
	Mostrar_Percepciones_CasillaAventurero(MiArreglo);
	TiempoInicial = ObtenerHoraActualSegundos();

	do{
		ESPERA_COMANDO_ACTUALIZAR_ARREGLO(MiArreglo,NCelementos);
		UART_TransmiteByte('\f');
		EstadoWumpus = NCelementos[1];
		if(EstadoWumpus == 16){
			VivoWumpus = F;
		}

		UART_TransmiteByte('\f');
		if(MODO_PRUEBA == 1){
			MUESTRA_MUNDO_WUMPUS(MiArreglo);
		}
		Vivo = Evaluar_Reglas(MiArreglo);
		if(Vivo == V){
			flagTesoro_Casilla = Analisis_Tesoro_CasillaActual(MiArreglo);
			if(flagTesoro_Casilla){
				flagTieneTesoro=V;
				//Se elimina el tesoro del mapa porque ya lo tenemos
				for(i=0;i<16;i++){
					MiArreglo[i] &= ~(1<<4);
				}
			}
			Ganar=Evaluar_Ganar(flagTieneTesoro,MiArreglo);
			if(Ganar==F){
				if(VivoWumpus==V){
					Move_Wumpus_ActualizaArreglo(NCelementos,MiArreglo);
					UART_TransmiteByte('\f');
					if(MODO_PRUEBA==1){
						MUESTRA_MUNDO_WUMPUS(MiArreglo);
					}
				}
				Vivo = Evaluar_Reglas(MiArreglo);
			}
		}
		Mostrar_Percepciones_CasillaAventurero(MiArreglo);
	}while( (Vivo==V)&&(Ganar==F)  );
	//UART_TransmiteCadena("Perdiste\n\r");
	if(Ganar == V){
		TiempoFinal = ObtenerHoraActualSegundos();
		nuevoTiempo = TiempoFinal-TiempoInicial;
		AnalisisDePartida(nuevoTiempo);
		VisualizaMenoresTiempos();
	}
	UART_TransmiteCadena("Presione ESC para reiniciar el juego");
	do{
		reiniciar = UART_EsperaByte();
	}while( !(reiniciar == 27) );	//Espera a que se presione "ESC"
	UART_TransmiteByte('\f');
}

/**************************************************************************/
/***** 					 ObtenerMenoresTiempos() 					  *****/
/**************************************************************************/
/***** Captura los tiempos de la memoria y las almacena en arreglos.  *****/
/**************************************************************************/
void ObtenerMenoresTiempos(void){
	uint8_t i = 0;
	uint8_t a = 0;
	uint8_t valor;
	for(i=0;i<5;i++){	//Para pasar de los bloque 0 a 4 (NickNames)
		a=0;
		do{				//Para obtener cada uno de los caracteres de un NickName
			valor = EEPROM_lee(Block[i],a);
			MenoresTiempos[i][a] = valor;
			a++;
		}while((valor != 0)&(a<9));	//(a<16);
	}
}

/**************************************************************************/
/***** 					    ObtenerNickNames() 						  *****/
/**************************************************************************/
/***** Captura los nicknames de la memoria y las almacena en arreglos.*****/
/**************************************************************************/
void ObtenerNickNames(void){
	uint8_t i = 0;
	uint8_t a = 0;
	uint8_t valor;
	for(i=5;i<10;i++){	//Para pasar de los bloque 5 a 9 (NickNames)
		a=0;
		do{				//Para obtener cada uno de los caracteres de un NickName
			valor = EEPROM_lee(Block[i],a);
			NickNames[i-5][a] = valor;
			a++;
		}while((valor != 0)&(a<9));	//(a<16);
	}
}

/**************************************************************************/
/***** 						 ObtenerFechas() 					 	  *****/
/**************************************************************************/
/***** Captura las fechas de la memoria y las almacena en arreglos.  *****/
/**************************************************************************/
void ObtenerFechas(void){
	uint8_t i = 0;
	uint8_t a = 0;
	uint8_t valor;
	for(i=10;i<15;i++){			//Para pasar de los bloque 5 a 9 (NickNames)
		a=0;
		do{						//Para obtener cada uno de los caracteres de un NickName
			valor = EEPROM_lee(Block[i],a);
			Fechas[i-10][a] = valor;
			a++;
		}while((valor != 0)&(a<11));	//(a<16);
	}
}

/**************************************************************************/
/***** 						 ObtenerRecords() 					 	  *****/
/**************************************************************************/
/***** Almacena en arreglos los mejores tiempos, nicknames y fechas.  *****/
/**************************************************************************/
void ObtenerRecords(void){
	ObtenerMenoresTiempos();
	ObtenerNickNames();
	ObtenerFechas();
}

/**************************************************************************/
/***** 					 VisualizaMenoresTiempos() 				 	  *****/
/**************************************************************************/
/***** Muestra los mejores tiempos de jugada junto con el nickname    *****/
/***** y la fecha de registro.       								  *****/
/**************************************************************************/
void VisualizaMenoresTiempos(void){
	uint8_t i;
	uint8_t a,b,c;
	uint8_t valor;
	ObtenerRecords();
	UART_SiguienteLinea();
	UART_TransmiteCadena(MensajeMejores);
	UART_SiguienteLinea();
	UART_SiguienteLinea();
	UART_SiguienteLinea();
	UART_TransmiteCadena("       ");
	UART_TransmiteCadena("Tiempo");
	UART_TransmiteCadena("       ");
	UART_TransmiteCadena("Fecha");
	UART_TransmiteCadena("       ");
	UART_TransmiteCadena("NickName");
	UART_SiguienteLinea();
	for(i=0;i<5;i++){
		a = 0;
		b = 0;
		c = 0;
		UART_TransmiteCadena("      ");UART_TransmiteByte(62);UART_TransmiteCadena(" ");
		do{
			valor = MenoresTiempos[i][a];
			UART_TransmiteByte(valor);
			a++;
		}while((valor != 0)&(a<9));
		UART_TransmiteCadena("        ");
		do{
			valor = Fechas[i][c];
			UART_TransmiteByte(valor);
			c++;
		}while((valor != 0)&(c<11));
		UART_TransmiteCadena("      ");
		do{
			valor = NickNames[i][b];
			UART_TransmiteByte(valor);
			b++;
		}while((valor != 0)&(b<9));
		UART_SiguienteLinea();
	}
}
/**************************************************************************/
/***** 						 InicializaRecord() 				 	  *****/
/**************************************************************************/
/***** Esta función inicializa los records con tiempo: 999 y nickname *****/
/***** : "-----" a los 5 mejores puntajes.       					  *****/
/**************************************************************************/
void InicializaRecord(void){
	uint8_t i = 0;
	for(i=1;i<6;i++){
		GuardarNuevoRecord(999, nicknameInicio, i);
	}
}

/**************************************************************************/
/***** 			 GuardarNuevoRecord(uint16_t, uint8_t, uint8_t) 	  *****/
/**************************************************************************/
/***** Esta funcion recibe el tiempo (segundos), el nickname en una   *****/
/***** cadena ASCIIZ y la posición en la que debería estar ubicado y  *****/
/***** desplaza el resto de records. Si la posición es 0, no merece   *****/
/***** entrar en el ranking.										  *****/
/**************************************************************************/
void GuardarNuevoRecord(uint16_t tiempo, uint8_t nicknameZ[], uint8_t numeroRecord){
	//Esta funcion recibe el tiempo (segundos), el nickname en una cadena ASCIIZ
	//y la posicion en la que debería estar ubicado y desplaza el resto de records.
	//Si la posicion es 0, no merece entrar en el ranking.
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	uint8_t d = 0;
	uint8_t e = 0;
	uint8_t valor;
	uint8_t Tiempo[8];
	uint8_t fechaZ[10];
	uint8_t i = numeroRecord;
	if (i != 0){			//¿Merece entrar en el top?
	//Transformar el tiempo en segundos a una cadena ASCIIZ
		if(0<tiempo<10){	//Si es un numero de una cifra
			Tiempo[0] = tiempo + '0';
			Tiempo[1] = 0;
		}
		if(9<tiempo<100){	//Si es un numero de dos cifras
			Tiempo[0] = (tiempo/10) + '0';
			Tiempo[1] = (tiempo - (tiempo/10)*10) + '0';
			Tiempo[2] = 0;
		}
		if(99<tiempo<1000){	//Si es un numero de tres cifras
			Tiempo[0] = (tiempo/100) + '0';
			Tiempo[1] = ((tiempo - (tiempo/100)*100)/10) + '0';
			Tiempo[2] = (tiempo - (tiempo/100)*100 - ((tiempo - (tiempo/100)*100)/10)*10 ) + '0';
			Tiempo[3] = 0;
		}
	//Correr una posicion hacia abajo los tiempos y nicknames
		for(c = 5; c > i; c--){
			d = 0;
			do{
				EEPROM_escribe(MenoresTiempos[c-2][d],Block[c-1],Offset[d]);
				d++;
			}while(MenoresTiempos[c-2][d] != 0);
		}
		for(c = 5; c > i; c--){
			d = 0;
			do{
				EEPROM_escribe(NickNames[c-2][d],Block[c+4],Offset[d]);
				d++;
			}while(NickNames[c-2][d] != 0);
		}
		for(c = 5; c > i; c--){
			d = 0;
			do{
				EEPROM_escribe(Fechas[c-2][d],Block[c+9],Offset[d]);
				d++;
			}while(Fechas[c-2][d] != 0);
		}
	//Guarda Tiempo
		do{
			EEPROM_escribe(Tiempo[b],Block[i-1],Offset[b]);
			valor = Tiempo[b];
			b++;
		}while(valor != 0);
	//Guarda en NickName
		do{
			EEPROM_escribe(nicknameZ[a], Block[i+4], Offset[a]);
			valor = nicknameZ[a];
			a++;
		}while(valor != 0);
	//Obtener fecha actual
		ObtenerFechaActual(fechaZ);
	//Guardar Fecha
		do{
			EEPROM_escribe(fechaZ[e], Block[i+9], Offset[e]);
			valor = fechaZ[e];
			e++;
		}while(valor != 0);
	}
}

/**************************************************************************/
/***** 						 NumeroRecord(uint16_t) 				  *****/
/**************************************************************************/
/***** Esta funciónn calcula la variable "NumeroRecord" que será      *****/
/***** utilizada para ubicar la posición en la que debe ir este       *****/
/***** record, si se merece. Para esto, se debe comparar el tiempo    *****/
/***** de la partida actual con los records almacenados.			  *****/
/**************************************************************************/
uint8_t NumeroRecord(uint16_t tiempo){
	uint8_t i;
	uint16_t val;
	uint8_t NumeroRecord = 0;
	uint8_t NumeroRecordFlag = 0;
	//Se actualizan los arreglos Menores Tiempos y NickNames
	ObtenerRecords();
	//Comparamos el tiempo introducido con los mejores tiempos
	for(i=0;i<5;i++){
		val = ((MenoresTiempos[i][0]-'0')*100+(MenoresTiempos[i][1]-'0')*10+(MenoresTiempos[i][2]-'0'));
		if(NumeroRecordFlag == 0){
			if (tiempo < val){
				NumeroRecord = i+1;
				NumeroRecordFlag = 1;
			}
		}
	}
	//Devuelve la posicion en la que debería entrar este jugador
	return NumeroRecord;
	//Si el "nuevoRecord"=0 -> el jugador no merece entrar en el top
}

/**************************************************************************/
/***** 					 AnalisisDePartida(uint16_t) 				  *****/
/**************************************************************************/
/***** Recibe el tiempo que duró la partida en segundo y analiza si   *****/
/***** es necesario actualizar la tabla de records. Si se actualizará *****/
/***** se pedirá que ingrese el NICKNAME para quedar registrado en el *****/
/***** juego.														  *****/
/**************************************************************************/
void AnalisisDePartida(uint16_t tiempo){
	uint8_t numeroRecord;
	uint8_t nickname[10];
	//Se obtiene la posicion en la que se debería con los resultados de esta partida
	numeroRecord = NumeroRecord(tiempo);
	//Si el "numeroRecord"=0, no se hará nada
	if (numeroRecord !=  0){
		//Si se merece estar en la tabla de records, se solicita nickname
		//Felicidades
		UART_SiguienteLinea();
		UART_TransmiteCadena(MensajeNuevoRecord);
		UART_SiguienteLinea();
		//solicitar nickname
		UART_TransmiteCadena(MensajeSolicitaNickName);
		UART_EsperaCadena7Digitos(nickname);
		//Se graba el nuevo nickname
		GuardarNuevoRecord(tiempo, nickname, numeroRecord);
		//Actualiza los arrelos que contienen los nicknames y los tiempos
		ObtenerRecords();
	}
}
