/**************************************************************************/
/**************************************************************************/
/***** 			 PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ 			  *****/
/***** 				 FACULTAD DE CIENCIAS E INGENIERÍA 				  *****/
/***** 						SISTEMAS DIGITALES 						  *****/
/**************************************************************************/
/***** Archivo: RTC.c												  *****/
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

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "RTC.h"
#include "I2c.h"

//****FUNCIONES SYSTEMCONTROL****//
void RTC_inicializacion(void){
/************************************************************************/
/************************************************************************/
/*****   Descripción: Se configura el reloj del sistema con los	    *****/
/*****   parámetros requeridos para esta aplicación. Se habilitará	*****/
/*****   un divisor del reloj, PLL como fuente, un oscilador y una 	*****/
/*****   frecuencia externa para poder utilizar una fuente de reloj	*****/
/*****   distinta a la que se utiliza por defecto, 16MHz.		 	*****/
/************************************************************************/
/************************************************************************/
	//Constantes a utilizar en la funcion
    uint32_t i;
    uint32_t Delay = 524288;
    uint32_t SYSCTL_RCC_R_PREV;
    uint32_t SYSCTL_RCC_RCONST;
    uint32_t SYSCTL_RCC2_RCONST;
	//Setin Bypass the PLL and system clock dividers
	SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;
	SYSCTL_RCC_R &=~ SYSCTL_RCC_USESYSDIV;
	SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
    // See if the oscillator needs to be enabled.
	//SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ;
	uint32_t Config = (0x01C00000 | 0x00000000 | 0x00000010 | 0x00000540);
	uint32_t SYSCTL_MAIN_OSC_DIS = 0x00000001;
	if((SYSCTL_RCC_R & SYSCTL_RCC_MOSCDIS) && !(Config & SYSCTL_MAIN_OSC_DIS)){
		SYSCTL_RCC_R_PREV = SYSCTL_RCC_R;
		// Make sure that the required oscillators are enabled.
		SYSCTL_RCC_R_PREV &= (~SYSCTL_RCC_MOSCDIS | (Config & SYSCTL_MAIN_OSC_DIS));
        // Clear the MOSC power up raw interrupt status to be sure it is not
        // set when waiting below.
        SYSCTL_MISC_R = SYSCTL_MISC_MOSCPUPMIS;
        SYSCTL_RCC_R = SYSCTL_RCC_R_PREV;
        // Timeout using the legacy delay value.
        while(((SYSCTL_RIS_R) & SYSCTL_RIS_MOSCPUPRIS) == 0)
        {
            Delay--;
            if(Delay == 0) { break; }
        }
        // If the main oscillator failed to start up then do not switch to
        // it and return.
        if(Delay == 0) { return; }

    }
	    // Set the new crystal value and oscillator source.  Because the OSCSRC2
	    // field in RCC2 overlaps the XTAL field in RCC, the OSCSRC field has a
	    // special encoding within ui32Config to avoid the overlap.
		SYSCTL_RCC_RCONST = SYSCTL_RCC_R;
		SYSCTL_RCC2_RCONST = SYSCTL_RCC2_R;
		SYSCTL_RCC_RCONST &= ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M);
		SYSCTL_RCC_RCONST |= Config & (SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M);
		SYSCTL_RCC2_RCONST &= ~(SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_OSCSRC2_M);
		SYSCTL_RCC2_RCONST |= Config & (SYSCTL_RCC2_USERCC2 | SYSCTL_RCC_OSCSRC_M);
		SYSCTL_RCC2_RCONST |= (Config & 0x00000008) << 3;
		SYSCTL_RCC_R = SYSCTL_RCC_RCONST;
		SYSCTL_RCC2_R = SYSCTL_RCC2_RCONST;
	    // Set the PLL configuration.
		SYSCTL_RCC_RCONST &= ~(SYSCTL_RCC_PWRDN);
		SYSCTL_RCC_RCONST |= Config & SYSCTL_RCC_PWRDN;
		SYSCTL_RCC2_RCONST &= ~SYSCTL_RCC2_PWRDN2;
		SYSCTL_RCC2_RCONST |= Config & SYSCTL_RCC2_PWRDN2;
	    // Clear the PLL lock interrupt.
	    SYSCTL_MISC_R = SYSCTL_MISC_PLLLMIS;
	    // Write the new RCC value.
	    if(SYSCTL_RCC_RCONST & SYSCTL_RCC2_USERCC2){
	        SYSCTL_RCC2_R = SYSCTL_RCC2_RCONST;
	        SYSCTL_RCC_R = SYSCTL_RCC_RCONST;
	    }
	    else{
	    	SYSCTL_RCC_R = SYSCTL_RCC_RCONST;
	    	SYSCTL_RCC2_R = SYSCTL_RCC2_RCONST;
	    }
	    // Set the requested system divider and disable the appropriate
	    // oscillators.  This value is not written immediately.
	    SYSCTL_RCC_RCONST &= ~(SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV | SYSCTL_RCC_MOSCDIS);
	    SYSCTL_RCC_RCONST |= Config & (SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV | SYSCTL_RCC_MOSCDIS);
	    SYSCTL_RCC2_RCONST &= ~(SYSCTL_RCC2_SYSDIV2_M);
	    SYSCTL_RCC2_RCONST |= Config & SYSCTL_RCC2_SYSDIV2_M;
	    if(Config & SYSCTL_RCC2_DIV400){
	    	SYSCTL_RCC_RCONST |= SYSCTL_RCC_USESYSDIV;
	        SYSCTL_RCC2_RCONST &= ~(SYSCTL_RCC_USESYSDIV);
	        SYSCTL_RCC2_RCONST |= Config & (SYSCTL_RCC2_DIV400 | SYSCTL_RCC2_SYSDIV2LSB);
	    }
	    else{
	    	SYSCTL_RCC2_RCONST &= ~(SYSCTL_RCC2_DIV400);
	    }
	    // See if the PLL output is being used to clock the system.
	    if(!(Config & SYSCTL_RCC_BYPASS)){
	        // Wait until the PLL has locked.
	        for(Delay = 32768; Delay > 0; Delay--){
	            if((SYSCTL_PLLSTAT_R & SYSCTL_PLLSTAT_LOCK)) { break; }
	        }
	        // Enable use of the PLL.
	        SYSCTL_RCC_RCONST &= ~(SYSCTL_RCC_BYPASS);
	        SYSCTL_RCC2_RCONST &= ~(SYSCTL_RCC2_BYPASS2);
	    }
	    // Write the final RCC value.
	    SYSCTL_RCC_R = SYSCTL_RCC_RCONST;
	    SYSCTL_RCC2_R = SYSCTL_RCC2_RCONST;
	    // Delay for a little bit so that the system divider takes effect.
	    for(i=0;i<16;i++);
}
//****FUNCIONES RTC****//
unsigned char BCDToBinary(unsigned char val){
 return (((val & 0xF0) >> 4) * 10) + (val & 0x0F);
}
//Get Time and Date
unsigned char GetClock(unsigned char reg){
     unsigned char clockData = I2CReceive(SLAVE_ADDRESS,reg);
     return BCDToBinary(clockData);
}
uint8_t GetSegundo(void){
	uint8_t segundo;
	segundo = (uint8_t)GetClock(SEC);
	return segundo;
}
uint8_t GetMinuto(void){
	uint8_t minuto;
	minuto = (uint8_t)GetClock(MIN);
	return minuto;
}
uint8_t GetHora(void){
	uint8_t hora;
	hora = (uint8_t)GetClock(HRS);
	return hora;
}
uint8_t GetDiaSemana(void){
	uint8_t diasemana;
	diasemana = (uint8_t)GetClock(DAY);
	return diasemana;
}
uint8_t GetDia(void){
	uint8_t dia;
	dia = (uint8_t)GetClock(DATE);
	return dia;
}
uint8_t GetMes(void){
	uint8_t mes;
	mes = (uint8_t)GetClock(MONTH);
	return mes;
}
uint8_t GetAno(void){
	uint8_t ano;
	ano = (uint8_t)GetClock(YEAR);
	return ano;
}
void ObtenerFechaActual(uint8_t Arreglo[]){
	//Esta funcion almacena en un arreglo la fecha actual en el formato
	//[Año/Mes/Dia] y añade un '0' al final del arreglo para convertirlo
	//en cadena ASCCIZ.
	uint8_t dia,mes,ano;
	dia = GetDia();
	mes = GetMes();
	ano = GetAno();

	Arreglo[0] = ((dia/10)+'0');
	Arreglo[1] = ((dia-(dia/10)*10)+'0');
	Arreglo[2] = '/';
	Arreglo[3] = ((mes/10)+'0');
	Arreglo[4] = ((mes-(mes/10)*10)+'0');
	Arreglo[5] = '/';
	Arreglo[6] = ((ano/10)+'0');
	Arreglo[7] = ((ano-(ano/10)*10)+'0');
	Arreglo[8] = 0;
}
void ObtenerHoraActual(uint8_t Arreglo[]){
	//Esta funcion almacena en un arreglo la fecha actual en el formato
	//[Hora/Minuto/Segundo] y añade un '0' al final del arreglo para convertirlo
	//en cadena ASCCIZ.
	uint8_t segundo,minuto,hora;
	segundo = GetSegundo();
	minuto = GetMinuto();
	hora = GetHora();

	Arreglo[0] = ((hora/10)+'0');
	Arreglo[1] = ((hora-(hora/10)*10)+'0');
	Arreglo[2] = ':';
	Arreglo[3] = ((minuto/10)+'0');
	Arreglo[4] = ((minuto-(minuto/10)*10)+'0');
	Arreglo[5] = ':';
	Arreglo[6] = ((segundo/10)+'0');
	Arreglo[7] = ((segundo-(segundo/10)*10)+'0');
	Arreglo[8] = 0;
}
uint32_t ObtenerHoraActualSegundos(void){
	//Se obtiene la hora actual y se transforma a segundos
	uint32_t tiempoSeg;
	uint8_t segundo,minuto,hora;
	segundo = GetSegundo();
	minuto = GetMinuto();
	hora = GetHora();
	tiempoSeg = (hora*60*60+minuto*60+segundo);
	return tiempoSeg;
}
