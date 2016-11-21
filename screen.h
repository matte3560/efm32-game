#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 201
#define SCREENSIZE_BYTES 153600

int fb;
uint16_t* addr;

void init_screen();
void drawball(int x, int y, int size, int lastx, int lasty, int* ball, uint16_t color);
void drawrect(int x, int y, int width, int height, int lastx, int lasty, uint16_t color);
void clear_screenbuffer();

void close_screen();
