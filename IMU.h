#ifndef __3420IMU_H__
#define __3420IMU_H__

#include <fsl_i2c.h>
#include <fsl_device_registers.h>
#include "Adafruit_BNO055.h"

class IMU {

public:
	typedef struct {
		double pitch;
		double roll;
	} orientation_vec;
	IMU();
	~IMU();
	static double get_pitch();
	static double get_roll();
	static void begin();
	static void read_data();
	
private:
	static void begin_timer();

};

#endif
