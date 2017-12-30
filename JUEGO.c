/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: JUEGO.c												  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** Módulo(s) uC: Ninguno									 	  *****/
/***** Autor: Cristhian Foester Llamocca Ccaico						  *****/
/***** Fecha: Noviembre 2017 										  *****/
/**************************************************************************/
/***** Funciones para generación, muestra y actualización de los ele_ *****/
/***** mentos del juego.		 									  *****/
/**************************************************************************/
/**************************************************************************/
#include"tm4c123gh6pm.h"
#include <stdint.h>
#include "JUEGO.h"
#include "UART.h"

#define LF 10
#define CR 13

/**************************************************************************/
/***** 			 					enter() 			  		 	  *****/
/**************************************************************************/
/***** La función regresa al extremo izquierdo el cursor.			  *****/
/**************************************************************************/
void enter(){
	while(UART0_FR_R & UART_FR_TXFF);
	UART0_DR_R = 0xD;
}

/**************************************************************************/
/***** 			 					arriba() 			  		 	  *****/
/**************************************************************************/
/***** La función mueve "n" posiciones hacia arriba el cursor. 		  *****/
/**************************************************************************/
void arriba(uint8_t n){
	uint8_t i;
	for(i=0;i<n;i++){
		while(UART0_FR_R & UART_FR_TXFF);
		// Se escribe el dato para su transmisión
		UART0_DR_R = 0x1B;
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x5B;
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x41;
	}
}

/**************************************************************************/
/***** 			 					abajo() 			  		 	  *****/
/**************************************************************************/
/***** La función mueve "n" posiciones hacia abajo el cursor. 		  *****/
/**************************************************************************/
void abajo(uint8_t n){
	uint8_t i;
	for(i=0;i<n;i++){
		while(UART0_FR_R & UART_FR_TXFF);
		// Se escribe el dato para su transmisión
		UART0_DR_R = 0x1B;
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x5B;
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x42;
	}
}

/**************************************************************************/
/***** 			 					derecha() 			  		 	  *****/
/**************************************************************************/
/***** La función mueve "n" posiciones hacia la derecha el cursor.	  *****/
/**************************************************************************/
void derecha(uint8_t n){
	uint8_t i;
	for(i=0;i<n;i++){
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x1B;
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x5B;
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x43;
	}
}

/**************************************************************************/
/***** 			 					pos() 			  		 	 	  *****/
/**************************************************************************/
/***** La función mueve hacia el tope de arriba al cursor.	 		  *****/
/**************************************************************************/
void Pos(void){
	uint8_t b;
	for(b=0;b<16;b++){
		arriba(1);
	}
}

/**************************************************************************/
/***** 			 				InferiorIz() 			  	 	 	  *****/
/**************************************************************************/
/***** La función ubica al cursor en la posición inferior izquierda.  *****/
/**************************************************************************/
void InferiorIz(void){
		enter();
		abajo(16);
}

/**************************************************************************/
/***** 			 				LineaH() 			  	 	 	  	  *****/
/**************************************************************************/
/***** La función genera una división horizontal para el tablero.	  *****/
/**************************************************************************/
void LineaH(void){
	uint32_t i;
	uint8_t guiones[]="----------";
	for(i=0;i<4;i++){
		//Nos movemos a la derecha 1 posicion
		derecha(1);
		UART_TransmiteCadena(guiones);
	}
}

/**************************************************************************/
/***** 			 					LineaV() 			  	 	 	  *****/
/**************************************************************************/
/***** La función genera una división vertical para el tablero.  	  *****/
/**************************************************************************/
void LineaV(void){
	uint32_t i;
	for(i=0;i<17;i++){
		//escribimos linea
		while(UART0_FR_R & UART_FR_TXFF);
		UART0_DR_R = 0x7C;
		//Movemos a la izquierda
		UART_TransmiteByte(0x1B);
		UART_TransmiteByte(0x5B);
		UART_TransmiteByte(0x44);
		if(i<16){
			//movemos abajo
			abajo(1);
		}
	}
}

/**************************************************************************/
/***** 			 			GENERA_CUADRICULA() 			  	 	  *****/
/**************************************************************************/
/***** La función muestra el tablero generado en el interfaz serial.  *****/
/**************************************************************************/
void GENERA_CUADRICULA(void){
	uint8_t g;
	for(g=0;g<5;g++){
		LineaH();
		if(g<4){
			enter();//nos movemos a la izquierda con "enter"
			abajo(4);//bajamos 4 posiciones
		}
		else {
			enter();
		}
	}
	Pos(); //función que me ubica en la posicion de arriba (16)
	uint8_t h;
	for(h=0;h<5;h++){
		LineaV();
		if(h<4){
			Pos();//nos movemos 11 veces a la derecha
			derecha(11);
		}
		else{
			Pos();
		}
	}
	InferiorIz();
}

