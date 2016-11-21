#include "input.h"

void init_input() {
	SW2=false;
	SW4=false;
	SW6=false;
	SW8=false;
	
	driver = open("/dev/gamepad", O_RDWR);
	
	/* setup the signal handler for SIGUSER1 
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */
	struct sigaction sig;
	sig.sa_sigaction = interrupt_handler;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sig, NULL);
}

void interrupt_handler(int n, siginfo_t *info, void *unused) //interrupt function
{
    //printf("USER SPACE!!!! Signal nr.: %d\n", n);
	int character;
	if(read(driver, &character, 1) == -1) {
		//is the file closed?
	}
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

void close_input() {
	close(driver); // close driver
}