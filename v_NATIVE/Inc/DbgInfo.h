#ifndef DBGINFO_H
#define DBGINFO_H
#include <stdint.h>

typedef volatile struct {
	uint16_t _dbg_good_pulses;
	//@@@ uint16_t _dbg_leftover;
	uint32_t _dbg_silence_counter;
	uint32_t _dbg_pulse_dur;
	uint32_t _dbg_total_pulses;
	uint16_t _dbg_total_cash;
	uint16_t _dbg_curr_state;
} DbgInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t cmp_(char *v1, char *v2, int n);
void cpy_(char *from, char *to, int n);

DbgInfo_t * getDbgInfo(void);
uint8_t isDbgInfoChanged(void);

#ifdef __cplusplus
}
#endif

#endif /* DBGINFO_H */