/**************************************************************************/
/***** 			 				UbicaCasilla() 			  	 		  *****/
/**************************************************************************/
/***** La función ubica al cursor en la casilla P. Se utiliza como	  *****/
/***** referencia la posicion inferior izquierda.					  *****/
/**************************************************************************/
void UbicaCasilla(uint8_t p){
 if( (p==0) || (p==1) || (p==2) || (p==3) ){
 arriba(2);
 }
 if( (p==5) || (p==6) || (p==7) || (p==4) ){
 arriba(6);
 }
 if( (p==9) || (p==10) || (p==11) || (p==8) ){
 arriba(10);
 }
 if( (p==13) || (p==14) || (p==15) || (p==12) ){
 arriba(14);
 }
 if( (p==0) || (p==4) || (p==8) || (p==12) ){
 derecha(2);
 }
 if( (p==1) || (p==5) || (p==9) || (p==13) ){
 derecha(13);
 }
 if( (p==2) || (p==6) || (p==10) || (p==14) ){
 derecha(24);
 }
 if( (p==3) || (p==7) || (p==11) || (p==15) ){
 derecha(35);
 }
}

/**************************************************************************/
/***** 			 				InferiorInicial() 		  	 		  *****/
/**************************************************************************/
/***** La función ubica al cursor en la posicion inferior izquierda   *****/
/***** Se utiliza como referencia la posicion P.					  *****/
/**************************************************************************/
void InferiorInicial(uint8_t p){
 enter();
 if( (p==0) || (p==1) || (p==2) || (p==3) ){
 abajo(2);
 }
 if( (p==5) || (p==6) || (p==7) || (p==4) ){
 abajo(6);
 }
 if( (p==9) || (p==10) || (p==11) || (p==8) ){
 abajo(10);
 }
 if( (p==13) || (p==14) || (p==15) || (p==12) ){
 abajo(14);
 }
}

/**************************************************************************/
/***** 			 		Obtener_Bitx_Datop_Arreglo() 		  	 	  *****/
/**************************************************************************/
/***** La función desplaza el dato en la posición "p" del arreglo[]   *****/
/***** y "x" indica el bit a desplazar dependiendo del nuemero a 	  *****/
/***** enmascarar.												      *****/
/**************************************************************************/
uint8_t Obtener_Bitx_Datop_Arreglo(uint8_t arreglo[], uint8_t p, uint8_t x){
	uint8_t Nuevo;
	switch(x){
	case 0: Nuevo= ((arreglo[p] & (1<<0))>>0);
			break;
	case 1: Nuevo= ((arreglo[p] & (1<<1))>>1);
			break;
	case 2: Nuevo= ((arreglo[p] & (1<<2))>>2);
			break;
	case 3: Nuevo= ((arreglo[p] & (1<<3))>>3);
			break;
	case 4: Nuevo= ((arreglo[p] & (1<<4))>>4);
			break;
	case 5: Nuevo= ((arreglo[p] & (1<<5))>>5);
			break;
	case 6: Nuevo= ((arreglo[p] & (1<<6))>>6);
			break;
	case 7: Nuevo= ((arreglo[p] & (1<<7))>>7);
	}
	return Nuevo;
}

/**************************************************************************/
/***** 			 				EscribeDatos() 		  			 	  *****/
/**************************************************************************/
/***** La función muestra las percepciones del jugador en la casilla  *****/
/***** actual en la que se encuentra.					  			  *****/
/**************************************************************************/
void EscribeDatos(uint8_t arreglo[], uint8_t p){
	uint8_t A, T, W, h, b, Q;
	derecha(2);
	A=Obtener_Bitx_Datop_Arreglo(arreglo, p, 5);
	if(A == 1){
		UART_TransmiteByte('A');
	}
	else{
		derecha(1);
	}
	T=Obtener_Bitx_Datop_Arreglo(arreglo, p, 4);
	if(T == 1){
		UART_TransmiteByte('T');
	}
	else{
		derecha(1);
	}
	W = Obtener_Bitx_Datop_Arreglo(arreglo, p, 3);
	if(W == 1){
		UART_TransmiteByte('W');
	}
	else{
		derecha(1);
	}
	h = Obtener_Bitx_Datop_Arreglo(arreglo, p, 2);
	if(h == 1){
		UART_TransmiteByte('h');
	}
	else{
		derecha(1);
	}
	Q = Obtener_Bitx_Datop_Arreglo(arreglo, p, 1);
	if(Q == 1){
		UART_TransmiteByte('O');
	}
	else{
		derecha(1);
	}
	b = Obtener_Bitx_Datop_Arreglo(arreglo, p, 0);
	if(b == 1){
		UART_TransmiteByte('b');
	}
}

/**************************************************************************/
/***** 			 		MUESTRA_ELEMENTOS_TABLERO()   			 	  *****/
/**************************************************************************/
/***** La función muestra la información de cada casilla (O, W, H, B, *****/
/*****  T, A).					  			  						  *****/
/**************************************************************************/
void MUESTRA_ELEMENTOS_TABLERO(uint8_t MiArreglo[]){
	uint8_t i;
	for(i=0;i<16;i++){
		UbicaCasilla(i);
		EscribeDatos(MiArreglo, i);
		InferiorInicial(i);
	}
}
