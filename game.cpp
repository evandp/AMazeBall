#include <math.h>
#include "game.h"

/**	Defines how quickly the game loop runs */
#define RATE 100.0f

/**	Acceleration due to gravity in pixels per second^2, used to calculate velocity of ball */
#define G 490.5f

/**	we love pi */
#define M_PI 3.14159265358979323846

/**
	Returns the given number rounded to the nearest integer.
	@param x number to be rounded
	@return x to nearest whole number 
*/
int round(float x) {
	return (int) (x + 0.5f);
}

/**
	Current state of the ball. Stores the current x,y position
	and velocity of the ball, which are all initialized to 0.
*/
Game::ball_t ball = {
	0.0f,
	0.0f,
	0.0f,
	0.0f,
};

/**
	Player's current score. Initialized to 0.
*/
int score = 0;

/**
	Represents features of the maze. The value at
	maze[i][j] determines the object at that location 
	(0 for nothing, 1 for wall, 2 for food, 3 for ball's
	initial location).
*/
int maze[32][32];

/**
	Time elapsed so far in the game.
*/
float time = 0.0f;

/**
	Stores whether the timer progress bar on the LED matrix
	should be blinking.
*/
bool blink_toggle = false;

/**
	Tells the game loop whether this is the first time that 
	the loop is being run.
*/
bool is_starting = true;

/**
	Time buffer for the countdown before the start of gameplay.
*/
float start_countdown = 1.5f;

/**
	Sets up initial state of the maze and displays it on LED matrix.
	Loads values from Utils::maze1 into Game::maze, and writes the 
	relevant RGB values to the LED matrix.
*/
void Game::init_maze() {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			maze[i][j] = Utils::maze1[i][j];
			if (maze[i][j] == 1) {
				if (i == 0 || i == 6 || i == 31 || j == 0 || j == 31) {
					MatrixController::update_pixel(i, j, 0, 0, 1);
				}
				else if (1 <= i && i <= 5 && !is_starting) {
					MatrixController::update_pixel(i, j, 1, 1, 0);
				} else if (!is_starting) {
					MatrixController::update_pixel(i, j, 1, 0, 0);
				}
			}
			if (maze[i][j] == 2) MatrixController::update_pixel(i, j, 1, 0, 1);
			if (maze[i][j] == 3) {
				ball.pos_x = i;
				ball.pos_y = j;
			}
		}
	}
	if (!is_starting) update_score();
}

/**
	Entry point into the game. Initializes IMU and LED matrix drivers.
	Initializes the state of [maze] and writes its values to the LED matrix.
	Begins the PIT2 timer, which starts up the game loop.
*/
void Game::start() {
	IMU::begin();
	MatrixController::init();
	Game::init_maze();
	Game::init_timer();
}

/**
	Sets up PIT2 timer. This timer will go off [RATE] times per second.
*/
void Game::init_timer() {
	NVIC_EnableIRQ(PIT2_IRQn); //enable PIT0 interrupts
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = PIT_MCR_MDIS(0);
	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF(1); //clear interrupt flag
	PIT->CHANNEL[2].LDVAL = DEFAULT_SYSTEM_CLOCK / RATE;
	PIT->CHANNEL[2].TCTRL = PIT_TCTRL_TIE(1); //enable interrupts
	PIT->CHANNEL[2].TCTRL |= PIT_TCTRL_TEN(1);
	NVIC_SetPriority(PIT2_IRQn, 0);
}

/**
	Calculates the new velocity of the ball by reading data from the IMU driver.
	First, the IMU data is used to calculate acceleration due to gravity in X and Y 
	directions. Then, the velocity is updated assuming constant acceleration over
	the time period that has elapsed since the last game loop.
*/
void Game::calc_vel() {
	double pitch = IMU::get_pitch();
	double roll = IMU::get_roll();
	
	int acc_x = G * sin(roll * M_PI / 180); // pixels per second^2 in x
	int acc_y = G * sin(pitch * M_PI / 180); // pixels per second^2 in y
	
	ball.vel_x += acc_x / RATE;
	ball.vel_y += acc_y / RATE;
}

/**
	Determines whether the ball is trying to move to a square that is occupied by a wall.
	@param prev_x previous x location of the ball
	@param prev_y previous y location of the ball
	@param curr_x new x location of the ball, as calculated with the updated velocity
	@param curr_y new y location of the ball, as calculated with the updated velocity
	@param x_dir offset from prev_x
	@param y_dir offset from prev_y
*/
bool edge_check(float prev_x, float prev_y, float curr_x, float curr_y, int x_dir, int y_dir) {
	return maze[round(prev_x + x_dir)][round(prev_y + y_dir)] == 1 && round(prev_x + x_dir) == round(curr_x) && round(prev_y + y_dir) == round(curr_y);
}

