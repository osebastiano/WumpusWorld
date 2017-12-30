/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CAT�LICA DEL PER� 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIER�A 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: I2C.c												  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** M�dulo(s) uC: I2C 										 	  *****/
/***** Autor: Sebastian Caballa Barrientos 							  *****/
/***** Fecha: Noviembre 2017 										  *****/
/**************************************************************************/
/***** Funciones para inicializaci�n, y env�o y recepci�n de datos a  *****/
/***** trav�s de comunicaci�n I2C. 									  *****/
/**************************************************************************/
/**************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "I2C.h"


/***** 				 		I2C_inicialization() 					  *****/
/**************************************************************************/
/***** Configura el m�dulo I2C0 como maestro para enviar y recibir 	  *****/
/***** data.														  *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void I2C_inicialization(void){
	 //Habilita el m�dulo I2C0
	 SYSCTL_RCGCI2C_R |= 0x0001;
	 //Espera a que realmente se active
	 while((SYSCTL_PRI2C_R & 0x0001) == 0){};
	 //Habilita perif�rico GPIO que contiene el I2C0, PORTB
	 SYSCTL_RCGCGPIO_R |= 0x0002;
	 //Espera a que realmente se active
	 while((SYSCTL_PRGPIO_R & 0x0002) == 0){};
	 //Configurar PB2,3 para I2C
	 GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FF) | 0x00003300;
	 //Habilitar funciones alternativas en PB2,3
	 GPIO_PORTB_AFSEL_R |= 0x0C;
	 //Habilitar Open Drain en PB3
	 GPIO_PORTB_ODR_R |= 0x08;
	 //Habilitar I/O digitales
	 GPIO_PORTB_DEN_R |= 0x0C;
	 //Configurar I2C para un reloj de 100kpbs
	 I2C0_MTPR_R =  24;
	 //Habilitar funcion de maestro
	 I2C0_MCR_R = 16;
}
/***** 				 			I2C_send() 							  *****/
/**************************************************************************/
/***** Env�a un dato a la direcci�n escogida					 	  *****/
/**************************************************************************/
/***** ENTRADAS: Direccion base, Dato								  *****/
/***** SALIDA: Ninguna 												  *****/
/**************************************************************************/
void I2C_send(int8_t slave, uint8_t data){
	//Esperar a que el m�dulo I2C est� disponible
	while(I2C0_MCS_R & I2C_MCS_BUSY){};// wait for I2C ready
	//Selecciona direcci�n de envio
	I2C0_MSA_R = (slave<<1) & 0xFE;
	//Se configura para envio
	I2C0_MSA_R &= ~(0x01);
	//Colocar dato en el FIFO
	I2C0_MDR_R = data;
	I2C0_MCS_R = 0x00000020;
	I2C0_MCS_R |=   0x00000007;
	//Esperar a que termine la trasnmici�n
	while(I2C0_MCS_R & I2C_MCS_BUSY){};
}
/***** 				 			I2CReceive() 						  *****/
/**************************************************************************/
/***** Recibe un dato de la direcci�n escogida en el registro 		  *****/
/***** especificado	  												  *****/
/**************************************************************************/
/***** ENTRADAS: Direccion base, Registro espec�fico				  *****/
/***** SALIDA: Dato 												  *****/
/**************************************************************************/
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg){
		///Esperar a que m�dulo est� disponible para trabajar
		while(I2C0_MCS_R & I2C_MCS_BUSY){};
		//Selecciona direcci�n de envio
		I2C0_MSA_R = (slave_addr<<1) & 0xFE;
		//Especifica el registro a leer
		I2C0_MDR_R = reg;
		/*I2C0_MRIS_R |= 0;*/
		I2C0_MCS_R = 0x00000020;
		I2C0_MCS_R |= 0x00000003;
		//Esperar a que I2C termine el proceso
		while(I2C0_MCS_R & I2C_MCS_BUSY){};
		//Escoger la direcci�n de la cual se va a leer
		I2C0_MSA_R |= (slave_addr << 1)|1;
		I2C0_MCS_R = 0x00000020;
		I2C0_MCS_R |=   0x00000007;
		//Esperar a que I2C termine el proceso
		while(I2C0_MCS_R & I2C_MCS_BUSY){};
		//Retorna la data que se ha recibido
		return (I2C0_MDR_R & 0xFF);
}
