#ifndef __3420IMU_H__
#define __3420IMU_H__

#include <fsl_i2c.h>
#include <fsl_device_registers.h>
#include "Adafruit_BNO055.h"


/** 
	This class is the interface between the Adafruit BNO055 driver and the game driver.
	Its main responsibility is to periodically poll the IMU for data, and keep a running
	average of this data until it is read by the game driver. 
*/
class IMU {

public:
	
	/** Stores data from IMU */
	typedef struct {
		double pitch;
		double roll;
	} orientation_vec;
	
	/** Constructor for IMU (useless because all members are static) */
	IMU();
	
	/** Destructor for IMU (useless because all members are static) */
	~IMU();
	
	/** Returns the filtered (i.e. averaged) pitch data.	*/
	static double get_pitch();
	
	/** Returns the filtered (i.e. averaged) roll data.	*/
	static double get_roll();
	
	/** Initializes IMU driver and begins reading data via PIT interrupt. */
	static void begin();
	
	/** Polls data from Adafruit BNO055 device for roll and pitch. */
	static void read_data();
	
private:
	
	/** Sets up timer interrupt on PIT1. */
	static void begin_timer();

};

#endif
