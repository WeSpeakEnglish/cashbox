#ifndef _SD_FILES
#define _SD_FILES
void SD_GetData(void);     // get data like tarifs and exits/inputs for drive slave
void SD_SetData(void);     // set data like tarifs and exits/inputs for drive slave
void SD_GetID(void);       // we get our ID of terminal
void SD_SetSession(void);  // we get/set session with PWD, CASHBOX and the counters
void SD_GetSession(void);
#endif