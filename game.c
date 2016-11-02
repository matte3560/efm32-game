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
#include <string.h>
#include <stdint.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREENSIZE_BYTES 153600

typedef struct {
  float x, y;
} vec2;

vec2 add(vec2 a, vec2 b) {
	vec2 ans;
	ans.x = a.x+b.x;
	ans.y = a.y+b.y;
	
	return ans;
}

vec2 mul(vec2 a, float b) {
	vec2 ans;
	ans.x = a.x*b;
	ans.y = a.y*b;
	
	return ans;
}

vec2 ballPos;
vec2 ballPosLastRender;
vec2 ballDir;
vec2 ballSize;
float ballSpeed;

void initGame() {
	/* INIT BALL */
	ballPos.x = SCREEN_WIDTH/2;
	ballPos.y = SCREEN_HEIGHT/2;
	ballPosLastRender = ballPos;
	ballDir.x=0.5f;
	ballDir.y=0.5f;
	ballSize.x = 10.0f;
	ballSize.y = 10.0f;
	
	ballSpeed = 25.0f;
	/* INIT BALL */
	
	/* INIT LEFT PLAYER */
	/* INIT LEFT PLAYER */
	
	/* INIT RIGHT PLAYER */
	/* INIT RIGHT PLAYER */
}

void input() { // update player positions
	
}

void update(float dt) { // update ball position
	ballPos = add(ballPos, mul(ballDir, ballSpeed*dt));
	
	if(ballPos.x <= 0)
		ballPos.x = -ballPos.x;
	else if(ballPos.x+ballSize.x >= SCREEN_WIDTH-1)
		ballPos.x = -ballPos.x;
	
	if(ballPos.y <= 0)
		ballPos.y = -ballPos.y;
	else if(ballPos.y+ballSize.y >= SCREEN_HEIGHT-1)
		ballPos.y = -ballPos.y;
}

void renderGame(int fbfd, uint16_t* addr) {
	memset(addr, 0, SCREENSIZE_BYTES); // clear buffer
	
	/* RENDER BALL */
	int xpos = (int)ballPos.x;
	int ypos = (int)ballPos.y;
	
	int y;
	int x;
	for(y = 0; y < ballSize.y; y++) {
		for(x = 0; x < ballSize.x; x++) {
			int index = (ypos+y)*SCREEN_WIDTH+(xpos+x);
			if(index > 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
				addr[index]|=0b1111100000111111;
		}
	}
	struct fb_copyarea rect;
	
	rect.dx = (ballPos.x < ballPosLastRender.x) ? ballPos.x : ballPosLastRender.x;
	rect.dy = (ballPos.y < ballPosLastRender.y) ? ballPos.y : ballPosLastRender.y;
	rect.width = ((ballPos.x > ballPosLastRender.x) ? ballPos.x : ballPosLastRender.x)+5 - rect.dx;
	rect.height = ((ballPos.y > ballPosLastRender.y) ? ballPos.y : ballPosLastRender.y)+5 - rect.dy;
	
	ioctl(fbfd, 0x4680, &rect);
	
	ballPosLastRender=ballPos;
	/* RENDER BALL */
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");

	initGame();
	
	int FPS=30;
	double frameTime = 1.0 / FPS;
	struct timeval lastTime;
	gettimeofday(&lastTime, NULL);
	double accumulator = 0.0;

	int frames = 0;
	double frameCounter = 0.0;
	
	int pfd = open("/dev/fb0", O_RDWR); // framebuffer
	uint16_t* addr = mmap(NULL, SCREENSIZE_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, pfd, 0);
	
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
			update((float)frameTime);
			
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
			renderGame(pfd, addr);
			
			frames++;
		}
	} while(!done);
	
	munmap(addr, SCREENSIZE_BYTES);
	close(pfd);
	
	exit(EXIT_SUCCESS);
}
