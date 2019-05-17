#include <math.h>
#include "game.h"

#define RATE 100.0f
#define G 245.25f // acceleration due to gravity in pixels per second^2
#define M_PI 3.14159265358979323846

int round(float x) {
	return (int) (x + 0.5f);
}

Game::ball_t ball = {
	0.0f,
	0.0f,
	0.0f,
	0.0f,
};

int score = -1;

int maze[32][32];

void Game::init_maze() {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			maze[i][j] = Utils::maze1[i][j];
			if (maze[i][j] == 1) {
				if (i == 0 || i == 6 || i == 31 || j == 0 || j == 31) {
					MatrixController::update_pixel(i, j, 0, 0, 1);
				}
				else if (1 <= i && i <= 5) {
					MatrixController::update_pixel(i, j, 1, 1, 0);
				} else {
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
	update_score();
}

void Game::start() {
	IMU::begin();
	MatrixController::init();
	Game::init_maze();
	Game::init_timer();
}

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

void Game::calc_vel() {
	double pitch = IMU::get_pitch();
	double roll = IMU::get_roll();
	
	int acc_x = G * sin(roll * M_PI / 180); // pixels per second^2 in x
	int acc_y = G * sin(pitch * M_PI / 180); // pixels per second^2 in y
	
	ball.vel_x += acc_x / RATE;
	ball.vel_y += acc_y / RATE;
}

bool edge_check(float prev_x, float prev_y, float curr_x, float curr_y, int x_dir, int y_dir) {
	return maze[round(prev_x + x_dir)][round(prev_y + y_dir)] == 1 && round(prev_x + x_dir) == round(curr_x) && round(prev_y + y_dir) == round(curr_y);
}

void Game::update_score() {
	score++;
	int msd = score / 10; // most significant digit
	int lsd = score % 10; // leasat significant digit
	MatrixController::draw_number(msd, 1, 23, 1, 1, 0);
	MatrixController::draw_number(lsd, 1, 27, 1, 1, 0);
}

void Game::update_ball() {
	__disable_irq();
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
		update_score();
	}
	
	MatrixController::update_pixel(round(ball.pos_x), round(ball.pos_y), 0, 1, 0);
	__enable_irq();
}

void Game::loop() {
	Game::calc_vel();
	update_ball();
}

extern "C" void PIT2_IRQHandler(void)
{
	PIT->CHANNEL[2].TCTRL = PIT_TCTRL_TEN(0); //disable interrupts
	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF(1); //reset flag
	Game::loop();
	PIT->CHANNEL[2].TCTRL = PIT_TCTRL_TEN(1); //reenable interrupts
	PIT->CHANNEL[2].TCTRL |= PIT_TCTRL_TIE(1);
}
