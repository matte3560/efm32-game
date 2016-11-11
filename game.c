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
#include <math.h>
#include <signal.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREENSIZE_BYTES 153600

void interrupt_handler(); //interrupt function prototype

typedef struct {
  float x, y;
} vec2;

float length(vec2 a) {
	return sqrtf(a.x*a.x+a.y*a.y);
}

vec2 add(vec2 a, vec2 b) {
	vec2 ans;
	ans.x = a.x+b.x;
	ans.y = a.y+b.y;
	
	return ans;
}

vec2 sub(vec2 a, vec2 b) {
	vec2 ans;
	ans.x = a.x-b.x;
	ans.y = a.y-b.y;
	
	return ans;
}

vec2 mul(vec2 a, float b) {
	vec2 ans;
	ans.x = a.x*b;
	ans.y = a.y*b;
	
	return ans;
}

vec2 normalize(vec2 a) {
	vec2 ans;
	float len = length(a);
	ans.x = a.x / len;
	ans.y = a.y / len;

	return ans;
}

// ball variables
vec2 ballPos;
vec2 ballPosLastRender;
vec2 ballDir;
int ballSizeX;
int ballSizeY;
float ballSpeed;

// player left variables
vec2 playerLeftPos;
vec2 playerLeftLastRender;
int playerLeftSizeX;
int playerLeftSizeY;
float playerLeftSpeed;

// player right variables
vec2 playerRightPos;
vec2 playerRightLastRender;
int playerRightSizeX;
int playerRightSizeY;
float playerRightSpeed;

void initGame(int fbfd, uint16_t* addr, FILE* driver) {
	
	/* INIT SCREEN */
	memset(addr, 0, SCREENSIZE_BYTES); //clear screen get rid of the penguin Tux
	struct fb_copyarea rect;
	
	rect.dx = 0;
	rect.dy = 0;
	rect.width = SCREEN_WIDTH;
	rect.height = SCREEN_HEIGHT;
	
	ioctl(fbfd, 0x4680, &rect);
	/* INIT SCREEN */
	
	/* INIT GAMEPAD */
	/*driver = fopen("/dev/gamepad", "rb");
	if (!driver) {
        printf("driver open error\n");
		exit(1);
    }
    if (signal(SIGIO, &interrupt_handler) == SIG_ERR) {
        printf("interrupt handler error\n");
        exit(1);
    }
    if (fcntl(fileno(driver), F_SETOWN, getpid()) == -1) {
        printf("PID error.\n");
        exit(1);
    }
    long oflags = fcntl(fileno(driver), F_GETFL);
    if (fcntl(fileno(driver), F_SETFL, oflags | FASYNC) == -1) {
        printf("FASYNC error\n");
        exit(1);
    }*/
	/* INIT GAMEPAD */
	
	srand(time(0)); // seed the random number generator
	
	/* INIT BALL */
	ballSizeX = 11;
	ballSizeY = 11;
	ballPos.x = SCREEN_WIDTH/2-(ballSizeX/2.0f);
	ballPos.y = SCREEN_HEIGHT/2-(ballSizeY/2.0f);
	ballPosLastRender = ballPos;
	int dir = 2*(rand()%2)-1;
	float angle = ((rand()/(float)RAND_MAX)*2 - 1)*(M_PI/6);
	ballDir.x=dir*cosf(angle);
	ballDir.y=-sinf(angle);
	ballDir = normalize(ballDir); // normalized direction vector
	ballSpeed = 300.0f;
	/* INIT BALL */
	
	/* INIT LEFT PLAYER */
	playerLeftSizeX = 10.0f;
	playerLeftSizeY = 40.0f;
	playerLeftPos.x = 10.0f;
	playerLeftPos.y = (SCREEN_HEIGHT/2)-(playerLeftSizeY/2);
	playerLeftLastRender = playerLeftPos;
	playerLeftSpeed = 50.0f;
	/* INIT LEFT PLAYER */
	
	/* INIT RIGHT PLAYER */
	playerRightSizeX = 10.0f;
	playerRightSizeY = 40.0f;
	playerRightPos.x = SCREEN_WIDTH-playerRightSizeX-11.0f;
	playerRightPos.y = (SCREEN_HEIGHT/2)-(playerRightSizeY/2);
	playerRightLastRender = playerRightPos;
	playerRightSpeed = 50.0f;
	/* INIT RIGHT PLAYER */
}

