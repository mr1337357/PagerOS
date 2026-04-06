extern int (*do_syscall)(int callnum, void *arg);

int strlen(char *msg);

void print(char *message);

int write(int fd, char *buff, int len);

int read(int fd, char *buff, int len);

void delay(int ms);

void event_handler(int event, void *arg);