/**
	Updates the 2-digit score on the LED display, according to current value
	in [score].
*/
void Game::update_score() {
	int msd = score / 10; // most significant digit
	int lsd = score % 10; // least significant digit
	MatrixController::draw_number(msd, 1, 23, 1, 1, 0);
	MatrixController::draw_number(lsd, 1, 27, 1, 1, 0);
}

/**
	Updates the position of the ball according to its current velocity, ensuring
	that the ball does not go through any barriers. Also increments the score
	if any food items were consumed. Updates LED display to reflect changes.
*/
void Game::update_ball() {
	MatrixController::update_pixel(round(ball.pos_x), round(ball.pos_y), 0, 0, 0);
	float prev_x = ball.pos_x;
	float prev_y = ball.pos_y;
	
	ball.pos_x += ball.vel_x / RATE;
	ball.pos_y += ball.vel_y / RATE;
	
	if (ball.pos_x < 0) {
		ball.pos_x = 0;
		ball.vel_x = 0;
	} else if (ball.pos_x > 31) {
		ball.pos_x = 31;
		ball.vel_x = 0;
	}
	
	if (ball.pos_y < 0) {
		ball.pos_y = 0;
		ball.vel_y = 0;
	} else if (ball.pos_y > 31) {
		ball.pos_y = 31;
		ball.vel_y = 0;
	}
	
	if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, 1, 0)) {
		ball.pos_x = prev_x;
		if (ball.vel_x > 0) ball.vel_x = 0;
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, -1, 0)) {
		ball.pos_x = prev_x;
		if (ball.vel_x < 0) ball.vel_x = 0;
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, 0, 1)) {
		ball.pos_y = prev_y;
		if (ball.vel_y > 0) ball.vel_y = 0;
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, 0, -1)) {
		ball.pos_y = prev_y;
		if (ball.vel_y < 0) ball.vel_y = 0;
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, 1, 1)) {
		if (maze[round(prev_x) + 1][round(prev_y)] == 1) {
			if (ball.vel_x > 0) ball.vel_x = 0;
			ball.pos_x = prev_x;
		}
		if (maze[round(prev_x)][round(prev_y) + 1] == 1) {
			if (ball.vel_y > 0) ball.vel_y = 0;
			ball.pos_y = prev_y;
		}
		if (!(maze[round(prev_x) + 1][round(prev_y)] == 1) && !(maze[round(prev_x)][round(prev_y) + 1] == 1)) {
			if (ball.vel_x > ball.vel_y) {
				ball.vel_y = 0;
				ball.pos_y = prev_y;
			} else {
				ball.vel_x = 0;
				ball.pos_x = prev_x;
			}
		}
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, 1, -1)) {
		if (maze[round(prev_x) + 1][round(prev_y)] == 1) {
			if (ball.vel_x > 0) ball.vel_x = 0;
			ball.pos_x = prev_x;
		}
		if (maze[round(prev_x)][round(prev_y) - 1] == 1) {
			if (ball.vel_y < 0) ball.vel_y = 0;
			ball.pos_y = prev_y;
		} 
		if (!(maze[round(prev_x) + 1][round(prev_y)] == 1) && !(maze[round(prev_x)][round(prev_y) - 1] == 1)) {
			if (ball.vel_x > ball.vel_y) {
				ball.vel_y = 0;
				ball.pos_y = prev_y;
			} else {
				ball.vel_x = 0;
				ball.pos_x = prev_x;
			}
		}
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, -1, 1)) {
		if (maze[round(prev_x) - 1][round(prev_y)] == 1) {
			if (ball.vel_x < 0) ball.vel_x = 0;
			ball.pos_x = prev_x;
		} 
		if (maze[round(prev_x)][round(prev_y) + 1] == 1) {
			if (ball.vel_y > 0) ball.vel_y = 0;
			ball.pos_y = prev_y;
		}
		if (!(maze[round(prev_x) - 1][round(prev_y)] == 1) && !(maze[round(prev_x)][round(prev_y) + 1] == 1)) {
			if (ball.vel_x > ball.vel_y) {
				ball.vel_y = 0;
				ball.pos_y = prev_y;
			} else {
				ball.vel_x = 0;
				ball.pos_x = prev_x;
			}
		}
	}
	else if (edge_check(prev_x, prev_y, ball.pos_x, ball.pos_y, -1, -1)) {
		if (maze[round(prev_x) - 1][round(prev_y)] == 1) {
			if (ball.vel_x < 0) ball.vel_x = 0;
			ball.pos_x = prev_x;
		} 
		if (maze[round(prev_x)][round(prev_y) - 1] == 1) {
			if (ball.vel_y < 0) ball.vel_y = 0;
			ball.pos_y = prev_y;
		}
		if (!(maze[round(prev_x) - 1][round(prev_y)] == 1) && !(maze[round(prev_x)][round(prev_y) - 1] == 1)) {
			if (ball.vel_x > ball.vel_y) {
				ball.vel_y = 0;
				ball.pos_y = prev_y;
			} else {
				ball.vel_x = 0;
				ball.pos_x = prev_x;
			}
		}
	}
	
	if (maze[round(ball.pos_x)][round(ball.pos_y)] == 2) {
		maze[round(ball.pos_x)][round(ball.pos_y)] = 0;
		score++;
		update_score();
	}
	
	MatrixController::update_pixel(round(ball.pos_x), round(ball.pos_y), 0, 1, 0);
}

