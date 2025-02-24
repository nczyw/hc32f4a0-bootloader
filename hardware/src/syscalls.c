#include <sys/stat.h>
#include <unistd.h>

int _close(int file) { return -1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _read(int file, char *ptr, int len) { return 0; }
int _fstat(int file, struct stat *st) { return 0; }
int _isatty(int file) { return 1; }

int _getpid_r(struct _reent *reent) { return 1; }

int _kill_r(struct _reent *reent, pid_t pid, int sig) {return -1;}