#include "IMU.h"

/** This is the sensor driver for the Adafruit BNO055 IMU. */
Adafruit_BNO055 sensor = Adafruit_BNO055(BNO055_ID, BNO055_ADDRESS_A);

/** The data from the IMU gets read into this struct. */
sensors_event_t * event = (sensors_event_t *) malloc(sizeof(sensors_event_t));

/** This struct stores the running averages for pitch and roll, where the average is "reset" once the data is read via get_pitch() or get_roll(). */
volatile IMU::orientation_vec data = {0, 0};

/** Keeps track of number of times the IMU has been polled for pitch data since the last get_pitch() call. */
int num_reads_p = 0;

/** Keeps track of number of times the IMU has been polled for roll data since the last get_roll() call. */
int num_reads_r = 0;

IMU::IMU() {
	
}

IMU::~IMU(){
	free(event);
}


/**
	Starts up the sensor driver and starts the PIT1 interrupts to poll the IMU.
	This function causes a small delay due to the set up of the sensor.
*/
void IMU::begin() {
	sensor.begin();
	IMU::begin_timer();
}

/**
	Sets up PIT1 to cause an interrupt 30 times per second.
*/
void IMU::begin_timer(){
	int rate = 10;
	
	NVIC_EnableIRQ(PIT1_IRQn); //enable PIT1 interrupts
	
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = PIT_MCR_MDIS(0);
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1); //clear interrupt flag
	PIT->CHANNEL[1].LDVAL = DEFAULT_SYSTEM_CLOCK / rate; //set timer to 1 second
	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TIE(1); //enable interrupts
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN(1);
	NVIC_SetPriority(PIT1_IRQn, 0);
}

/**
	Returns the running average of the IMU pitch data since the last
	call to this function. Resets the running average so that num_reads_p 
	is now 0.
*/
double IMU::get_pitch(){
	__disable_irq();
	
	num_reads_p = 0;
	
	__enable_irq();
	
	return data.pitch;
}

/**
	Returns the running average of the IMU roll data since the last
	call to this function. Resets the running average so that num_reads_r
	is now 0.
*/
double IMU::get_roll(){
	__disable_irq();
	
	num_reads_r = 0;
	
	__enable_irq();
	
	return data.roll;
}

/**
	Polls the IMU sensor for pitch and roll data and incorporates the 
	results into the running averages stored by the data struct.
	This function is used by the PIT1 interrupt.
*/
void IMU::read_data(){
	sensor.Adafruit_BNO055::getEvent(event);
	if (num_reads_r > 0) {
		data.roll = (num_reads_r * data.roll + event->orientation.z) / (num_reads_r + 1.0);
	} else {
		data.roll = event->orientation.z;
	}
	if (num_reads_p > 0) {
		data.pitch = (num_reads_p * data.pitch + event->orientation.y) / (num_reads_p + 1.0);
	} else {
		data.pitch = event->orientation.y;
	}
	num_reads_r++;
	num_reads_p++;
}

/**
	Interrupt handler for PIT1. Causes the IMU driver to poll the sensor
	and add the new data to the running averages for pitch and roll stored
	by the IMU driver.
*/
extern "C" void PIT1_IRQHandler(void)
{
	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TEN(0);
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1); //reset flag

	IMU::read_data();

	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TEN(1);
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE(1);
}