void input() { // update player positions
	
}

void update(float dt) { // update ball position
	ballPos = add(ballPos, mul(ballDir, ballSpeed*dt));
	
	/* WIN LOGIC */
	if(ballPos.x <= 0) {
		ballPos.x = 0;
		ballDir.x = -ballDir.x;
	} else if(ballPos.x+ballSizeX >= SCREEN_WIDTH-1) {
		ballPos.x = SCREEN_WIDTH - ballSizeX - 1;
		ballDir.x = -ballDir.x;
	}
	/* WIN LOGIC */
	
	/* TOP AND BOTTOM SCREEN COLLISION */
	if(ballPos.y <= 0) {
		ballPos.y = 0;
		ballDir.y = -ballDir.y;
	} else if(ballPos.y+ballSizeY >= SCREEN_HEIGHT-1) {
		ballPos.y = SCREEN_HEIGHT - ballSizeY - 1;
		ballDir.y = -ballDir.y;
	}
	/* TOP AND BOTTOM SCREEN COLLISION */
	
	/* PLAYER LEFT COLLISION */
	if(	ballPos.x <= playerLeftPos.x+playerLeftSizeX &&
	     (ballPos.y+ballSizeY) >= playerLeftPos.y &&
	     ballPos.y <= playerLeftPos.y + playerLeftSizeY) {
	     ballPos.x = playerLeftPos.x+playerLeftSizeX;
	     float relativeIntersectY = (playerLeftPos.y+(playerLeftSizeY/2.0f)) - (ballPos.y+(ballSizeY/2.0f));
	     
	     float normalizedRelativeIntersectionY = (relativeIntersectY/(playerLeftSizeY/2.0f));
	     float bounceAngle = normalizedRelativeIntersectionY * (M_PI/4); //max 45 degree bounce
	     ballDir.x = cosf(bounceAngle);
	     ballDir.y = -sinf(bounceAngle);
	     ballDir = normalize(ballDir);
	}
	/* PLAYER LEFT COLLISION */
	
	/* PLAYER RIGHT COLLISION */
	if(	ballPos.x + ballSizeX >= playerRightPos.x &&
	     (ballPos.y+ballSizeY) >= playerRightPos.y &&
	     ballPos.y <= playerRightPos.y + playerRightSizeY) {
	     ballPos.x = playerRightPos.x-ballSizeX;
	     float relativeIntersectY = (playerRightPos.y+(playerRightSizeY/2.0f)) - (ballPos.y+(ballSizeY/2.0f));
	     
	     float normalizedRelativeIntersectionY = (relativeIntersectY/(playerRightSizeY/2.0f));
	     float bounceAngle = normalizedRelativeIntersectionY * (M_PI/4); //max 45 degree bounce
	     ballDir.x = -cosf(bounceAngle);
	     ballDir.y = -sinf(bounceAngle);
	     ballDir = normalize(ballDir);
	}
	/* PLAYER RIGHT COLLISION */
}

