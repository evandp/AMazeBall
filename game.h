#include "matrix_controller.h"
#include "imu.h"
#include "utils.h"

class Game {
	
	public:
		Game();
		void terminate();
	
	private:
		void init_ball();
		void loop();
		void calc_vel();
		void update_ball();
		bool is_running;
	
		typedef struct {
			int pos_x;
			int pos_y;
			float vel_x;
			float vel_y;
		} ball_t;
		
		ball_t ball;
};