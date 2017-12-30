/**************************************************************************/
/**************************************************************************/
/***** 			PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: UART.h 												  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** Módulo(s) uC: UART0 											  *****/
/***** Autor: Rolando Sánchez Ponce 								  *****/
/***** Fecha: Octubre 2017 											  *****/
/**************************************************************************/
/***** Funciones para comunicación serial asíncrona mediante el UART0 *****/
/**************************************************************************/
/***** Conexiones: 													  *****/
/***** U0Rx - PA0  													  *****/
/***** U0Tx - PA1  													  *****/
/**************************************************************************/
/**************************************************************************/
#ifndef _UART_H_
#define _UART_H_
/**************************************************************************/
/***** 						C O N S T A N T E S 					  *****/
/**************************************************************************/
/***** CONSTANTES BOOLEANAS *****/
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
/***** CONSTANTES PARA PARA CARACTERES ASCII *****/
#define LF  10  // Nueva línea
#define FF  12  // Nueva página
#define CR  13  // Retorno de carro
#define DEL 127 //Borrar
/**************************************************************************/
/***** 			P R O T O T I P O S D E F U N C I O N E S 			  *****/
/**************************************************************************/
/**************************************************************************/
/***** 						UART_Inicializa() 						  *****/
/**************************************************************************/
/***** Configura el UART0 con los siguientes parámetros de comunica-  *****/
/***** ción serial: 9600, 8, N, 1. 									  *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_inicializacion(void);
/**************************************************************************/
/***** 						UART_EsperaByte() 						  *****/
/**************************************************************************/
/***** Lee un dato recibido por el puerto serial, para esto espera    *****/
/***** que haya un dato en el buffer de recepción del UART0 para 	  *****/
/***** leerlo. 														  *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Dato recibido por el UART0 							  *****/
/**************************************************************************/
uint8_t UART_EsperaByte(void);
/**************************************************************************/
/***** 						UART_RevisaRx()							  *****/
/**************************************************************************/
/***** Revisa si hay algún dato recibido en el buffer de recepción 	  *****/
/***** del UART0. Si lo hay lo lee. No hay lazo de espera. 			  *****/
/**************************************************************************/
/***** ENTRADAS: 													  *****/
/***** *datoRxPtr: Dirección de memoria donde se almacena el dato 	  *****/
/***** recibido si lo hubiera. 										  *****/
/***** SALIDA: TRUE: Hay dato recibido y leido. 					  *****/
/***** FALSE: No hay dato recibido. 								  *****/
/**************************************************************************/
uint8_t UART_RevisaRx(uint8_t *datoRxPtr);
/**************************************************************************/
/***** 						UART_TransmiteByte() 					  *****/
/**************************************************************************/
/***** Escribe un byte en el buffer de transmisión del UART0 para 	  *****/
/***** ser transmitido por el puerto serial. Si el buffer estuviera   *****/
/***** lleno espera hasta que se pueda escribir. 					  *****/
/**************************************************************************/
/***** ENTRADAS: 													  *****/
/***** datoTx: Dato a transmitir por el puerto serial 				  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_TransmiteByte(uint8_t datoTx);
/**************************************************************************/
/***** 						UART_SiguienteLinea() 					  *****/
/**************************************************************************/
/***** Transmite los caracteres necesarios para posicionar el cursor  *****/
/***** de la pantalla del terminal al inicio de la siguiente línea.   *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_SiguienteLinea(void);
/**************************************************************************/
/***** 						UART_EsperaCadena() 					  *****/
/**************************************************************************/
/***** Recibe por el puerto serial una cadena de caracteres ASCII y   *****/
/***** la almacena en un arreglo. El usuario ingresa ENTER para fina- *****/
/***** lizar y debe ingresar por lo menos un caracter antes del ENTER *****/
/***** La función inserta 0 al final de la cadena para que sea ASCIIZ *****/
/***** La función hace un eco por cada caracter recibido y posiciona  *****/
/***** el cursor al inicio de la siguiente línea al salir. 			  *****/
/**************************************************************************/
/***** ENTRADAS: 													  *****/
/***** cadenaASCIIZ[]: Dirección de inicio de la cadena ASCIIZ 		  *****/
/***** a recibir.													  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_EsperaCadena(uint8_t cadenaASCIIZ[]);
/**************************************************************************/
/***** 				UART_EsperaCadena7Digitos() 					  *****/
/**************************************************************************/
/***** Recibe por el puerto serial una cadena de caracteres ASCII y   *****/
/***** la almacena en un arreglo. El usuario ingresa ENTER para fina- *****/
/***** lizar y debe ingresar por lo menos un caracter antes del ENTER *****/
/***** La función inserta 0 al final de la cadena para que sea ASCIIZ *****/
/***** La longitud máxima de la cadena es de 8 caracteres.			  *****/
/**************************************************************************/
/***** ENTRADAS: 													  *****/
/***** cadenaASCIIZ[]: Dirección de inicio de la cadena ASCIIZ 		  *****/
/***** a recibir.													  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_EsperaCadena7Digitos(uint8_t cadenaASCIIZ[]);
/**************************************************************************/
/***** 						UART_TransmiteCadena() 					  *****/
/**************************************************************************/
/***** Transmite por el puerto serial una cadena ASCIIZ. 			  *****/
/**************************************************************************/
/***** ENTRADAS: 													  *****/
/***** cadenaASCIIZ[]: Dirección de inicio de la cadena ASCIIZ a 	  *****/
/***** a transmitir. 												  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_TransmiteCadena(const uint8_t cadenaASCIIZ[]);
/**************************************************************************/
/***** 						UART_EsperaNumeroB10() 					  *****/
/**************************************************************************/
/***** Recibe por el puerto serial una cadena de caracteres ASCII que *****/
/***** representan un número en base 10. El usuario ingresa ENTER pa- *****/
/***** ra finalizar y debe ingresar por lo menos un digito antes del  *****/
/***** ENTER. La función hace un eco por cada caracter recibido y po- *****/
/***** siciona el cursor al inicio de la siguiente línea al salir.    *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna. 											  *****/
/***** SALIDA: Número ingresado por el usuario. 					  *****/
/***** Rango [0, 4294967295] 										  *****/
/**************************************************************************/
uint32_t UART_EsperaNumeroB10(void);
/**************************************************************************/
/***** 						UART_TransmiteNumeroB10() 				  *****/
/**************************************************************************/
/***** Transmite por el puerto serial los caracteres que representan  *****/
/***** un número en base 10. 										  *****/
/**************************************************************************/
/***** ENTRADAS: 													  *****/
/***** numero: Número a transmitirse en caracteres ASCII. 			  *****/
/***** Rango [0, 4294967295] 										  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_TransmiteNumeroB10(uint32_t numero);
#endif
/****************************************************************************/
/***** 						F I N D E L A R C H I V O 					*****/
/****************************************************************************/
