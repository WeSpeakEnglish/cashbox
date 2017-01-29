#include "SIMCOM.hpp"
#include "usart.h"
#include "SIMCOM_drive.hpp"

SIMCOM Sim800(huart2, 19200);

#ifdef __cplusplus
extern "C"{
#endif

void SIM800_init(void){
 Sim800.init();
}
void SIM800_queue_handler(void){
 Sim800.queue_handler();
}
void SIM800_Show(void){
 Sim800.Show();
}

void SIM800_submit_washing(uint8_t w_num, uint16_t cost){
 Sim800.submit_washing(w_num, cost);
}

void SIM800_submit_collection(void){
 Sim800.submit_collection();
}
 
#ifdef __cplusplus
}
#endif