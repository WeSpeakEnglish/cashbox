#ifndef SIMCOM_DRIVE_H
#define SIMCOM_DRIVE_H
#ifdef __cplusplus
extern "C"{
#endif
 void SIM800_init(void);
 void SIM800_queue_handler(void);
 void SIM800_Show(void);
 void SIM800_submit_washing(uint8_t w_num, uint16_t cost);
 void SIM800_submit_collection(void);

#ifdef __cplusplus
}
#endif
#endif