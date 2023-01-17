/*
 * sdCard.h
 *
 *  Created on: 2 dec 2022.
 *      Author: 1
 */

#ifndef INC_SDCARD_H_
#define INC_SDCARD_H_
#include <stdbool.h>
//#define SPI_SD 1

//SpiPort_t SPI_PORT(SPI_SD);
//#define SD_CONFIG SdSpiConfig(PIN_CSSD, DEDICATED_SPI, SD_SCK_MHZ(50), &SPI_PORT)
//SdFat sd1;

extern bool SD_disabled;
extern bool image_loaded;
extern myListEntry myFileList[64];
extern char tableName[64*(128)];
void initializeSdCard(void);
void disableSD (unsigned char disable_sd);
uint8_t buildCat(char* path);


#endif /* INC_SDCARD_H_ */
