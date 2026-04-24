
int PCinit(void);
int PCopen(char* name, int flags, int perms);
int PCcreat(char* name, int perms);
int PClseek(int fd, int offset, int mode);
int PCread(int fd, char* buff, int len);
int PCwrite(int fd, char* buff, int len);
int PCclose(int fd);