void renderGame(int fbfd, uint16_t* addr) {
	memset(addr, 0, SCREENSIZE_BYTES); // clear buffer
	struct fb_copyarea rect;
	int y;
	int x;
	
	/* RENDER BALL */
	int ballxpos = (int)ballPos.x;
	int ballypos = (int)ballPos.y;
	
	for(y = 0; y < ballSizeY; y++) {
		for(x = 0; x < ballSizeX; x++) {
			vec2 pixelfromcenter;
			pixelfromcenter.x = x-ballSizeX/2;
			pixelfromcenter.y = y-ballSizeY/2;
			if(length(pixelfromcenter) <= ballSizeX/2) {
				int index = (ballypos+y)*SCREEN_WIDTH+(ballxpos+x);
				if(index >= 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
					addr[index]|=0b0000011111100000;
			}
		}
	}
	
	rect.dx = (ballxpos < ballPosLastRender.x) ? ballxpos : ballPosLastRender.x;
	rect.dy = (ballypos < ballPosLastRender.y) ? ballypos : ballPosLastRender.y;
	rect.width = ((ballxpos > ballPosLastRender.x) ? ballxpos : ballPosLastRender.x)+(ballSizeX+1) - rect.dx;
	rect.height = ((ballypos > ballPosLastRender.y) ? ballypos : ballPosLastRender.y)+(ballSizeY+1) - rect.dy;
	
	ioctl(fbfd, 0x4680, &rect);
	
	ballPosLastRender.x=ballxpos;
	ballPosLastRender.y=ballypos;
	/* RENDER BALL */
	
	/* RENDER PLAYER LEFT */
	int playerleftposx = (int)playerLeftPos.x;
	int playerleftposy = (int)playerLeftPos.y;
	
	for(y = 0; y < playerLeftSizeY; y++) {
		for(x = 0; x < playerLeftSizeX; x++) {
			int index = (playerleftposy+y)*SCREEN_WIDTH+(playerleftposx+x);
			if(index >= 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
				addr[index]|=0b0000000000011111;
		}
	}
	
	rect.dx = (playerleftposx < playerLeftLastRender.x) ? playerleftposx : playerLeftLastRender.x;
	rect.dy = (playerleftposy < playerLeftLastRender.y) ? playerleftposy : playerLeftLastRender.y;
	rect.width = ((playerleftposx > playerLeftLastRender.x) ? playerleftposx : playerLeftLastRender.x)+(playerLeftSizeX+1) - rect.dx;
	rect.height = ((playerleftposy > playerLeftLastRender.y) ? playerleftposy : playerLeftLastRender.y)+(playerLeftSizeY+1) - rect.dy;
	
	ioctl(fbfd, 0x4680, &rect);
	
	playerLeftLastRender.x=playerleftposx;
	playerLeftLastRender.y=playerleftposy;
	/* RENDER PLAYER LEFT */
	
	/* RENDER PLAYER RIGHT */
	int playerrightposx = (int)playerRightPos.x;
	int playerrightposy = (int)playerRightPos.y;
	
	for(y = 0; y < playerRightSizeY; y++) {
		for(x = 0; x < playerRightSizeX; x++) {
			int index = (playerrightposy+y)*SCREEN_WIDTH+(playerrightposx+x);
			if(index >= 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
				addr[index]|=0b1111100000000000;
		}
	}
	
	rect.dx = (playerrightposx < playerRightLastRender.x) ? playerrightposx : playerRightLastRender.x;
	rect.dy = (playerrightposy < playerRightLastRender.y) ? playerrightposy : playerRightLastRender.y;
	rect.width = ((playerrightposx > playerRightLastRender.x) ? playerrightposx : playerRightLastRender.x)+(playerRightSizeX+1) - rect.dx;
	rect.height = ((playerrightposy > playerRightLastRender.y) ? playerrightposy : playerRightLastRender.y)+(playerRightSizeY+1) - rect.dy;
	
	ioctl(fbfd, 0x4680, &rect);
	
	playerRightLastRender.x=playerrightposx;
	playerRightLastRender.y=playerrightposy;
	/* RENDER PLAYER RIGHT */
}

void interrupt_handler(int signalnr) //interrupt function
{
    printf("Signal nr.: %d\n", signalnr);
	//Now read from driver
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
	
	int pfd = open("/dev/fb0", O_RDWR); // open framebuffer
	uint16_t* addr = mmap(NULL, SCREENSIZE_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, pfd, 0);
	FILE* driver;
	
	initGame(pfd, addr, driver);
	
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
	
	fclose(driver); // close driver
	
	//close framebuffer
	munmap(addr, SCREENSIZE_BYTES);
	close(pfd);
	
	exit(EXIT_SUCCESS);
}
