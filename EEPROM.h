/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: EEPROM.h 											  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** Módulo(s) uC: EEPROM 										  *****/
/***** Autor: Sebastian Caballa Barrientos 							  *****/
/***** Fecha: Noviembre 2017 										  *****/
/**************************************************************************/
/***** Funciones para configuracion, grabado y lectura en EEPROM.	  *****/
/**************************************************************************/
/**************************************************************************/
#ifndef EEPROM_H_
#define EEPROM_H_

/***** 				 	EEPROM_inicializacion() 					  *****/
/**************************************************************************/
/***** Configura el EEPROM indicando que el ancho de palabra es de 32 *****/
/***** bits. 														  *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void EEPROM_inicializacion (void);
/***** 				 		EEPROM_escribe() 					  	  *****/
/**************************************************************************/
/***** Guarda el Dato de 32 bits en la EEPROM en la ubicación deseada *****/
/**************************************************************************/
/***** ENTRADAS: Dato, Número de Block, Número de Offset			  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void EEPROM_escribe(uint32_t, uint16_t , uint8_t);
/***** 				 			EEPROM_lee() 					  	  *****/
/**************************************************************************/
/***** Lee el Dato de 32 bits en la EEPROM en la ubicación deseada 	  *****/
/**************************************************************************/
/***** ENTRADAS: Número de Block, Número de Offset			 		  *****/
/***** SALIDA: Dato almacenado en EEPROM							  *****/
/**************************************************************************/
uint32_t EEPROM_lee(uint16_t, uint8_t);

#endif /* EEPROM_H_ */
