#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREENSIZE_BYTES 153600

int fb;
uint16_t* addr;

void init_screen();
void close_screen();