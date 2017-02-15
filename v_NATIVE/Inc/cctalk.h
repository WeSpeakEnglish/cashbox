#ifndef _CCTALK_H
#define _CCTALK_H



#define BUFLEN_CC 32

typedef struct{
  uint8_t buffer[BUFLEN_CC]; // easy without twice buffering awaiting while ready
  uint8_t readyBuff; // buffer is ready
  uint16_t IndWR;
}CC_struct;

enum{CC_OPEN = 0, CC_MASTER_READY, CC_REQUEST, CC_CLOSE, CC_SIMPLEPOOL, CC_DISABLEESCROW};

extern CC_struct ccTalk;

uint8_t ccTalkParseOK(void);
void ccTalkParseStatus(void);
void ccTalkSendCMD(uint8_t Des);

#endif