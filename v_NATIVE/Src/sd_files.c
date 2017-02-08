#include "sd_files.h"
#include "diskio.h"
#include "ff_gen_drv.h"
#include "fatfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "simcom.h"
#include "stdlib.h"

void parseSD_wm(char * pData ){ // we parse string like wm=150
  uint16_t ind = 0;
  if(pData[0] == 'w')
    if(pData[1] == 'm')
      if((pData[2] > 0x30) &&(pData[2] < 0x39) ){
         ind = pData[2] - 0x30 -1;
          Sim800.WM_SD.index[ind] = (ind + 1); //existed index
          if(pData[3] == '='){
           pData = &pData[4];
           Sim800.WM_SD.price[ind] = atoi(pData);
          }
          
      }

}
 
void SD_GetData(void){ 
  
  FRESULT frslt;
  FATFS filesystem;
  char filebuf[32];  
  char * pChar = &filebuf[0];
  FIL file;
  UINT nRead, ind = 0;
 osDelay(100); 
taskENTER_CRITICAL(); 
{
 frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
 if(frslt == FR_OK){
 frslt = f_open(&file, "wm.txt", FA_OPEN_EXISTING | FA_READ); //open the existed file
 if(frslt == FR_OK){
 // f_lseek(&OurFile,0); //pointer to the first byte
  while( !f_eof(&file) && ind < sizeof(filebuf) ){
  f_read(&file, (pChar+ind), 1, &nRead);

   if(*(pChar+ind) == '\r'){
     *(pChar+ind) ='\0';
     parseSD_wm(filebuf);  
     ind = 0;
    }
   else{

   if((*(pChar+ind)) > 0x2F)
      ind++;
   }

   
  } 
  f_close(&file);//close the file 
 }
 
 
 f_mount(NULL, "0:", 0);//unmount the drive 
 }
}
taskEXIT_CRITICAL(); 

}

