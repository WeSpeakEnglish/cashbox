#include "DbgInfo.h"

DbgInfo_t *getDbgInfo(void)
{
	static DbgInfo_t dbg = { 0 };
	return &dbg;
}

uint8_t cmp_(char *v1, char *v2, int n)
{
  while(*v1 == *v2)
  { if (--n <= 0) return 1; ++v1; ++v2; }
  return 0;
}

void cpy_(char *from, char *to, int n)
{
  while(n--) *to++ = *from++;
}

uint8_t isDbgInfoChanged(void)
{
	static DbgInfo_t dbg_cpy = { 0 };
	DbgInfo_t *p_dbg = getDbgInfo();
	if ( cmp_(
	  (char *)p_dbg,
	  (char *)(&dbg_cpy),
	  sizeof(DbgInfo_t)
	) )
		return 0;
	else
		cpy_(
		  (char *)p_dbg,
		  (char *)(&dbg_cpy),
		  sizeof(DbgInfo_t)
		);
	return 1;
}
