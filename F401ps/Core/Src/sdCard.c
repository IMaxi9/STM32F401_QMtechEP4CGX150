/*
 * sdCard.c
 *
 *  Created on: 2 dec 2022.
 *      Author: Maksim Ilyin (imax9.narod.ru)
 */

#include "main.h"
#include <stdbool.h>
#include "navigateOptions.h"
#include "sdCard.h"
#include "fileBrowser.h"
#include "fatfs.h"
#include "ff.h"
#include "osd.h"
#include "user_diskio_spi.h"
//#include "spi.h"
extern unsigned char option_sel[MAX_OPTIONS];
extern FATFS fs;
extern char dbgstr[256];
//char type[2]={0,0};
extern GPIO_InitTypeDef GPIO_InitStruct;
void initializeSdCard(void) {
	FRESULT fres;
	HAL_Delay(1000);
//	uint8_t s[512];
//	DSTATUS stat = USER_SPI_initialize(0);
//	dbgprintf("USER_SPI_initialize %i\n", stat);
//    HAL_Delay(100);
//    DRESULT dres=USER_SPI_read(0,s,0x0000,1);
//    dbgprintf("USER_SPI_read %i\n", dres);
//    HAL_Delay(100);
//    dbgprintf("Read: %X %X %X %X %X %X %X %X\n",s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7]);
    HAL_Delay(100);
	fres = f_mount(&fs, "", 1); //1=mount now
	    if (fres != FR_OK) {
	    	dbgprintf("mount error %i\n", fres);
	    	while(1);
	    }
	    else{
	    	dbgprintf("Mount OK\n");
	    }
	    HAL_Delay(100);
	    DWORD free_clusters;
	    FATFS* fs_ptr = &fs;
	    fres = f_getfree("", &free_clusters, &fs_ptr);
	    if (fres != FR_OK) {
	    	dbgprintf("getfree error %i\n", fres);
	    while(1);
	        }
	    else{
	    	dbgprintf("fre_clust: %lu\n",free_clusters);
	    }

//  if ( !sd1.begin( SD_CONFIG ) ) {
	    if (fres != FR_OK){
	  dbgprintf("SD Card initialization failed!\n");

    //hold until a SD is inserted
    while ( (f_mount(&fs, "", 1))!= FR_OK ) {
      HAL_Delay(2000);
    }

  }
//  Log.notice("SD Card initialization done."CR);
  dbgprintf("SD Card initialization done.\n");
}

uint8_t buildCat(char* path){
	//char type[2]={0,0};
//	uint8_t error = 2; // no need file
	uint16_t offsetName = 0;
//	char* currentAddr = tableName;
	static FILINFO fileInfo;
	DIR dir;
	FRESULT fres;
	uint8_t i=0;
	char fileName[256];
	uint16_t lenName;
	memset(&myFileList,0,sizeof(myFileList));
	dbgprintf("buildCat %s\n", path);
	if (strlen(path)>1){
		dbgprintf("insert parent type\n");
		myFileList[0].typeFile = PAR_TYPE;
		myFileList[0].index = 1;
		myFileList[0].lenName = 1;
		i++;
	}
	fres = f_opendir(&dir, path);

	if (fres == FR_OK){
		while(i<64){
		    fres = f_readdir(&dir, &fileInfo);
//		    dbgprintf("%s error=%i\n",fileInfo.fname,fres);
		    strncpy (fileName,fileInfo.fname,LONG_FILENAME_LEN);
		    if (fres==FR_OK && fileName[0]){
		    	if(fileInfo.fattrib&AM_SYS || fileInfo.fattrib&AM_HID ){
		    		continue;
		    	}
		    	if(fileInfo.fattrib&AM_DIR){
		    		myFileList[i].typeFile = DIR_TYPE;
		    	}else{
		    		myFileList[i].typeFile = FIL_TYPE;
		    		if(!(strstr(fileName, "np1") || strstr(fileName, "np2"))){
		    			continue;
		    		}
		    	}
//		    	strcpy (fileName,fileInfo.fname);
//		    	dbgprintf("%s\n",fileName);
		    	lenName = strlen(fileName);

		    if(lenName){
		    	myFileList[i].lenName =lenName;
		    	myFileList[i].index = i+1;
		    	myFileList[i].offsetName = offsetName;
//		    	strncpy (&tableName+offsetName,&fileName,lenName);
		    	uint8_t j=0;
		    	for(j=0;j<lenName;j++){
		    	tableName[offsetName+j]=fileName[j];
		    	}
		    	if(myFileList[i].typeFile == FIL_TYPE){
		    		dbgprintf("%s\n",fileName);
		    	}else{
		    		dbgprintf("<%s>\n",fileName);
		    	}
		    	offsetName+=(lenName);
		    	i++;
		    	myFileList[i].index = 0;
		    	}
		    }
		    else{
//		    	dbgprintf("Error open file %i i=%i\n",fres,i);
		    	break;
		    }
		}
		f_closedir(&dir);
	}else{
		sprintf(dbgstr,"opendir error %i", fres);
		OsdWriteOffset( 2, dbgstr, 0, 0, 0, 0 );
//		error = 1; // opendir error
	}
	dbgprintf("Files=%d\n",i-1);
	return i-1;
}

