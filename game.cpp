#include "game.h"

void Game::init_ball() {
	Game::ball.pos_x = 15;
	Game::ball.pos_y = 15;
	Game::ball.vel_x = 0;
	Game::ball.vel_y = 0;
}

Game::Game() {
	IMU::begin();
	MatrixController::init();
	Game::init_ball();
	Game::is_running = true;
	Game::loop();
}

void Game::terminate() {
	Game::is_running = false;
}

void Game::calc_vel() {
	double pitch = IMU::get_pitch();
	double roll = IMU::get_roll();
	
	int acc_x = 0;
	int acc_y = 0;
	
	if (pitch > 15) {
		acc_y = 1;
	} else if (pitch < -15) {
		acc_y = -1;
	} else {
		acc_y = 0;
	}
	
	if (roll > 15) {
		acc_x = 1;
	} else if (roll < -15) {
		acc_x = -1;
	} else {
		acc_x = 0;
	}
	
	ball.vel_x += acc_x;
	ball.vel_y += acc_y;
	
	if (ball.vel_x > 3) ball.vel_x = 3;
	if (ball.vel_x < -3) ball.vel_x = -3;
	
	if (ball.vel_y > 3) ball.vel_y = 3;
	if (ball.vel_y < -3) ball.vel_y = -3;
	
	// simulates drag
	if (acc_x == 0) {
		ball.vel_x *= 0.75;
	}
	if (acc_y == 0) {
		ball.vel_y *= 0.75;
	}
}

void Game::update_ball() {
	MatrixController::update_pixel(ball.pos_x, ball.pos_y, 0, 0, 0);
	ball.pos_x += (int)ball.vel_x;
	ball.pos_y += (int)ball.vel_y;
	
	if (ball.pos_x < 0) {
		ball.pos_x = 0;
	} else if (ball.pos_x > 31) {
		ball.pos_x = 31;
	}
	
	if (ball.pos_y < 0) {
		ball.pos_y = 0;
	} else if (ball.pos_y > 31) {
		ball.pos_y = 31;
	}
	MatrixController::update_pixel(ball.pos_x, ball.pos_y, 0, 1, 0);
}

void Game::loop() {
	while (Game::is_running) {
		calc_vel();
		update_ball();
		utils::delay(10);
	}
}
