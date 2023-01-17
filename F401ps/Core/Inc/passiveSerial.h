/*
 * passiveSerial.h
 *
 *  Created on: 14 янв. 2023 г.
 *      Author: 1
 */

#ifndef INC_PASSIVESERIAL_H_
#define INC_PASSIVESERIAL_H_

#define BUFFER_SIZE 2048

//black pill F401
// #define DCLK			PB0 out
// #define CONF_DONE	PA0 in
// #define nCONFIG		PA1 out
// #define nSTATUS		PA2 in
// #define ADSO			PB8 out

#define ps_clock() GPIOB->BSRR = (1U << 0); GPIOB->BSRR =(1U << 16);

#define DCLK_HIGH() GPIOB->BSRR = (1U << 0);
#define DCLK_LOW()  GPIOB->BSRR =(1U << 16);
#define ADSO_HIGH() GPIOB->BSRR = (1U << 8);
#define ADSO_LOW()  GPIOB->BSRR =(1U << 24);
#define nCONFIG_HIGH() GPIOA->BSRR =(1U << 1);
#define nCONFIG_LOW()  GPIOA->BSRR =(1U << 17);

void program_FPGA_PS(char* path);

#endif /* INC_PASSIVESERIAL_H_ */
