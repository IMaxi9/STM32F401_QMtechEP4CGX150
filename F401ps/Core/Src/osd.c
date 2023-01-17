/*
 * osd.c
 *
 *  Created on: 30 nov 2022.
 *      Author: Maksim Ilyin (imax9.narod.ru)
 */
#include <fileBrowser.h>
#include "osd.h"
#include "main.h"
#include "charrom.h"
#include "readKeyboard.h"

#define SPI_DESELECTED()   GPIOB->BSRR = (1U << 12) //pin PB12 (SPI_SS)
#define SPI_SELECTED()     GPIOB->BSRR = (1U << 28) //pin PB12 (SPI_SS)
extern SPI_HandleTypeDef hspi2;
extern char strDebug[128];

void spi_osd_cmd_cont(unsigned char cmd)
{
  EnableOsdSPI();
  //SPI.transfer(cmd);

  HAL_SPI_Transmit(&spiFPGA,&cmd,1,100);
}

void spi_n(unsigned char val, unsigned short cnt)
{
  while (cnt--)
	  HAL_SPI_Transmit(&spiFPGA,&val,1,100);
}

void spi8(unsigned char parm)
{
  //SPI.transfer(parm);
  HAL_SPI_Transmit(&spiFPGA,&parm,1,100);
}

void spi16(unsigned short parm)
{
	uint8_t data;
	data=(parm >> 8)&0xff;
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
	data=(parm&0xff);
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
//  SPI.transfer(parm >> 8);
//  SPI.transfer(parm >> 0);
}

void spi24(unsigned long parm)
{
//  SPI.transfer(parm >> 16);
//  SPI.transfer(parm >> 8);
//  SPI.transfer(parm >> 0);
	uint8_t data;
	data=(parm >> 16)&0xff;
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
	data=(parm >> 8)&0xff;
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
	data=(parm&0xff);
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
}

void spi32(unsigned long parm)
{
//  SPI.transfer(parm >> 24);
//  SPI.transfer(parm >> 16);
//  SPI.transfer(parm >> 8);
//  SPI.transfer(parm >> 0);
	uint8_t data;
	data=(parm >> 24)&0xff;
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
	data=(parm >> 16)&0xff;
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
	data=(parm >> 8)&0xff;
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
	data=(parm&0xff);
	HAL_SPI_Transmit(&spiFPGA,&data,1,100);
}

void EnableOsdSPI()
{
  //SPI.setModule(2); // select the second SPI (connected on FPGA)
  SPI_SELECTED();   // slave active
}

void DisableOsdSPI()
{
  SPI_DESELECTED(); // slave deselected
  //SPI.setModule(1); // select the SD Card SPI
}

void OSDVisible (bool visible)
{
  if (visible)
  {
    spi_osd_cmd_cont(0x41);
  }
  else
  {
    spi_osd_cmd_cont(0x40);
  }
  DisableOsdSPI();
}

void OsdClear(void)
{
  spi_osd_cmd_cont(0x20);
  // clear buffer
  spi_n(0x00, OSDLINELEN * OSDNLINE);
  DisableOsdSPI();
}

void OsdWrite(unsigned char n, char *s, unsigned char invert, unsigned char stipple, unsigned char arrow)
{
  OsdWriteOffset(n, s, invert, stipple, 0, arrow);
}

