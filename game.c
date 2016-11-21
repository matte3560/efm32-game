#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "input.h"
#include "screen.h"
#include "sound.h"

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
int ballSize;
float ballSpeed;
float ballSpeedIncrement;
int *ball;

// player left variables
vec2 playerLeftPos;
vec2 playerLeftLastRender;
int playerLeftSizeX;
int playerLeftSizeY;
float playerLeftSpeed;
int playerLeftScore;

// player right variables
vec2 playerRightPos;
vec2 playerRightLastRender;
int playerRightSizeX;
int playerRightSizeY;
float playerRightSpeed;
int playerRightScore;

void initGame() {
	
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
	ballSpeed = 100.0f;
	ballSpeedIncrement=10.0f;
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
	playerLeftScore=0;
	/* INIT LEFT PLAYER */
	
	/* INIT RIGHT PLAYER */
	playerRightSizeX = 10.0f;
	playerRightSizeY = 40.0f;
	playerRightPos.x = SCREEN_WIDTH-playerRightSizeX-11.0f;
	playerRightPos.y = (SCREEN_HEIGHT/2)-(playerRightSizeY/2);
	playerRightLastRender = playerRightPos;
	playerRightSpeed = 150.0f;
	playerRightScore=0;
	/* INIT RIGHT PLAYER */
	
	drawscore(playerLeftScore, playerRightScore);
}

void input(float dt) { // update player positions
	// INTERRUPT
	if(SW2 && !SW4) {
		playerLeftPos.y-=playerLeftSpeed*dt;
		if(playerLeftPos.y < 0)
			playerLeftPos.y=0;
	}
	if(SW4 && !SW2) {
		playerLeftPos.y+=playerLeftSpeed*dt;
		if(playerLeftPos.y+playerLeftSizeY >= SCREEN_HEIGHT)
			playerLeftPos.y=SCREEN_HEIGHT-playerLeftSizeY-1;
	}
	if(SW6 && !SW8) {
		playerRightPos.y-=playerRightSpeed*dt;
		if(playerRightPos.y < 0)
			playerRightPos.y=0;
	}
	if(SW8 && !SW6) {
		playerRightPos.y+=playerRightSpeed*dt;
		if(playerRightPos.y+playerRightSizeY >= SCREEN_HEIGHT)
			playerRightPos.y=SCREEN_HEIGHT-playerRightSizeY-1;
	}
	
	// POLLING
	/*int input;
	read(driver, &input, 1);
	input=(~input) & 0xFF;
	
	if((input & 0b10000000) != 0) {
		playerRightPos.y+=playerRightSpeed*dt;
		if(playerRightPos.y+playerRightSizeY >= SCREEN_HEIGHT)
			playerRightPos.y=SCREEN_HEIGHT-playerRightSizeY-1;
	}
	if((input & 0b01000000) != 0) {
		//printf("SW7\n");
	}
	if((input & 0b00100000) != 0) {
		playerRightPos.y-=playerRightSpeed*dt;
		if(playerRightPos.y < 0)
			playerRightPos.y=0;
	}
	if((input & 0b00010000) != 0) {
		//printf("SW5\n");
	}
	if((input & 0b00001000) != 0) {
		playerLeftPos.y+=playerLeftSpeed*dt;
		if(playerLeftPos.y+playerLeftSizeY >= SCREEN_HEIGHT)
			playerLeftPos.y=SCREEN_HEIGHT-playerLeftSizeY-1;
	}
	if((input & 0b00000100) != 0) {
		//printf("SW3\n");
	}
	if((input & 0b00000010) != 0) {
		playerLeftPos.y-=playerLeftSpeed*dt;
		if(playerLeftPos.y < 0)
			playerLeftPos.y=0;
	}
	if((input & 0b00000001) != 0) {
		//printf("SW1\n");
	}*/
}

