#ifndef _CCTALK_H
#define _CCTALK_H



#define BUFLEN_CC 64

typedef struct{
 volatile uint8_t  buffer[BUFLEN_CC];    // easy without twice buffering awaiting while ready
 volatile uint8_t  readyBuff;            // buffer is ready
 volatile uint8_t  readySymbols;         // number of symbols in the buffer
 volatile uint16_t IndWR;               //current write index
 volatile uint8_t  TransferFlag;
 volatile uint8_t  gotMoney;
 volatile uint8_t  opCount;
}CC_struct;

enum{CC_OPEN = 0, CC_MASTER_READY, CC_REQUEST, CC_CLOSE, CC_SIMPLEPOOL, CC_DISABLEESCROW, CC_READBUFFEREDBILL, CC_REQ_ACC_COUNT, CC_REQ_IND_ACC_COUNT};

extern CC_struct ccTalk;

uint8_t ccTalkParseOK(void);
void ccTalkParseStatus(void);
void ccTalkSendCMD(uint8_t Des);
uint8_t ccTalkChecksum(uint8_t * pBuff, uint8_t size);
void ccTalkParseAccCount(void);

#endif