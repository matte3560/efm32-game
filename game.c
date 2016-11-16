#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
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

int driver;
void interrupt_handler(int n, siginfo_t *info, void *unused); //interrupt function prototype

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

bool SW2=false;
bool SW4=false;
bool SW6=false;
bool SW8=false;

// ball variables
vec2 ballPos;
vec2 ballPosLastRender;
vec2 ballDir;
int ballSize;
float ballSpeed;
int *ball;

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

void initGame(int fbfd, uint16_t* addr) {
	
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

	/* setup the signal handler for SIGUSER1 
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */
	struct sigaction sig;
	sig.sa_sigaction = interrupt_handler;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sig, NULL);

	/* INIT GAMEPAD */
	
	srand(time(0)); // seed the random number generator
	
	/* INIT BALL */
	ballSize = 10;
	ballPos.x = SCREEN_WIDTH/2-(ballSize/2.0f);
	ballPos.y = SCREEN_HEIGHT/2-(ballSize/2.0f);
	ballPosLastRender = ballPos;
	// Setting up max 20 degree angle in random direction
	int dir = 2*(rand()%2)-1;
	float angle = ((rand()/(float)RAND_MAX)*2 - 1)*(M_PI/6);
	ballDir.x=dir*cosf(angle);
	ballDir.y=-sinf(angle);
	ballDir = normalize(ballDir); // normalized direction vector
	ballSpeed = 300.0f;
	ball=malloc(ballSize*ballSize*sizeof(int));
	int x;
	for(x=0; x < ballSize; x++){ ball[0*ballSize+x]=0; if(x >= 3 && x <= 6){ ball[0*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[1*ballSize+x]=0; if(x >= 1 && x <= 8){ ball[1*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[2*ballSize+x]=0; if(x >= 1 && x <= 8){ ball[2*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[3*ballSize+x]=0; if(x >= 0 && x <= 9){ ball[3*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[4*ballSize+x]=0; if(x >= 0 && x <= 9){ ball[4*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[5*ballSize+x]=0; if(x >= 0 && x <= 9){ ball[5*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[6*ballSize+x]=0; if(x >= 0 && x <= 9){ ball[6*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[7*ballSize+x]=0; if(x >= 1 && x <= 8){ ball[7*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[8*ballSize+x]=0; if(x >= 1 && x <= 8){ ball[8*ballSize+x]=1; }}
	for(x=0; x < ballSize; x++){ ball[9*ballSize+x]=0; if(x >= 3 && x <= 6){ ball[9*ballSize+x]=1; }}
	
	/* INIT BALL */
	
	/* INIT LEFT PLAYER */
	playerLeftSizeX = 10.0f;
	playerLeftSizeY = 40.0f;
	playerLeftPos.x = 10.0f;
	playerLeftPos.y = (SCREEN_HEIGHT/2)-(playerLeftSizeY/2);
	playerLeftLastRender = playerLeftPos;
	playerLeftSpeed = 150.0f;
	/* INIT LEFT PLAYER */
	
	/* INIT RIGHT PLAYER */
	playerRightSizeX = 10.0f;
	playerRightSizeY = 40.0f;
	playerRightPos.x = SCREEN_WIDTH-playerRightSizeX-11.0f;
	playerRightPos.y = (SCREEN_HEIGHT/2)-(playerRightSizeY/2);
	playerRightLastRender = playerRightPos;
	playerRightSpeed = 150.0f;
	/* INIT RIGHT PLAYER */
}

void input(int driver, float dt) { // update player positions
	if(SW2) {
		playerLeftPos.y-=playerLeftSpeed*dt;
		if(playerLeftPos.y < 0)
			playerLeftPos.y=0;
	}
	if(SW4) {
		playerLeftPos.y+=playerLeftSpeed*dt;
		if(playerLeftPos.y+playerLeftSizeY >= SCREEN_HEIGHT)
			playerLeftPos.y=SCREEN_HEIGHT-playerLeftSizeY-1;
	}
	if(SW6) {
		playerRightPos.y-=playerRightSpeed*dt;
		if(playerRightPos.y < 0)
			playerRightPos.y=0;
	}
	if(SW8) {
		playerRightPos.y+=playerRightSpeed*dt;
		if(playerRightPos.y+playerRightSizeY >= SCREEN_HEIGHT)
			playerRightPos.y=SCREEN_HEIGHT-playerRightSizeY-1;
	}
}

void update(float dt) { // update ball position
	ballPos = add(ballPos, mul(ballDir, ballSpeed*dt));
	
	/* WIN LOGIC */
	bool win=false;
	if(ballPos.x <= playerLeftPos.x) {
		win=true;
	} else if(ballPos.x+ballSize >= playerRightPos.x+playerRightSizeX) {
		win=true;
	}
	if(win) {
		ballPos.x = SCREEN_WIDTH/2-(ballSize/2.0f);
		ballPos.y = SCREEN_HEIGHT/2-(ballSize/2.0f);
		// Setting up max 20 degree angle in random direction
		int dir = 2*(rand()%2)-1;
		float angle = ((rand()/(float)RAND_MAX)*2 - 1)*(M_PI/6);
		ballDir.x=dir*cosf(angle);
		ballDir.y=-sinf(angle);
		ballDir = normalize(ballDir); // normalized direction vector
	}
	/* WIN LOGIC */
	
	/* TOP AND BOTTOM SCREEN COLLISION */
	if(ballPos.y <= 0) {
		ballPos.y = 0;
		ballDir.y = -ballDir.y;
	} else if(ballPos.y+ballSize >= SCREEN_HEIGHT-1) {
		ballPos.y = SCREEN_HEIGHT - ballSize - 1;
		ballDir.y = -ballDir.y;
	}
	/* TOP AND BOTTOM SCREEN COLLISION */
	
	/* PLAYER LEFT COLLISION */
	if(	ballPos.x <= playerLeftPos.x+playerLeftSizeX &&
	     (ballPos.y+ballSize) >= playerLeftPos.y &&
	     ballPos.y <= playerLeftPos.y + playerLeftSizeY) {
	     ballPos.x = playerLeftPos.x+playerLeftSizeX;
	     float relativeIntersectY = (playerLeftPos.y+(playerLeftSizeY/2.0f)) - (ballPos.y+(ballSize/2.0f));
	     
	     float normalizedRelativeIntersectionY = (relativeIntersectY/(playerLeftSizeY/2.0f));
	     float bounceAngle = normalizedRelativeIntersectionY * (M_PI/4); //max 45 degree bounce
	     ballDir.x = cosf(bounceAngle);
	     ballDir.y = -sinf(bounceAngle);
	     ballDir = normalize(ballDir);
	}
	/* PLAYER LEFT COLLISION */
	
	/* PLAYER RIGHT COLLISION */
	if(	ballPos.x + ballSize >= playerRightPos.x &&
	     (ballPos.y+ballSize) >= playerRightPos.y &&
	     ballPos.y <= playerRightPos.y + playerRightSizeY) {
	     ballPos.x = playerRightPos.x-ballSize;
	     float relativeIntersectY = (playerRightPos.y+(playerRightSizeY/2.0f)) - (ballPos.y+(ballSize/2.0f));
	     
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
	
	for(y = 0; y < ballSize; y++) {
		for(x = 0; x < ballSize; x++) {
			
			if(ball[y*ballSize+x]==1) {
				int index = (ballypos+y)*SCREEN_WIDTH+(ballxpos+x);
				if(index >= 0 && index < SCREEN_WIDTH*SCREEN_HEIGHT)
					addr[index]|=0b0000011111100000;
			}
		}
	}
	
	rect.dx = (ballxpos < ballPosLastRender.x) ? ballxpos : ballPosLastRender.x;
	rect.dy = (ballypos < ballPosLastRender.y) ? ballypos : ballPosLastRender.y;
	rect.width = ((ballxpos > ballPosLastRender.x) ? ballxpos : ballPosLastRender.x)+(ballSize+1) - rect.dx;
	rect.height = ((ballypos > ballPosLastRender.y) ? ballypos : ballPosLastRender.y)+(ballSize+1) - rect.dy;
	
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

void interrupt_handler(int n, siginfo_t *info, void *unused) //interrupt function
{
    //printf("USER SPACE!!!! Signal nr.: %d\n", n);
	int character;
	read(driver, &character, 1);
	character=(~character) & 0xFF;
	SW2=false;
	SW4=false;
	SW6=false;
	SW8=false;
	
	if((character & 0b10000000) != 0) {
		//printf("SW8\n");
		SW8=true;
	}
	if((character & 0b01000000) != 0) {
		//printf("SW7\n");
	}
	if((character & 0b00100000) != 0) {
		//printf("SW6\n");
		SW6=true;
	}
	if((character & 0b00010000) != 0) {
		//printf("SW5\n");
	}
	if((character & 0b00001000) != 0) {
		//printf("SW4\n");
		SW4=true;
	}
	if((character & 0b00000100) != 0) {
		//printf("SW3\n");
	}
	if((character & 0b00000010) != 0) {
		//printf("SW2\n");
		SW2=true;
	}
	if((character & 0b00000001) != 0) {
		//printf("SW1\n");
	}
}

int main(int argc, char *argv[])
{
	int FPS=30;
	double frameTime = 1.0 / FPS;
	struct timeval lastTime;
	gettimeofday(&lastTime, NULL);
	double accumulator = 0.0;

	int frames = 0;
	double frameCounter = 0.0;
	
	int pfd = open("/dev/fb0", O_RDWR); // open framebuffer
	uint16_t* addr = mmap(NULL, SCREENSIZE_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, pfd, 0);
	driver = open("/dev/gamepad", O_RDWR);
	
	initGame(pfd, addr);
	
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
			input(driver, (float)frameTime);
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
	
	free(ball);
	
	close(driver); // close driver
	
	//close framebuffer
	munmap(addr, SCREENSIZE_BYTES);
	close(pfd);
	
	exit(EXIT_SUCCESS);
}
