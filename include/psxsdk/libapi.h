#ifndef LIBAPI_H
#define LIBAPI_H

struct EXEC {
    unsigned long pc0;
    unsigned long gp0;
    unsigned long t_addr;
    unsigned long t_size;
    unsigned long d_addr;
    unsigned long d_size;
    unsigned long b_addr;
    unsigned long b_size;
    unsigned long s_addr;
    unsigned long s_size;
    unsigned long sp, fp, gp, ret, base;
};

extern void InitCARD(long val);
extern long StartCARD(void);
extern long StopCARD(void);
extern void _bu_init(void);
extern long _card_info(long chan);
extern long _card_clear(long chan);
extern long _card_load(long chan);
extern long _card_auto(long val);
extern void _new_card(void);
extern long _card_status(long drv);
extern long _card_wait(long drv);
extern unsigned long _card_chan(void);
extern long _card_write(long chan, long block, unsigned char* buf);
extern long _card_read(long chan, long block, unsigned char* buf);

extern void InitHeap(unsigned long*, unsigned long);
extern long Load(char*, struct EXEC*);
extern long Exec(struct EXEC*, long, char**);
// GPU_cw
extern void _bu_init(void);

/*
 * Opens a device for low-level input/output and returns
 * the descriptor. Returns -1 on failure.
 */
int open(const char* __file, int __oflag);

extern long lseek(long, long, long);

/*
 * Reads n bytes from the descriptor
 * fd to the are specified by buf.
 * Returns the actual number of bytes
 * read into the are. An error returns -1
 */
extern long read(long fd,   // File descriptor
                 void* buf, // Pointer to read buffer address
                 long n     // Number of bytes to read
);
extern long write(long, void*, long);

/*
 * Close releases the file descriptor.
 * fd is returned when successful, and -1 in all other cases.
 */
extern int close(int fd // File descriptor
);

/*
 * Initializes the file system
 * Returns 1 if successful, 0 in all other cases.
 */
extern long format(char* fs // File system name
);

extern struct DIRENTRY* firstfile(char*, struct DIRENTRY*);
extern struct DIRENTRY* nextfile(struct DIRENTRY*);
extern long erase(char*);
extern long Krom2RawAdd(unsigned long);
extern void ChangeClearPAD(long);
extern void StopPAD(void);
extern void FlushCache(void);
extern void DeliverEvent(unsigned long, unsigned long);

extern long OpenEvent(unsigned long, long, long, long (*func)());
extern long CloseEvent(long);
extern long WaitEvent(long);
extern long TestEvent(long);
extern long EnableEvent(long);
extern long DisableEvent(long);
extern void DeliverEvent(unsigned long, unsigned long);
extern void UnDeliverEvent(unsigned long, unsigned long);

// ChangeClearRCnt
extern void _96_remove(void);
extern long SetRCnt(unsigned long, unsigned short, long);
extern long StartRCnt(unsigned long);

#endif
