/*
 * osd.h
 *
 *  Created on: 30 сент. 2022 г.
 *      Author: Maksim Ilyin (imax9.narod.ru)
 */

#ifndef INC_OSD_H_
#define INC_OSD_H_
#include <stdbool.h>
#define spiFPGA	hspi2
#define spiSD	hspi1

#define MM1_OSDCMDWRITE    0x20      // OSD write video data command
#define MM1_OSDCMDENABLE   0x41      // OSD enable command
#define MM1_OSDCMDDISABLE  0x40      // OSD disable command

#define OSDNLINE         8           // number of lines of OSD
#define OSDLINELEN       256         // single line length in bytes

#define OSD_ARROW_LEFT 1
#define OSD_ARROW_RIGHT 2

#define SCREEN_WIDTH_IN_CHARS 32

void spi_osd_cmd_cont(unsigned char cmd);
void EnableOsdSPI();
void DisableOsdSPI();
void spi8(unsigned char parm);
void spi16(unsigned short parm);
void spi24(unsigned long parm);
void spi32(unsigned long parm);
void OSDVisible (bool visible);
void OsdClear(void);
void OsdWrite(unsigned char n, char *s, unsigned char invert, unsigned char stipple, unsigned char arrow);
void OsdWriteOffset(unsigned char n, char *s, unsigned char invert, unsigned char stipple, char offset, unsigned char arrow);
void OSD_progressBar(unsigned char line, char *text, unsigned char percent);
void initOSD(void);
void removeOSD(void);
bool confirmAutoLoad();

#endif /* INC_OSD_H_ */