void update(float dt) { // update ball position
	ballSpeed+=ballSpeedIncrement*dt; // ballspeed will be incremented by 10 every second
	ballPos = add(ballPos, mul(ballDir, ballSpeed*dt));
	
	/* WIN LOGIC */
	bool win=false;
	if(ballPos.x <= playerLeftPos.x) { // player right win
		win=true;
		playerRightScore++;
	} else if(ballPos.x+ballSize >= playerRightPos.x+playerRightSizeX) { // player left win
		win=true;
		playerLeftScore++;
	}
	if(win) {
		drawline();
		drawscore(playerLeftScore, playerRightScore);
		ballSpeed = 100;
		ballPos.x = SCREEN_WIDTH/2-(ballSize/2.0f);
		ballPos.y = SCREEN_HEIGHT/2-(ballSize/2.0f);
		// Setting up max 20 degree angle in random direction
		int dir = 2*(rand()%2)-1;
		float angle = ((rand()/(float)RAND_MAX)*2 - 1)*(M_PI/6);
		ballDir.x=dir*cosf(angle);
		ballDir.y=-sinf(angle);
		ballDir = normalize(ballDir); // normalized direction vector
		
		// reset player positions
		playerLeftPos.y = (SCREEN_HEIGHT/2)-(playerLeftSizeY/2);
		playerRightPos.y = (SCREEN_HEIGHT/2)-(playerRightSizeY/2);
		
		if(playerRightScore ==5 || playerLeftScore == 5) {
			soundWinning();
			playerRightScore=0;
			playerLeftScore=0;
			
			drawscore(playerLeftScore, playerRightScore);
		} else {
			soundScore();
		}
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

void renderGame() {

	clear_screenbuffer();
	
	/* RENDER BALL */
	int ballxpos = (int)ballPos.x;
	int ballypos = (int)ballPos.y;
	
	drawball(ballxpos, ballypos, ballSize, ballPosLastRender.x, ballPosLastRender.y, ball, 0b0000011111100000);
	
	ballPosLastRender.x=ballxpos;
	ballPosLastRender.y=ballypos;
	/* RENDER BALL */
	
	/* RENDER PLAYER LEFT */
	int playerleftposx = (int)playerLeftPos.x;
	int playerleftposy = (int)playerLeftPos.y;
	drawrect(playerleftposx, playerleftposy, playerLeftSizeX, playerLeftSizeY, playerLeftLastRender.x, playerLeftLastRender.y, 0b0000000000011111);
	
	playerLeftLastRender.x=playerleftposx;
	playerLeftLastRender.y=playerleftposy;
	/* RENDER PLAYER LEFT */
	
	/* RENDER PLAYER RIGHT */
	int playerrightposx = (int)playerRightPos.x;
	int playerrightposy = (int)playerRightPos.y;
	drawrect(playerrightposx, playerrightposy, playerRightSizeX, playerRightSizeY, playerRightLastRender.x, playerRightLastRender.y, 0b1111100000000000);
	
	playerRightLastRender.x=playerrightposx;
	playerRightLastRender.y=playerrightposy;
	/* RENDER PLAYER RIGHT */
}

int main(int argc, char *argv[])
{
	int FPS=30;
	double frameTime = 1.0 / FPS;
	struct timeval lastTime;
	gettimeofday(&lastTime, NULL);

	int frames = 0;
	double frameCounter = 0.0;
	
	init_input();
	init_screen();
	initSound();
	initGame();

	soundStartup();
	
	bool done = false;
	do {
		struct timeval newTime;
		gettimeofday(&newTime, NULL);

		long int ms = ((newTime.tv_sec * 1000000 + newTime.tv_usec) - (lastTime.tv_sec * 1000000 + lastTime.tv_usec));
		double passedTime = ms/1e6;
		lastTime = newTime;

		if (passedTime > 1.00) // avoiding spiral of death
			passedTime = 0.0;

		frameCounter += passedTime;

		//game input
		input((float)passedTime);
		//game update
		update((float)passedTime);

		if (frameCounter >= 1.0)
		{
			printf("FPS: %d\n", frames);

			frames = 0;
			frameCounter = 0.0;
		}

		//render framebuffer
		renderGame();

		frames++;
	} while(!done);
	
	free(ball);
	
	close_input();
	close_screen();
	
	exit(EXIT_SUCCESS);
}
