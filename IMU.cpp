#include "IMU.h"

Adafruit_BNO055 sensor = Adafruit_BNO055(BNO055_ID, BNO055_ADDRESS_A);
sensors_event_t * event = (sensors_event_t *) malloc(sizeof(sensors_event_t));
volatile IMU::orientation_vec data = {0, 0};
int num_reads_p = 0;
int num_reads_r = 0;

IMU::IMU() {
	
}

IMU::~IMU(){
	free(event);
}

void IMU::begin() {
	sensor.begin();
	IMU::begin_timer();
}

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

double IMU::get_pitch(){
	__disable_irq();
	
	num_reads_p = 0;
	
	__enable_irq();
	
	return data.pitch;
}

double IMU::get_roll(){
	__disable_irq();
	
	num_reads_r = 0;
	
	__enable_irq();
	
	return data.roll;
}

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

extern "C" void PIT1_IRQHandler(void)
{
	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TEN(0);
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1); //reset flag

	IMU::read_data();

	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TEN(1);
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE(1);
}


