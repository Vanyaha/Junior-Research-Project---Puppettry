/*
 drive.c:

 Compile command:
   gcc -Wall -o drive drive.c -lwiringPi

 Usage:
   sudo ./drive MOTOR COUNT DIR(0-1)
   MOTOR: Motor number (1 to 4)
   COUNT: Encoder count target
   DIR: Direction of the motor
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>
#include <softPwm.h>

#define DEFAULT_HI_DUTY      90
#define DEFAULT_LO_DUTY      50
#define SLOWDOWN_THRESHOLD   10
#define STOP_THRESHOLD        2

////*********************/////
#include <sys/types.h>
#include <unistd.h>

#define PWM_0 0
#define PWM_1 1
#define PWM_2 2
#define PWM_3 3
////*********************/////



typedef struct {
	int driver_pins[4];
	int driver_pin;
	int fb_pin;
	int dir_pin;
	char is_hw;
} _t_motor;

_t_motor motor;

// pulseCounter:
//  Variable to count interrupts
//  Should be declared volatile to make sure the compiler doesn't cache it.
static volatile int pulseCounter;

int motorResponse (int motor_num, int finalCount, int direction);
void control_event(int sig);
void driveMotor(int duty_cycle);
int set_motor_pins(int motor_num);


////////////////***************************************////////////////


void myInterrupt0 (void)
{
  ++pulseCounter;
  //printf("Pulse Count:  %d\n", pulseCounter); 
}
////////////////***************************************////////////////

