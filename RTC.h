/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: RTC.h												  *****/
/***** Microcontrolador: TM4C123GH6PM 								  *****/
/***** Módulo(s) uC: I2C 										 	  *****/
/***** Autor: Sebastian Caballa Barrientos 							  *****/
/***** Fecha: Noviembre 2017 										  *****/
/**************************************************************************/
/***** Funciones para inicialización de la comunicación entre reloj	  *****/
/***** externo y microcontrolador, recepción de hora y fecha en di_	  *****/
/***** ferentes formatos.											  *****/
/**************************************************************************/
/**************************************************************************/

#ifndef RTC_H_
#define RTC_H_

//****REGISTROS DS1307****//
 #define SLAVE_ADDRESS  0x68	//READ=0xE8;
 #define SEC 			0x00
 #define MIN 			0x01
 #define HRS 			0x02
 #define DAY 			0x03
 #define DATE 			0x04
 #define MONTH 			0x05
 #define YEAR 			0x06
 #define CNTRL 			0x07

//****DIRECTIVAS****//
//void SetTimeDate(unsigned char, unsigned char, unsigned char,unsigned char, unsigned char, unsigned char,unsigned char);
unsigned char BCDToBinary(unsigned char);	//CORRECTO
unsigned char GetClock(unsigned char reg);	//CORRECTO
void RTC_inicializacion(void);	//CORRECTO
uint8_t GetSegundo(void);
uint8_t GetMinuto(void);
uint8_t GetHora(void);
uint8_t GetDiaSemana(void);
uint8_t GetDia(void);
uint8_t GetMes(void);
uint8_t GetAno(void);
void ObtenerFechaActual(uint8_t Arreglo[]);
void ObtenerHoraActual(uint8_t Arreglo[]);
uint32_t ObtenerHoraActualSegundos(void);

#endif /* RTC_H_ */