void disableSD (unsigned char disable_sd) {
//    Log.trace("disableSD : %d"CR, disable_sd);
    dbgprintf("disableSD : %d\n",disable_sd);

    if (disable_sd)
    {
      if (!SD_disabled)
      {
        //SPI.setModule( SPI_SD );
       // SPI.end();
//        pinMode( PA4, INPUT_PULLUP );
//        pinMode( PA5, INPUT_PULLUP );
//        pinMode( PA6, INPUT_PULLUP );
//        pinMode( PA7, INPUT_PULLUP );
//        GPIOA->CRL &= ~(GPIO_CRL_MODE7|GPIO_CRL_CNF7|GPIO_CRL_MODE6|GPIO_CRL_CNF6|GPIO_CRL_MODE5|GPIO_CRL_CNF5|GPIO_CRL_MODE4|GPIO_CRL_CNF4);
        //GPIOA->CRL |= (0x00 << GPIO_CRL_MODE7_Pos) | (0x02 << GPIO_CRL_CNF7_Pos);
//        GPIOA->CRL |= ((0x02 << GPIO_CRL_CNF7_Pos)|(0x02 << GPIO_CRL_CNF6_Pos)|(0x02 << GPIO_CRL_CNF5_Pos)|(0x02 << GPIO_CRL_CNF4_Pos));
//        GPIOA->ODR |= ((1<<7)|(1<<6)|(1<<5)|(1<<4));

    	  GPIO_InitStruct.Pin = PIN_CSSD_Pin;
    	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    	  GPIO_InitStruct.Pull = GPIO_PULLUP;
    	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    	  HAL_GPIO_Init(PIN_CSSD_GPIO_Port, &GPIO_InitStruct);

    	  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    	  GPIO_InitStruct.Pull = GPIO_PULLUP;
    	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    	  //GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        SD_disabled = true;
        //SendStatusWord ();
//        Log.notice("SD disabled"CR);
        dbgprintf("SD disabled\n");

      }
    }
    else
    {
     if (SD_disabled)
     {
//        pinMode( PA4, OUTPUT ); // SS
//        pinMode( PA5, OUTPUT ); // SCK
//        pinMode( PA6, INPUT_PULLUP ); // MISO
//        pinMode( PA7, OUTPUT ); // MOSI
//        GPIOA->CRL &= ~(GPIO_CRL_MODE7|GPIO_CRL_CNF7|GPIO_CRL_MODE6|GPIO_CRL_CNF6|GPIO_CRL_MODE5|GPIO_CRL_CNF5|GPIO_CRL_MODE4|GPIO_CRL_CNF4);
//        GPIOA->CRL |= (0x00 << GPIO_CRL_MODE7_Pos) | (0x02 << GPIO_CRL_CNF7_Pos);
//        GPIOA->CRL |= (0x01<<GPIO_CRL_CNF6_Pos) | (0x00<<GPIO_CRL_MODE6_Pos);
//        GPIOA->CRL |= (0x02<<GPIO_CRL_CNF5_Pos) | (0x03<<GPIO_CRL_MODE5_Pos);
    	 GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    	 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    	 GPIO_InitStruct.Pull = GPIO_NOPULL;
    	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    	 GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    	 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    	 GPIO_InitStruct.Pin = PIN_CSSD_Pin;
    	 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    	 GPIO_InitStruct.Pull = GPIO_PULLUP;
    	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    	 HAL_GPIO_Init(PIN_CSSD_GPIO_Port, &GPIO_InitStruct);

        SD_disabled = false;
   //     SendStatusWord ();
//        Log.notice("SD enabled"CR);
        dbgprintf("SD enabled\n");
      }
    }

    //tell the core we already handled the request
    option_sel[ 31 ] = 0xff;

}
