#include "sd_files.h"
#include "diskio.h"
#include "ff_gen_drv.h"
#include "fatfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
 
void SD_GetData(void){ 
  
  FRESULT frslt;
  FATFS filesystem;
  DIR cwd;
  FILINFO fileInfo;
  uint8_t is_file_found = 0;
  int nFiles = 0;
  char filebuf[64];  
  FIL file;
  UINT nRead, nWritten;
 osDelay(1000); 
taskENTER_CRITICAL(); 
{
 frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
 frslt = f_open(&file, "wm.txt", FA_OPEN_EXISTING | FA_READ); //open the existed file
 if(frslt == FR_OK)
 f_close(&file);//close the file

 f_mount(NULL, "0:", 0);//unmount the drive 
}
taskEXIT_CRITICAL(); 

}