////////////////***************************************////////////////
//			INT MAIN STARTS HERE
////////////////***************************************////////////////
int main()
{
  
  printf("\n\n\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
  printf("Resetting Internal/External PWM outputs to null \n");  
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n\n\n");
  
  // Intializing PWM pins and outputs to off for start of program //

  pinMode(26,PWM_OUTPUT);	// Internal PWM0 output  /
  pinMode(23,PWM_OUTPUT);	// Internal PWM1 output  /
  
  pinMode(0,OUTPUT);    // Four bit input to eternal PWM2; 	
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(21,OUTPUT);

  pinMode(4,OUTPUT);	// Four bit input to eternal PWM3; 
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(27,OUTPUT);

  pwmWrite(26,0);	      //initialize to off
  pwmWrite(23,0);             //initialize to off 

  pid_t pwm_0_id, pwm_1_id, pwm_2_id, pwm_3_id;
  pwm_0_id = getpid();

  int pwm0dir, pwm1dir, pwm2dir, pwm3dir;
  char me;
  wiringPiSetup();
  pwm0dir = LOW;
//  pwm1dir = LOW;
//  pwm2dir = LOW;
//  pwm3dir = LOW;

  pinMode(1,OUTPUT);	/* PWM0 direction control  */
  pinMode(22,OUTPUT);	/* PWM1 direction control  */
  pinMode(7,OUTPUT);	/* PWM2 direction control  */
  pinMode(28,OUTPUT);	/* PWM3 direction control  */

  //pwm0dir = LOW;
  //pwm1dir = HIGH;
  //digitalWrite(1,pwm0dir);
  //digitalWrite(22, pwm1dir);
  
//  printf("Are you ready to see the motors independently run??\n\n");
// printf("5 \n");
 // delay (1000);
 // printf("4 \n");
 // delay (1000);
 // printf("3 \n");
 // delay (1000);
  //printf("2 \n");
 // delay (1000);
//  printf("1 \n");
//delay (1000);

  (void)signal(SIGINT, control_event);
  (void)signal(SIGQUIT, control_event);
  (void)signal(SIGTERM, control_event);

  /////////////////////// FORK COMMAND ///////////////////////////////

  pwm_1_id = fork();
	if (pwm_1_id == -1) {	/* the fork has failed and we might as well quit  */
		printf("Could not create pwm1 id.  Closing down\n");
		return -1;
	}
	if (pwm_1_id == 0) {	/* The child process received a 0  */
		pwm_1_id = getpid();	/*  We are in process pwm_1  */
		//printf("I am pwm_1 with pid %d\n", pwm_1_id);
		pwm_2_id = fork();
		if (pwm_2_id == -1)  {  /* the fork has failed and we might as well quit  */
			printf("Could not create pwm2 id.  Closing down\n");
			return -1;
		}
		if (pwm_2_id == 0) {	/* The child process receives a 0  */
			pwm_2_id = getpid();  /* we are iin process pwm_2 */
			me = PWM_2;	/* process pwm_2 also knows the value of pwm_0_id and pwm_1_id  */
			//printf("I am pwm_2 with pid %d\n", pwm_2_id);
		}
		else me = PWM_1;	/* process pwm_1 also know the value of pwm_0_id and pwm_2_id */
	}
	else  {	/* the parent process received the id for pwm_1_id */
		pwm_3_id = fork();
		if (pwm_3_id == -1) {	/* the fork has failed ane we might as well quit  */
			printf("Could not create pwm3 id.  Closing down\n");
			return -1;
		}
		if (pwm_3_id == 0) {	/* The child process receives a 0 */
			pwm_3_id = getpid();	/* we are in process pwm_3  */
			//printf("I am pwm_3 with pid %d\n", pwm_3_id);
			me = PWM_3;	/* process pwm_3 also knows the value of pwm_0_id and pwm_1_id */
		}
		else {
			me = PWM_0;	/* process pwm_0 also knows the value of pwm_1_id and pwm_3_id */
			printf("I am pwm_0 with pid %d\n", pwm_0_id);
		}
	}

  while (1)  {
  //              (void)signal(SIGINT, control_event);
//		(void)signal(SIGQUIT, control_event);
//		(void)signal(SIGTERM, control_event);

		switch(me) {
			case PWM_0:

				delay(5000);
				digitalWrite(1, pwm0dir);
				pwmWrite(26, 900);
				delay(800);
				pwm0dir = HIGH;
				digitalWrite(1, pwm0dir);
				delay(800);
				pwmWrite(26, 50);
				printf("I am in case pwm0\n\n\n");
				if(pwm0dir == HIGH){
				  pwm0dir = LOW;
				}
//				delay(5000);
//				motorResponse(1, 72,  1);
//				motorResponse(1, 72, 0);
				break;

			case PWM_1:

				delay(5000);
				digitalWrite(23, pwm1dir);
				pwmWrite(23, 900);
				delay(500);
				pwm1dir = HIGH;
				digitalWrite(23, pwm1dir);
				delay(1000);
				pwm1dir = LOW;
				delay(500);
				pwmWrite(23, 50);
				printf("I am in case pwm1\n\n\n");
				if(pwm1dir == LOW){
				  pwm1dir = HIGH;
				}
//				delay(5000); 
//				motorResponse(2, 72, 1);
//				motorResponse(2, 72, 1);
//				motorResponse(2, 36, 1);
				break;

			case PWM_2:

				delay(10000);
				digitalWrite(7, pwm2dir);
				digitalWrite(21,HIGH); //* PW1100b which is almost 800/1024 */
			        digitalWrite(3, HIGH);
       				digitalWrite(2, HIGH);
          			digitalWrite(0, HIGH);
				delay(500);
				pwm2dir = HIGH;
				digitalWrite(7, pwm2dir);
				delay(1000);
				pwm2dir = LOW;
				digitalWrite(7, pwm2dir);
				delay(500);
				digitalWrite(21, LOW);
				digitalWrite(3, LOW);
				digitalWrite(2, LOW);
				digitalWrite(0, HIGH);
				printf("I am in pwm2\n\n\n");
				if(pwm2dir == LOW){
				  pwm2dir = HIGH;
				}
				delay(20000);
//				motorResponse(3, 72, 1);
//				motorResponse(3, 18, 1);
//				motorResponse(3, 0, 0);

				break;

			case PWM_3:
				delay(10000);
				digitalWrite(28, pwm3dir);
				digitalWrite(27, HIGH);
				digitalWrite(6, HIGH);
				digitalWrite(5, HIGH);
				digitalWrite(4, HIGH);
				delay(1500);
				pwm3dir = HIGH;
				digitalWrite(28, pwm3dir);
				delay(1500);
				digitalWrite(27, LOW);
				digitalWrite(6, LOW);
				digitalWrite(5, LOW);
				digitalWrite(4, HIGH);
				printf("I am in pwm3\n\n\n");
				if(pwm3dir == HIGH){
				  pwm3dir = LOW;
				}

//				printf("what is going on with PWM3\n\n");
//				delay(10000);
				//digitalWrite(4, HIGH);
				//digitalWrite(5, HIGH);
				//digitalWrite(6, HIGH);
				//digitalWrite(27, HIGH);
//				motorResponse(4, 72, 1);
//				motorResponse(4, 72, 0);
				break;

			default:
				printf("exiting program");
				return 0;
				break;
		}
	}

  return 0;
}



////////////////***************************************////////////////
//                         INT MAIN ENDS HERE
////////////////***************************************////////////////

int motorResponse (int motor_num, int finalCount, int direction)
{ 
 

  //if (argc <= 3)
  //{
  //  fprintf(stderr, "Usage: [sudo] ./drive MOTOR(1-4) COUNT DIR(0-1)\n");
  //  return 1;
  //}
  
  //int motor_num = motor_num;
  //int finalCount = finalCount;
  //int direction = direction;
  printf("Driving motor %d, to count %d, dir %d\n", motor_num, finalCount, direction);
  printf("\n\n\n\n"); 

  enum Phases
  {
    STOPPED,
    HI_SPEED,
    LO_SPEED
  };
  enum Phases phase = STOPPED;

  (void)signal(SIGINT,control_event);
  (void)signal(SIGQUIT,control_event);
  (void)signal(SIGTERM,control_event);

  if (wiringPiSetup() < 0)
  {
    fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
    return 1;
  }

  if(set_motor_pins(motor_num) == 1)
  {
	  fprintf(stderr, "Invalid motor number, must be 1 to 4.\n");
	  return 1;
  }

  int stop_count = finalCount-STOP_THRESHOLD > 2 ?
      finalCount-STOP_THRESHOLD : 2;
  int slowDown_count = finalCount-SLOWDOWN_THRESHOLD > 1 ?
      finalCount-SLOWDOWN_THRESHOLD : 1;
  if (slowDown_count >= stop_count)
  {
    slowDown_count = stop_count-STOP_THRESHOLD > 1 ?
      stop_count-STOP_THRESHOLD : 1;
  }

  pulseCounter = 0;
  printf("Monitoring pin %d, slowdown at %d, stop at %d\n", motor.fb_pin,
      slowDown_count, stop_count);
  wiringPiISR(motor.fb_pin, INT_EDGE_FALLING, &myInterrupt0);

  digitalWrite(motor.dir_pin, direction);

  while (1)
  {
    if (phase == STOPPED
        && pulseCounter < slowDown_count)
    {
      driveMotor(DEFAULT_HI_DUTY);
      phase = HI_SPEED;
    }
    else if ((phase == HI_SPEED || phase == STOPPED )
        && pulseCounter >= slowDown_count
        && pulseCounter < stop_count)
    {
      driveMotor(DEFAULT_LO_DUTY);
      phase = LO_SPEED;
    }
    else if ((phase == LO_SPEED || phase == HI_SPEED)
       && pulseCounter >= stop_count)
    {
      driveMotor(0);
      delay(400);
      phase = STOPPED;
      break;
    }
    delay(100);
  }
  driveMotor(0);
  printf("Final count: %d\n", pulseCounter);
  return 0;

}

////////////////***************************************////////////////
//			INT MAIN ENDS HERE
////////////////***************************************////////////////


void driveMotor(int duty_cycle)
{
	if (motor.is_hw == 1)
	{
		int value = duty_cycle*1024/100;
		pwmWrite(motor.driver_pin, value);
	}
	else
	{
		int i;
		int bitval;
		int power = (int)((float)(duty_cycle) * 15.0)/100.0;
		for( i=0; i<4; i++)
		{
			int mask = 1 << i;
			bitval = (power & mask) >> i;
			digitalWrite(motor.driver_pins[i], bitval);
		}
	}
}



////////////////***************************************////////////////


void control_event(int sig)
{
  printf("\n  Exiting...\n");
  driveMotor(0);
  delay(200); //wait a little for the pwm to finish write
  printf("Count: %d\n", pulseCounter);
  exit(0);
}



////////////////***************************************////////////////



int set_motor_pins(int motor_num)  // motor 1 to 4
{
	int i;
	switch(motor_num)
	{
		case 1:
			// PWM0
			for(i =0; i<4; i++)
				motor.driver_pins[i] = -1;
			motor.is_hw = 1;
			motor.driver_pin = 26;
			motor.fb_pin = 8;
			motor.dir_pin = 1;
			pinMode (26, PWM_OUTPUT) ;
			pinMode (1, OUTPUT) ;
			pinMode (8, INPUT) ;
		
			break;
		case 2:
			// PWM1
			for(i =0; i<4; i++)
				motor.driver_pins[i] = -1;
			motor.is_hw = 1;
			motor.driver_pin = 23;
			motor.fb_pin = 9;
			motor.dir_pin = 22;
			pinMode (23, PWM_OUTPUT) ;
			pinMode (22, OUTPUT) ;
			pinMode (9, INPUT) ;
			break;			
		case 3:
			// PWM2
		    	motor.driver_pins[0] = 0;
		    	motor.driver_pins[1] = 2;
		    	motor.driver_pins[2] = 3;
		    	motor.driver_pins[3] = 21;
			
			motor.driver_pin = -1;
			motor.is_hw = 0;
			motor.fb_pin = 10;
			motor.dir_pin = 7;
			pinMode (0, OUTPUT) ;
			pinMode (2, OUTPUT) ;
			pinMode (3, OUTPUT) ;
			pinMode (21, OUTPUT) ;
			pinMode (7, OUTPUT) ;
			pinMode (10, INPUT) ;
			break;
		case 4:
			// PWM3
		    	motor.driver_pins[0] = 4;
		    	motor.driver_pins[1] = 5;
		    	motor.driver_pins[2] = 6;
		    	motor.driver_pins[3] = 27;
			
			motor.driver_pin = -1;
			motor.is_hw = 0;
			motor.fb_pin = 11;
			motor.dir_pin = 28;
			pinMode (4, OUTPUT) ;
			pinMode (5, OUTPUT) ;
			pinMode (6, OUTPUT) ;
			pinMode (27, OUTPUT) ;
			pinMode (28, OUTPUT) ;
			pinMode (11, INPUT) ;				
			break;
		default:
			return 1;
			break;
	}
	if(motor.is_hw == 1){
		printf("hw motor pins:\n  drive:%d\n  fb:%d\n  dir:%d\n",motor.driver_pin, motor.fb_pin, motor.dir_pin);
	}
	return 0;
}



