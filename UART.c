/**************************************************************************/
/***** 				F U N C I O N E S P Ú B L I C A S 			  	  *****/
/**************************************************************************/
/**************************************************************************/
/***** 						UART_Inicializa() 						  *****/
/**************************************************************************/
/***** Configura el UART0 con los siguientes parámetros de comunica-  *****/
/***** ción serial: 115200, 8, N, 1. 								  *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"

void UART_inicializacion(void) {
	//**Inicializar UART0**//
	//Activamos reloj para GPIOA
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
	//Esperamos a que se active realmente
	while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0));
	//Activar reloj de UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
	//Esperamos a que se active realmente
	while(!(SYSCTL_PRUART_R & SYSCTL_PRUART_R0));
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00)| 0x00000011;
	//Activamos funciones alternas
	GPIO_PORTA_AMSEL_R &= ~0x03;
	GPIO_PORTA_AFSEL_R |= 0x03;
	//Activamos pines digitales
	GPIO_PORTA_DEN_R |= 0x03;
	// Use the internal 16MHz oscillator as the UART clock source.
	UART0_CC_R |= 0x05;
	// Initialize the UART for console I/O.
	//UARTStdioConfig(0, 115200, 16000000);
	//Inhabilitamos UART
	UART0_CTL_R &= ~(UART_CTL_UARTEN);
	//velocidad de 115200bps Y 16MHz
	UART0_IBRD_R = (UART0_IBRD_R & 0xFFFFFFF0) |8;
	UART0_FBRD_R = (UART0_FBRD_R & 0xFFFFFFC0) |44;
	//8,N,1 FIFOS
	UART0_LCRH_R = (UART0_LCRH_R & 0xFFFFFF00)| 0x70;
	//Habilitamos UART
	UART0_CTL_R |= (UART_CTL_RXE|UART_CTL_TXE|UART_CTL_UARTEN);
}
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
uint8_t UART_EsperaByte(void) {
	uint8_t datoRx;
	while (UART0_FR_R & UART_FR_RXFE);
	datoRx = (uint8_t) UART0_DR_R;
	return datoRx;
}
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
uint8_t UART_RevisaRx(uint8_t *datoRxPtr) {
	uint8_t hayDatoRxFlag;
	hayDatoRxFlag = FALSE;
	if (!(UART0_FR_R & UART_FR_RXFE)) {
		*datoRxPtr = (uint8_t) UART0_DR_R;
		hayDatoRxFlag = TRUE;
	}
	return hayDatoRxFlag;
}
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
void UART_TransmiteByte(uint8_t datoTx) {
	while (UART0_FR_R & UART_FR_TXFF)
		;
	UART0_DR_R = (UART0_DR_R & 0xFFFFFF00) | datoTx;
}
/**************************************************************************/
/***** 						UART_SiguienteLinea() 					  *****/
/**************************************************************************/
/***** Transmite los caracteres necesarios para posicionar el cursor  *****/
/***** de la pantalla del terminal al inicio de la siguiente línea.   *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void UART_SiguienteLinea(void) {
// Transmitimos nueva línea (LF)
	UART_TransmiteByte(LF);
// Transmitimos retorno de carro (CR)
	UART_TransmiteByte(CR);
}
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
void UART_EsperaCadena(uint8_t cadenaASCIIZ[]) {
	uint8_t datoRx;
	uint8_t hay1CaracterFlag = FALSE;
	uint8_t finFlag = FALSE;
	uint8_t i = 0;
	do {
// Esperamos recibir un dato por el puerto serial.
		datoRx = UART_EsperaByte();
// Si dato recibido es diferente de ENTER.
		if (datoRx != CR) {
			if (!hay1CaracterFlag)
				hay1CaracterFlag = TRUE;
			UART_TransmiteByte(datoRx);
			cadenaASCIIZ[i++] = datoRx;
		}
// Si dato recibido es ENTER.
		else if (hay1CaracterFlag) {
			cadenaASCIIZ[i] = 0;
			UART_SiguienteLinea();
			finFlag = TRUE;
		}
	} while (!finFlag);
}
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
void UART_EsperaCadena7Digitos(uint8_t cadenaASCIIZ[]) {
	uint8_t datoRx;
	uint8_t hay1CaracterFlag = FALSE;
	uint8_t finFlag = FALSE;
	uint8_t i = 0;
	uint8_t a = 0;
	do {
// Esperamos recibir un dato por el puerto serial.
		datoRx = UART_EsperaByte();
// Si dato recibido es diferente de ENTER.
		if ( (datoRx != CR) & (a<7) ){
			if(datoRx != DEL){
				if (!hay1CaracterFlag)
					hay1CaracterFlag = TRUE;
				UART_TransmiteByte(datoRx);
				cadenaASCIIZ[i++] = datoRx;
				a++;
			}
			else{
				UART_TransmiteByte(datoRx);
				i--;
				a--;
			}
		}
// Si dato recibido es ENTER.
		else if (hay1CaracterFlag) {
			cadenaASCIIZ[i] = 0;
			UART_SiguienteLinea();
			finFlag = TRUE;
		}
	} while (!finFlag);
}
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
void UART_TransmiteCadena(const uint8_t cadenaASCIIZ[]) {
	uint8_t i;
	for (i = 0; cadenaASCIIZ[i]; i++) {
		UART_TransmiteByte(cadenaASCIIZ[i]);
	}
}
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
uint32_t UART_EsperaNumeroB10(void) {
	uint8_t datoRx;
	uint32_t numero = 0;
	uint8_t hay1DigitoFlag = FALSE;
	uint8_t finFlag = FALSE;
	do {
// Esperamos recibir un dato por el puerto serial.
		datoRx = UART_EsperaByte();
// Si dato recibido es diferente de ENTER.
		if (datoRx != CR) {
// Si dato recibido es un caracter numérico
			if (('0' <= datoRx) && (datoRx <= '9')) {
				if (!hay1DigitoFlag)
					hay1DigitoFlag = TRUE;
				UART_TransmiteByte(datoRx);
				numero = 10 * numero + datoRx - '0';
			}
		}
// Si dato recibido es ENTER.
		else if (hay1DigitoFlag) {
			UART_SiguienteLinea();
			finFlag = TRUE;
		}
	} while (!finFlag);
	return numero;
}
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
void UART_TransmiteNumeroB10(uint32_t numero) {
	uint32_t divisor = 1000000000;
	uint8_t hayDigSigFlag = FALSE;
	uint8_t i, digito;
// Repetimos 10 veces: máxima cantidad de dígitos que puede tener número.
	for (i = 0; i < 10; i++) {
// Hallamos digito y actualizamos numero y divisor.
		digito = numero / divisor;
		numero = numero % divisor;
		divisor = divisor / 10;
// Si aún no llegamos al dígito más significativo de numero.
		if (!hayDigSigFlag) {
// Si digito es diferente de 0
			if (digito != 0) {
				hayDigSigFlag = TRUE;
				UART_TransmiteByte(digito + '0');
			}
// Si digito es 0 y es la última iteración
			else if (i == 9) {
				UART_TransmiteByte(digito + '0');
			}
		}
// Si ya hemos llegado al dígito más significativo.
		else {
			UART_TransmiteByte(digito + '0');
		}
	}
}
/**************************************************************************/
/***** 					F I N D E L A R C H I V O 					  *****/
/**************************************************************************/
