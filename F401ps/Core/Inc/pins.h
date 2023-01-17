/*
 * pins.h
 *
 *  Created on: 30 ����. 2022 �.
 *      Author: Maksim Ilyin (imax9.narod.ru)
 */


#ifndef INC_PINS_H_
#define INC_PINS_H_

#include "main.h"
#define spiFPGA	hspi2
#define spiSD	hspi1

#define NEPTUNO
#define SDFAT_FILE_TYPE 1

#if SDFAT_FILE_TYPE == 1
#define SDFAT_FILE_TYPE_STR "FAT16/FAT32"
#elif SDFAT_FILE_TYPE == 2
#define SDFAT_FILE_TYPE_STR "exFAT"
#elif SDFAT_FILE_TYPE == 3
#define SDFAT_FILE_TYPE_STR "FAT16/FAT32/exFAT"
#endif
#ifdef NEPTUNO
#define SPLASH "         N E P T U N O  "
#define EXTENSION "NP1"
#define LAUNCHER_CORE_EXT "CNU"
#endif
#define LAUNCHER_EXTENSION "ARC"

#define CORE_EXTENSIONS EXTENSION"/"LAUNCHER_EXTENSION

//black pill
// #define PIN_TCK     PB0
// #define PIN_TDI     PB1
// #define PIN_TMS     PB2
// #define PIN_TDO     PB10

#define SPI_DESELECTED()   GPIOB->BSRR = (1U << 12) //pin PB12 (SPI_SS)
#define SPI_SELECTED()     GPIOB->BSRR = (1U << 28) //pin PB12 (SPI_SS)



#endif /* INC_PINS_H_ */
