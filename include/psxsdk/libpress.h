extern void DecDCTReset(int mode);
void DecDCTin(u_long* runlevel, int mode);
void DecDCTout(u_long* cell, int size);
void (*DecDCToutCallback(void (*func)(void)))(void);
int DecDCTvlc(u_long* bs, u_long* buf);
