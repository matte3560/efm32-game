#include "screen.h"

void init_screen() {
	fb = open("/dev/fb0", O_RDWR); // open framebuffer
	addr = mmap(0, SCREENSIZE_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
	
	memset(addr, 0, SCREENSIZE_BYTES); //clear screen get rid of the penguin Tux
	struct fb_copyarea rect;
	
	rect.dx = 0;
	rect.dy = 0;
	rect.width = SCREEN_WIDTH;
	rect.height = SCREEN_HEIGHT;
	
	ioctl(fb, 0x4680, &rect);
}

void clear_screenbuffer() {
	memset(addr, 0, SCREENSIZE_BYTES); // clear buffer
}

void close_screen() {
	//close framebuffer
	munmap(addr, SCREENSIZE_BYTES);
	close(fb);
}