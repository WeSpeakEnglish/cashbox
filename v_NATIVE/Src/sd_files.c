#include "sd_files.h"
#include "diskio.h"
#include "ff_gen_drv.h"
#include "fatfs.h"

#include "simcom.h"
#include "stdlib.h"
#include "calculations.h"
#include <string.h>
#include "stm32f3xx_hal.h"
#include "core.h"
#include "vend.h"

void SD_LedOn(uint8_t OnOff) {
    if (OnOff)
        HAL_GPIO_WritePin(SD_LED_GPIO_Port, SD_LED_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SD_LED_GPIO_Port, SD_LED_Pin, GPIO_PIN_RESET);
}

uint8_t SD_Detect(void) {
    if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_RESET) {
        SD_LedOn(1);
        return 1;
    }
    else {
        SD_LedOn(0);
        return 0;
    }
}

void parseSD_wm(char * pData) { // we parse string like wm=150
    uint16_t i,j = 0;
    if (pData[0] == 'w')
        if (pData[1] == 'm')
            if ((pData[2] > 0x30) && (pData[2] < (0x30 + MAX_WASHINGS + 1))) { // MAX_WASHINGS
                i = pData[2] - 0x30 - 1;
            //    WL[wm].index[i] = (i + 1); //existed index
                if (pData[3] == '=') {
                    pData = &pData[4];
                    j = 4;
                    while(pData[j] > 0x2F){
                      j++;
                    }
                    j--;
                    WL[i].start_button_pin = atoi(&pData[j]);
                    pData[j] = '\0';
                    j--;
                    WL[i].send_signal_relay = atoi(&pData[j]);
                    pData[j] = '\0';
                    WL[i].price = atoi(pData);

                }
            }
}

void SD_SetData(void) { // data has got from the server, and we need to write
    FRESULT frslt;
    FATFS filesystem;
    char filebuf[8];
    char * pChar = &filebuf[0];
    FIL file;
    UINT nWrite, i = 0;
    if (SD_Detect()) {
            frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
            if (frslt == FR_OK) {
                frslt = f_open(&file, "wm.txt", FA_WRITE | FA_CREATE_ALWAYS); //open the existed file
                if (frslt == FR_OK) {
                    for (i = 0; i < MAX_WASHINGS; i++) {
                       // if (VendPrices.index[i]) { // if it exist
                            f_write(&file, "wm", 2, &nWrite);
                            Utoa(i + 1, pChar);
                            f_write(&file, pChar, 1, &nWrite);
                            f_write(&file, "=", 1, &nWrite);
                            Utoa(WL[i].price, pChar);
                            f_write(&file, pChar, strlen(pChar), &nWrite);
                            f_write(&file, ",", 1, &nWrite);
                            Utoa(WL[i].send_signal_relay, pChar);
                            f_write(&file, pChar, strlen(pChar), &nWrite);
                            f_write(&file, ",", 1, &nWrite);
                            Utoa(WL[i].start_button_pin, pChar);
                            f_write(&file, pChar, strlen(pChar), &nWrite);          
                            f_write(&file, "\r\n", 2, &nWrite);

                   //     }
                    }
                    f_close(&file); //close the file 
                }
                f_mount(NULL, "0:", 0); //unmount the drive 
            }
    }
    SD_LedOn(0);
}

void SD_GetData(void) {

    FRESULT frslt;
    FATFS filesystem;
    char filebuf[32];
    char * pChar = &filebuf[0];
    FIL file;
    UINT nRead, i = 0;
    if (SD_Detect()) {
            frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
            if (frslt == FR_OK) {
              frslt = f_open(&file, "wm.txt", FA_OPEN_EXISTING | FA_READ); //open the existed file
                if (frslt == FR_OK) {
                    while (!f_eof(&file) && i < sizeof (filebuf)) {
                        f_read(&file, (pChar + i), 1, &nRead);

                        if (*(pChar + i) == '\r') {
                            *(pChar + i) = '\0';
                            parseSD_wm(filebuf);
                            i = 0;
                        } else {
                            if ((*(pChar + i)) > 0x2F)
                                i++;
                        }
                    }
                    f_close(&file); //close the file 
                }
                f_mount(NULL, "0:", 0); //unmount the drive 
            }
 //LOOK           

    }
    SD_LedOn(0);
}

