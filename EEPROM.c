/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: EEPROM.c 											  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** Módulo(s) uC: EEPROM 										  *****/
/***** Autor: Sebastian Caballa Barrientos 							  *****/
/***** Fecha: Noviembre 2017 										  *****/
/**************************************************************************/
/***** Funciones para configuracion, grabado y lectura en EEPROM.	  *****/
/**************************************************************************/
/**************************************************************************/

//Limites//
//OFFSET 4bits
//DWR 32bits
//BLOQUE 16bits


#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EEPROM.h"

/**************************************************************************/
/***** 				 	EEPROM_inicializacion() 					  *****/
/**************************************************************************/
/***** Configura el EEPROM indicando que el ancho de palabra es de 32 *****/
/***** bits. 														  *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void EEPROM_inicializacion (void){
		//***********				EEPROM inicializacion				***********//
		int i;
		//Activar reloj del sistema para EEPROM
		SYSCTL_RCGCEEPROM_R |= SYSCTL_RCGCEEPROM_R0;
		//Delay (6 cycles plus funcition call overhead)
		for (i=0;i<6000;i++);
		//Esperar a que EEPROM termine su encendido
		while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING){}
		//Leer los bits PRETRY e ERETRY //Reiniciar proceso si hay error
		while( EEPROM_EESUPP_R & (EEPROM_EESUPP_PRETRY | EEPROM_EESUPP_ERETRY) ){
			EEPROM_EESUPP_R |= 0x01;
		}
		//Reiniciar módulo EEPROM
		SYSCTL_SREEPROM_R &=~ 0x01;
		//Esperar a que esté disponible el módulo
		while((SYSCTL_SREEPROM_R & 0x01)!=0){}
		//Delay (6 cycles plus funcition call overhead)
		for (i=0;i<6000;i++);
		//Esperar a que EEPROM termine su encendido
		while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING){}
		//Leer los bits PRETRY e ERETRY //Reiniciar proceso si hay error
		while( EEPROM_EESUPP_R & (EEPROM_EESUPP_PRETRY | EEPROM_EESUPP_ERETRY) ){
			EEPROM_EESUPP_R |= 0x01;
		}
}
/**************************************************************************/
/***** 				 		EEPROM_escribe() 					  	  *****/
/**************************************************************************/
/***** Guarda el Dato de 32 bits en la EEPROM en la ubicación deseada *****/
/**************************************************************************/
/***** ENTRADAS: Dato, Número de Block, Número de Offset			  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void EEPROM_escribe(uint32_t dato, uint16_t block, uint8_t offset){
	//***********						EEPROM write						***********//
	//Esperar hasta que la EEPROM esté disponible para trabajar
	while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING){}
	//Definimos una palabra de 32 bits
	EEPROM_EESIZE_R |= EEPROM_EESIZE_WORDCNT_M;
	//Escogemos el bloque (de 32) en el que escribiremos, solo si no utilizamos EEOFFSET
	EEPROM_EEBLOCK_R |= block;
	//Se selecciona el offset para ubicar el registro sobre el que se escribirá
	EEPROM_EEOFFSET_R = offset;
	//Utilizamos este registro seleccionado para almacenar
	EEPROM_EERDWR_R = dato;
	//Reiniciamos los valores de EEOFSET y BLOCK
	EEPROM_EEOFFSET_R &=~ offset;
	EEPROM_EEBLOCK_R &=~ block;
}
/**************************************************************************/
/***** 				 			EEPROM_lee() 					  	  *****/
/**************************************************************************/
/***** Lee el Dato de 32 bits en la EEPROM en la ubicación deseada 	  *****/
/**************************************************************************/
/***** ENTRADAS: Número de Block, Número de Offset			 		  *****/
/***** SALIDA: Dato almacenado en EEPROM							  *****/
/**************************************************************************/
uint32_t EEPROM_lee(uint16_t block, uint8_t offset){
	//***********						EEPROM read							***********//
	//Esperar hasta que la EEPROM esté disponible para trabajar
	while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING){}
	//Variable para mostrar valor almacenado en EEPROM
	uint32_t valor;
	//Definimos una palabra de 32 bits
	EEPROM_EESIZE_R |= EEPROM_EESIZE_WORDCNT_M;
	//Escogemos el bloque (de 32) del que leeremos, solo si no utilizamos EEOFFSET
	EEPROM_EEBLOCK_R |= block;
	//Se selecciona el offset para ubicar el registro que se leerá
	EEPROM_EEOFFSET_R |= offset;
	//Utilizamos este registro seleccionado leer data
	valor = EEPROM_EERDWR_R;
	//Reiniciamos los valores de EEOFSET y BLOCK
	EEPROM_EEOFFSET_R &=~ offset;
	EEPROM_EEBLOCK_R &=~ block;
	//Devolver valor almacenado en la direccion (Offset)
	return valor;
}
