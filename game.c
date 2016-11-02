#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdbool.h>

void input() {
	
}

void update() {
	
}

void renderGame(int fbfd, char* addr) {
	struct fb_copyarea rect;
	
	rect.dx = 0;
	rect.dy = 0;
	rect.width = 320;
	rect.height = 240;
	
	ioctl(fbfd, 0x4680, &rect);
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");

	int FPS=30;
	double frameTime = 1.0 / FPS;
	struct timeval lastTime;
	gettimeofday(&lastTime, NULL);
	double accumulator = 0.0;

	int frames = 0;
	double frameCounter = 0.0;
	
	int pfd = open("/dev/fb0", O_RDWR); // framebuffer
	char* addr = mmap(NULL, 153600, PROT_WRITE, MAP_PRIVATE, pfd, 0);
	
	bool done = false;
	do {
		struct timeval newTime;
		gettimeofday(&newTime, NULL);
		
		long int ms = ((newTime.tv_sec * 1000000 + newTime.tv_usec) - (lastTime.tv_sec * 1000000 + lastTime.tv_usec));
		double passedTime = ms/1e6;
		lastTime = newTime;
		
		//if (passedTime > 0.10) // avoiding spiral of death
		//	passedTime = 0.10;
		
		accumulator += passedTime;
		frameCounter += passedTime;

		bool render = false;
		while (accumulator >= frameTime)
		{
			render = true;

			accumulator -= frameTime;
			
			//game input
			input();
			//game update
			update();
			
			if (frameCounter >= 1.0)
			{
				printf("FPS: %d\n", frames);

				frames = 0;
				frameCounter = 0.0;
			}
		}

		if (render)
		{
			//render framebuffer
			//renderGame(pfd, addr);
			
			frames++;
		}
	} while(!done);
	
	munmap(addr, 153600);
	close(pfd);
	
	exit(EXIT_SUCCESS);
}