void SD_GetID(void){
     FRESULT frslt;
    FATFS filesystem;
    char filebuf[32];
    uint8_t State = 0;
    char * pChar = &filebuf[0];
    char * pItoa = &filebuf[0]; // initial position of number
    FIL file;
    UINT nRead, i = 0;
    if (SD_Detect()) {
            frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
            if (frslt == FR_OK) {
              frslt = f_open(&file, "id.txt", FA_OPEN_EXISTING | FA_READ); //open the existed file
                 if (frslt == FR_OK) {
                    while (!f_eof(&file) && i < sizeof (filebuf)) {
                        f_read(&file, (pChar + i), 1, &nRead);
                        switch(State){
                            case 0:
                              if (*(pChar + i) == 'i')
                                State++;
                              break;
                            case 1:
                              if (*(pChar + i) == 'd')
                                State++;
                              break; 
                            case 2: 
                              if (*(pChar + i) == '=')
                                State++;
                              break;
                            case 3:  
                              if ((*(pChar + i) > 0x2F) && (*(pChar + i) < 0x3A)){
                                State++;
                                pItoa = (pChar + i);
                              }
                              break;  
                        }
                       i++;   
                }
                if(pItoa != &filebuf[0]){  //was set
                  terminal_UID = atoi(pItoa);
                }
               
                f_close(&file); //close the file 
                f_mount(NULL, "0:", 0); //unmount the drive 
            }
            }
    }
  SD_LedOn(0);
}

void SD_GetSession(void){
     FRESULT frslt;
    FATFS filesystem;
    char filebuf[WASHERS_MAX_COUNT*4];
    uint8_t State = 0;
    char * pChar = &filebuf[0];
    FIL file;
    UINT nRead, i = 0;
    if (SD_Detect()) {
            frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
            if (frslt == FR_OK) {
              frslt = f_open(&file, "session.txt", FA_OPEN_EXISTING | FA_READ); //open the existed file
                 if (frslt == FR_OK) {
                    while (!f_eof(&file) && i < sizeof (filebuf)) {
                        f_read(&file, (pChar + i), 1, &nRead);
                        switch(State){
                            case 0:
                              if (i == sizeof(Session)-1){
                                pChar = &filebuf[0];
                                i = 0;
                                State++;
                                memcpy((void *)&Session, (const void * )pChar, sizeof(Session)); 
                              }
                              break;
                            case 1:
                              if (i == sizeof(UserCounter)-1){
                                pChar = &filebuf[0];
                                i = 0;
                                State++;
                                memcpy((void *)&UserCounter, (const void * )pChar, sizeof(UserCounter)); 
                              }
                              break; 
                            case 2: 
                              if (i == sizeof(CashBOX)-1){
                                pChar = &filebuf[0];
                                i = 0;
                                State++;
                                memcpy((void *)&CashBOX, (const void * )pChar, sizeof(CashBOX)); 
                              }
                              break;
                            case 3:  
                              if (i == sizeof(Password)-1){
                                pChar = &filebuf[0];
                                i = 0;
                                State++;
                                memcpy((void *)&Password, (const void * )pChar, sizeof(Password)); 
                              }
                              break;  
                        }
                       i++;   
                }
  
                f_close(&file); //close the file 
                f_mount(NULL, "0:", 0); //unmount the drive 
            }
            }
    }
  SD_LedOn(0);
}

void SD_SetSession(void){
    FRESULT frslt;
    FATFS filesystem;
    FIL file;
    UINT nWrite;
    if (SD_Detect()) {
            frslt = f_mount(&filesystem, "0:", 1); //mount the drive 
            if (frslt == FR_OK) {
                frslt = f_open(&file, "session.txt", FA_WRITE | FA_CREATE_ALWAYS); //open the existed file
                if (frslt == FR_OK) {
                           f_write(&file, (const void *) &Session, sizeof(Session), &nWrite);
                           f_write(&file, (const void *) &UserCounter, sizeof(UserCounter), &nWrite);
                           f_write(&file, (const void *) &CashBOX, sizeof(CashBOX), &nWrite);
                           f_write(&file, (const void *) &Password, sizeof(Password), &nWrite);

                   }
                    f_close(&file); //close the file 
                }
                f_mount(NULL, "0:", 0); //unmount the drive 
            }
    
    SD_LedOn(0);
return;
}