// write a null-terminated string <s> to the OSD buffer starting at line <n>
void OsdWriteOffset(unsigned char n, char *s, unsigned char invert, unsigned char stipple, char offset, unsigned char arrow)
{
  unsigned short i;
  unsigned char b;
  const unsigned char *p;
  unsigned char stipplemask = 0xff;
  int linelimit = OSDLINELEN - 1;
  int arrowmask = arrow;

  // Serial.print("OsdWriteOffset ");
  //Serial.println(s);

  if (n == 7 && (arrow & OSD_ARROW_RIGHT))
  {
    linelimit -= 22;
  }

  if (stipple)
  {
    stipplemask = 0x55;
    stipple = 0xff;
  }
  else
  {
    stipple = 0;
  }

  // select buffer and line to write to
  spi_osd_cmd_cont(MM1_OSDCMDWRITE | n);


  if (invert) invert = 255;

  i = 0;

  // send all characters in string to OSD
  while (1)
  {

    /* if(i==0)
      {
           // Render sidestripe
           unsigned char j;

           p = &titlebuffer[(7-n)*8];

           spi16(0xffff);  // left white border

           for(j=0;j<8;j++) spi_n(255^*p++, 2);

           spi16(0xffff);  // right white border
           spi16(0x0000);  // blue gap
           i += 22;
      }
      else*/ if (n == 7 && (arrowmask & OSD_ARROW_LEFT)) // Draw initial arrow
    {

      unsigned char b;

      spi24(0x00);

      p = &charfont[0x10][0];

      for (b = 0; b < 8; b++) spi8(*p++ << offset);

      p = &charfont[0x14][0];

      for (b = 0; b < 8; b++) spi8(*p++ << offset);

      spi24(0x00);
      spi_n(invert, 2);
      i += 24;
      arrowmask &= ~OSD_ARROW_LEFT;

      if (*s++ == 0) break; // Skip 3 characters, to keep alignent the same.
      if (*s++ == 0) break;
      if (*s++ == 0) break;
    }
    else
    {

      b = *s++;

      if (b == 0)  break; // end of string


      else if (b == 0x0d || b == 0x0a) // cariage return / linefeed, go to next line
      {
        // increment line counter
        if (++n >= linelimit) n = 0;

        // send new line number to OSD
        DisableOsdSPI();

        spi_osd_cmd_cont(MM1_OSDCMDWRITE | n);
      }
      else if (i < (linelimit - 8)) // normal character
      {
        unsigned char c;
        p = &charfont[b][0];

        // if (b==0x41) Serial.println("----");

        for ( c = 0; c < 8; c++ ) //character width
        {
          //  if (b==0x41) Serial.println(*p);

          spi8(((*p << offset)&stipplemask)^invert);
          p++;
          stipplemask ^= stipple;
        }

        i += 8;

      }
    }
  }

  for (; i < linelimit; i++)  spi8(invert); // clear end of line


  if (n == 7 && (arrowmask & OSD_ARROW_RIGHT)) // Draw final arrow if needed
  {
    unsigned char c;

    spi24(0x00);
    p = &charfont[0x15][0];

    for (c = 0; c < 8; c++) spi8(*p++ << offset);

    p = &charfont[0x11][0];

    for (c = 0; c < 8; c++) spi8(*p++ << offset);

    spi24(0x00);
    i += 22;
  }

  // deselect OSD SPI device
  DisableOsdSPI();
}

// write a null-terminated string <s> to the OSD buffer starting at line <n>
void OSD_progressBar(unsigned char line, char *text, unsigned char percent )
{
  // line : OSD line number (0-7)
  // text : pointer to null-terminated string
  // start : start position (in pixels)
  // width : printed text length in pixels
  // offset : scroll offset in pixels counting from the start of the string (0-7)
  // invert : invertion flag

  unsigned long start = 215;
  unsigned long width = 40;
//  unsigned long offset = 0;
//  unsigned char invert = 0;
//  int i, j;
  unsigned char count = 0;
  const unsigned char *p;

  // select buffer and line to write to
  spi_osd_cmd_cont(MM1_OSDCMDWRITE | line);

  while ( count < start )
  {
    count++;

    if (percent < (count * 100 / 214))
    {
      spi8( 0x00 );
    }
    else
    {
      spi8( 0xff );
    }
  }

  while ( width > 8 )
  {
    unsigned char b;
    p = &charfont[*text++][0];
    for (b = 0; b < 8; b++) spi8(*p++);
    width -= 8;
  }

  if (width)
  {
    p = &charfont[*text++][0];
    while (width--)
      spi8(*p++);
  }

  DisableOsdSPI();
}

void initOSD(void)
{
  EnableOsdSPI();
  OsdClear();
  OSDVisible(true);
}

void removeOSD(void)
{
  EnableOsdSPI();
  OsdClear();
  OSDVisible(false);
  DisableOsdSPI();
}

bool confirmAutoLoad() {
  int event;
  unsigned char key, cmd;
  int cur_select = 1;
  int total_options = 2;

//  Log.notice("confirmAutoLoad"CR);
  dbgprintf("confirmAutoLoad\n");
  OsdClear();
  OsdWriteOffset( 1, "Auto load this image next time?", 0, 0, 0, 0 );

  do {
    OsdWriteOffset( 3, " - No", (cur_select == 0), 0, 0, 0 );
    OsdWriteOffset( 4, " - Yes", (cur_select == 1), 0, 0, 0 );

    event = readKeyboard( &key, &cmd );

    if ( event == EVENT_KEYPRESS ) {
      if ( key == KEY_UP ) {
        if ( cur_select > 0 ) {
          cur_select--;
        } else {
          cur_select = total_options - 1;
        }
      }
      else if ( key == KEY_DOW ) {
        if ( cur_select < total_options - 1 ) {
          cur_select++;
        } else {
          cur_select = 0;
        }
      }
      else if ( key == KEY_RET ) {
        break;
      }
    }
    HAL_Delay(150);
  } while ( true );

  return cur_select;
}
