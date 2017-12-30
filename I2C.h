/**************************************************************************/
/**************************************************************************/
/***** 			      PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			      *****/
/***** 				        FACULTAD DE CIENCIAS E INGENIERÍA 				      *****/
/***** 						            SISTEMAS DIGITALES 						          *****/
/**************************************************************************/
/***** Archivo: I2C.h												                          *****/
/***** Microcontrolador: TM4C123GH6PM 								                *****/
/***** Módulo(s) uC: I2C 										 	                        *****/
/***** Autor: Sebastian Caballa Barrientos 					            		  *****/
/***** Fecha: Noviembre 2017 										                      *****/
/**************************************************************************/
/***** Funciones para inicialización, y envío y recepción de datos a  *****/
/***** través de comunicación I2C. 									                  *****/
/**************************************************************************/
/**************************************************************************/

#ifndef I2C_H_
#define I2C_H_
/**************************************************************************/
/***** 				 		          I2C_inicialization() 					            *****/
/**************************************************************************/
/***** Configura el módulo I2C0 como maestro para enviar y recibir 	  *****/
/***** data.														                              *****/
/**************************************************************************/
/***** ENTRADAS: Ninguna 											                        *****/
/***** SALIDA: Ninguna 												                        *****/
/**************************************************************************/
void I2C_inicialization(void);
/**************************************************************************/
/***** 				 			            I2C_send() 							              *****/
/**************************************************************************/
/***** Envía un dato a la dirección escogida					 	              *****/
/**************************************************************************/
/***** ENTRADAS: Direccion base, Dato								                  *****/
/***** SALIDA: Ninguna 												                        *****/
/**************************************************************************/
void I2C_send(int8_t,uint8_t);
/**************************************************************************/
/***** 				 			            I2CReceive() 						              *****/
/**************************************************************************/
/***** Recibe un dato de la dirección escogida en el registro 		    *****/
/***** especificado	  												                        *****/
/**************************************************************************/
/***** ENTRADAS: Direccion base, Registro específico				          *****/
/***** SALIDA: Dato 												                          *****/
/**************************************************************************/
uint32_t I2CReceive(uint32_t, uint8_t);

#endif /* I2C_H_ */
