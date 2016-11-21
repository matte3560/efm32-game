#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int driver;
bool SW2;
bool SW4;
bool SW6;
bool SW8;
void interrupt_handler(int n, siginfo_t *info, void *unused); //interrupt function prototype
void init_input();
void close_input();