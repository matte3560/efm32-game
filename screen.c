#include "screen.h"

void init_screen() {
	fb = open("/dev/fb0", O_RDWR); // open framebuffer
	addr = mmap(0, SCREENSIZE_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
	
	memset(addr, 0, SCREENSIZE_BYTES); //clear screen get rid of the penguin Tux
	//add border
	for(int x=0; x < SCREEN_WIDTH; x++) {
		addr[200*SCREEN_WIDTH+x]=0b1111111111111111;
	}
	int number0[875] = {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0};

	for(int j = 0; j < 35; j++) {
		for(int i =0; i < 25; i++) {
			if(number0[j*25+i]==1) {
				addr[(205+j)*SCREEN_WIDTH+(40+i)]=0b0000000000011111;
			}
		}
	}
	
	struct fb_copyarea rect;
	
	rect.dx = 0;
	rect.dy = 0;
	rect.width = SCREEN_WIDTH;
	rect.height = SCREEN_HEIGHT;
	
	ioctl(fb, 0x4680, &rect);
}

void drawball(int x, int y, int size, int lastx, int lasty, int* ball, uint16_t color) {
	int i;
	int j;
	
	for(j = 0; j < size; j++) {
		for(i = 0; i < size; i++) {
			
			if(ball[j*size+i]==1) {
				int index = (y+j)*SCREEN_WIDTH+(x+i);
				if(index >= 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
					addr[index]|=0b0000011111100000;
			}
		}
	}
	
	struct fb_copyarea rect;
	
	rect.dx = (x < lastx) ? x : lastx;
	rect.dy = (y < lasty) ? y : lasty;
	rect.width = ((x > lastx) ? x : lastx)+(size+1) - rect.dx;
	rect.height = ((y > lasty) ? y : lasty)+(size+1) - rect.dy;
	
	ioctl(fb, 0x4680, &rect);
}

void drawrect(int x, int y, int width, int height, int lastx, int lasty, uint16_t color) {
	int i;
	int j;
	
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			int index = (y+j)*SCREEN_WIDTH+(x+i);
			if(index >= 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
				addr[index]|=color;
		}
	}
	
	struct fb_copyarea rect;
	
	rect.dx = (x < lastx) ? x : lastx;
	rect.dy = (y < lasty) ? y : lasty;
	rect.width = ((x > lastx) ? x : lastx)+(width+1) - rect.dx;
	rect.height = ((y > lasty) ? y : lasty)+(height+1) - rect.dy;
	
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