/**
	Increments the timer. Updates the rate of blinking of the progress bar 
	according to the amount of time remaining.
*/
void Game::update_timer() {
	time += 1 / RATE;

	float time_remaining = 60.0f - time;
	float blink_rate = 60.0f;
	if (time_remaining < 5.0f) {
		blink_rate = 0.05f;
	}
	else if (time_remaining < 15.0f) {
		blink_rate = 0.125f;
	}
	else if (time_remaining < 30.0f) {
		blink_rate = 0.25f;
	}
	if (fmod(time_remaining, blink_rate) < 0.01f) {
		blink_toggle = !blink_toggle;
	}
	if (time < 61.0f) {
		if (blink_toggle) {
			MatrixController::draw_line_horiz(1, (int)(time/2), 0, 0, 0, 1);
		} else {
			MatrixController::draw_line_horiz(1, (int)(time/2), 0, 1, 0, 0);
		}
	}

}

/**
	Performs logic for one time step in the game. Updates the ball's velocity and position,
	the player's score, and the timer. Is called by PIT2 interrupt handler.
*/
void Game::loop() {
	if (is_starting) {
		start_countdown -= 1 / RATE;
		if (start_countdown < 1.5f) {
			MatrixController::draw_number(3, 1, 8, 1, 0, 0);
		}
		if (start_countdown < 1.0f) {
			MatrixController::draw_number(2, 1, 14, 1, 1, 0);
		}
		if (start_countdown < 0.5f) {
			MatrixController::draw_number(1, 1, 20, 0, 1, 0);
		}
		if (start_countdown < 0.0f) {
			MatrixController::clear_number(1, 8);
			MatrixController::clear_number(1, 14);
			MatrixController::clear_number(1, 20);
			is_starting = false;
			Game::init_maze();
		}
	} else {
		Game::calc_vel();
		update_ball();
		update_timer();
	}
}

/**
	Displays end game screen and the player's final score on the LED matrix.
	Clears the maze from the board and displays "GAME OVER".
*/
void Game::game_over() {
	MatrixController::draw_line_horiz(1, 30, 0, 1, 0, 0);
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			maze[i][j] = Utils::game_over[i][j];
			if (maze[i][j] == 1) {
				if (i == 0) continue;
				if (i == 6 || i == 31 || j == 0 || j == 31) {
					MatrixController::update_pixel(i, j, 0, 0, 1);
				}
				else if (1 <= i && i <= 5) {
					continue;
				} else {
					MatrixController::update_pixel(i, j, 1, 0, 0);
				}
			} else {
				MatrixController::update_pixel(i, j, 0, 0, 0);
			}
		}
	}
	update_score();
}

/**
	PIT2 interrupt handler. Calls the game loop function in order to 
	take one step in the game. If time has run out, disable the PIT2 timer
	and display the end screen on the LED.
*/
extern "C" void PIT2_IRQHandler(void)
{
	PIT->CHANNEL[2].TCTRL = PIT_TCTRL_TEN(0); //disable interrupts
	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF(1); //reset flag
	__disable_irq();
	Game::loop();
	__enable_irq();
	if (time >= 60.0f) {
		Game::game_over();
	} else {
		PIT->CHANNEL[2].TCTRL = PIT_TCTRL_TEN(1); //reenable interrupts
		PIT->CHANNEL[2].TCTRL |= PIT_TCTRL_TIE(1);
	}
}
