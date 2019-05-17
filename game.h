#include "matrix_controller.h"
#include "imu.h"
#include "utils.h"

class Game {
	
	public:
		static void start();
		static void terminate();
		static void loop();
	
		typedef struct {
			float pos_x; // pixel location in x
			float pos_y; // pixel location in y
			float vel_x; // pixels per second in x
			float vel_y; // pixels per second in y
		} ball_t;
		
	private:
		static void init_maze();
		static void init_timer();
		static void calc_vel();
		static void update_ball();		
};
