#include <fsl_i2c.h>
#include <fsl_device_registers.h>
#include "Adafruit_BNO055.h"

Adafruit_BNO055 *sensor;
sensors_event_t *event;
int y = 0;

void LEDRed_On() {
	PTB->PCOR = (1 << 22);
}

void LEDRed_Off() {
	PTB->PSOR = (1 << 22);
}

void setup() {
	NVIC_EnableIRQ(PIT0_IRQn); //enable PIT0 interrupts
	
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //Enable the clock to port E
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	I2C0->C1 = I2C_C1_IICEN(1);
	I2C0->C1 = I2C_C1_IICIE(1);
	
	PORTE->PCR[24] = PORT_PCR_MUX(5);
	PORTE->PCR[25] = PORT_PCR_MUX(5);
	
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = PIT_MCR_MDIS(0);
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1); //clear interrupt flag
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK; //set timer to 1 second
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE(1); //enable interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN(1);
	
	//stuff for LEDs (testing only)
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //Enable the clock to port B
	PORTB->PCR[22] = PORT_PCR_MUX(001); //Set up PTB22 as GPIO
	PTB->PDDR = (3 << 21);
	LEDRed_Off();
}



int main() {
	
	setup();
	
	
	I2C_Enable(I2C0, false);
	I2C_EnableInterrupts(I2C0, kI2C_GlobalInterruptEnable);
	
	i2c_master_config_t conf;
	conf.enableMaster = true;
	conf.enableStopHold = false;
	conf.baudRate_Bps = 100000;
	conf.glitchFilterWidth = 0;
	
	I2C_MasterInit(I2C0, &conf, 21000000);
	
	event = (sensors_event_t *) malloc(sizeof(sensors_event_t));
	
	//default set up
	event->version = 0;
	event->sensor_id = 0;
	event->timestamp = 0;
	event->type = 0;
	
	sensor = new Adafruit_BNO055(BNO055_ID, BNO055_ADDRESS_A);
	
	while(1);
	
	return 0;
}


/* 
     PIT Interrupt Handler
*/
extern "C" void PIT0_IRQHandler(void)
{
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(0); //disable interrupts
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1); //reset flag

	(*sensor).Adafruit_BNO055::getEvent(event);
		y++;
		if (y % 2 == 0){
			LEDRed_Off();
		} else {
			LEDRed_On();
		}
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(1); //reenable interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE(1);
}


