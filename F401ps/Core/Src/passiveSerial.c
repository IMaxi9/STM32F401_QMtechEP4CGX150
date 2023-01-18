/*
 * passiveSerial.c
 *
 *  Created on: 14 янв. 2023 г.
 *      Author: 1
 */
#include "pins.h"
#include "gpio.h"
#include "passiveSerial.h"
#include "fatfs.h"

void program_FPGA_PS(char* path) {
	dbgprintf("Send file: %s to PassiveSerial\n",path);
	unsigned long bytecount = 0;
    unsigned long file_size;
    unsigned int read_count = 0;
    unsigned long to_read;
    unsigned long loaded = 0;
    char sd_buffer[BUFFER_SIZE];
    uint32_t bytesread;
    DCLK_HIGH();
    nCONFIG_HIGH();
    ADSO_HIGH();
    FIL file;
    if (f_open(&file, path, FA_READ) != FR_OK){
    	dbgprintf("Error open %s\n",path);
    	return;
	}
    uint32_t total = f_size(&file);
    int divisor = total / 32;
	//int state = 0;
	file_size = total;
	read_count = file_size / sizeof( sd_buffer );
	if ( file_size % sizeof( sd_buffer ) != 0 ) {
		read_count ++;
	}
	dbgprintf("Programming > ");
	/* Drive a transition of 0 to 1 to NCONFIG to indicate start of configuration */
	nCONFIG_LOW();
	for(uint16_t i=0;i<45;i++)
		__NOP(); // must be low for at least 45us

	if ((GPIOA->IDR & (1<<2))) {
		dbgprintf("\nFPGA error... NSTATUS is not LOW\n");
	}
	nCONFIG_HIGH();

	// now wait for NSTATUS to go high
	// specs: max 800us
	uint32_t i = 1000000;
	while (!(GPIOA->IDR & (1<<2))){
		if (--i == 0){
	    	dbgprintf("FPGA NSTATUS is NOT high!\n");
	        f_close(&file);
	        return;
	    }
	}
	for ( int k = 1; k < read_count + 1; k++ ) {
	    to_read = ( file_size >= ( sizeof( sd_buffer ) * k ) ) ? sizeof( sd_buffer ) : file_size - loaded + 1;
	    //val = file.read( sd_buffer, to_read );
	    f_read(&file, sd_buffer, to_read, (void *)&bytesread);
	    for ( int f = 0; f < to_read; f ++ ) {

	      if ( bytecount % divisor == 0 ) {
	        dbgprintf("*");
	        if (!(GPIOA->IDR & (1<<2))) {
	        	dbgprintf("\nFPGA error... NSTATUS is LOW\n");
	        	dbgprintf("Programmed: %d bytes\n", bytecount);
	        return;
	        	}
	      }
	      bytecount++ ;
	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x01 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x02 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x04 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x08 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x10 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x20 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x40 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

//	      ADSO_LOW();
//	      DCLK_LOW();
	      DATCLK_LOW();
	      if ( ( sd_buffer[ f ]) & 0x80 )
	    	  ADSO_HIGH();
	      DCLK_HIGH();

	    } //to_read
	    loaded += to_read;
	  }	// count

	dbgprintf(" OK\nProgrammed: %l bytes\n", bytecount);
	f_close(&file);
	if (!(GPIOA->IDR & (1<<0))) {
		dbgprintf("FPGA error... CONF_DONE is LOW\r");
	      return;
	    }
	/* Start initialization */
	/* Clock another extra DCLK cycles while initialization is in progress
	through internal oscillator or driving clock cycles into CLKUSR pin */
	/* These extra DCLK cycles do not initialize the device into USER MODE */
	/* It is not required to drive extra DCLK cycles at the end of configuration */
	/* The purpose of driving extra DCLK cycles here is to insert some delay
	while waiting for the initialization of the device to complete before
	checking the CONFDONE and NSTATUS signals at the end of whole
	configuration cycle */

	for ( i = 0; i < 50; i++ ){
		DCLK_LOW();
		DCLK_HIGH();
	}
}
