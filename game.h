#include "matrix_controller.h"
#include "imu.h"
#include "utils.h"

class Game {
	
	public:
	
	/**
		Entry point into the game; initializes the relevant timers and hardware interfaces,
		which begins the game loop.
	*/
		static void start();
	
	/**
		Displays end game screen and the player's final score on the LED matrix.
	*/
		static void game_over();
	
	/**
		Performs logic for one time step in the game. Updates the ball's velocity and position,
		the player's score, and the timer.
	*/
		static void loop();
	
	/**
		Stores information about the current state of the ball
	*/
		typedef struct {
			float pos_x; // pixel location in x
			float pos_y; // pixel location in y
			float vel_x; // pixels per second in x
			float vel_y; // pixels per second in y
		} ball_t;
		
	private:
		
	/**
		Displays countdown to start of game on LED matrix.
	*/
		static void start_sequence();
	
	/**
		Sets up initial state of the maze and displays it on LED matrix.
	*/
		static void init_maze();
	
	/**
		Sets up PIT2 timer, which controls the game loop.
	*/
		static void init_timer();
	
	/**
		Calculates the new velocity of the ball by reading data from the IMU driver.
	*/
		static void calc_vel();
	
	/**
		Updates the position of the ball according to its current velocity, ensuring
		that the ball does not go through any barriers. Also increments the score
		if any food items were consumed. Updates LED display to reflect changes.
	*/
		static void update_ball();
		
	/**
		Updates the 2-digit score on the LED display
	*/
		static void update_score();
		
	/**
		Increments the timer. Potentially updates the rate of blinking
		of the timer display on the LED matrix to reflect the new value.
	*/
		static void update_timer();
};
