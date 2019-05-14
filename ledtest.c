#include <fsl_device_registers.h>
#include <fsl_gpio.h>


#define clear_clk() (GPIO_PinWrite(clk_p, clk_s, 0))
#define set_clk() (GPIO_PinWrite(clk_p, clk_s, 1))
#define clear_lat() (GPIO_PinWrite(lat_p, lat_s, 0))
#define set_lat() (GPIO_PinWrite(lat_p, lat_s, 1))
#define clear_oe() (GPIO_PinWrite(oe_p, oe_s, 0))
#define set_oe() (GPIO_PinWrite(oe_p, oe_s, 1))

GPIO_Type *r1_p, *g1_p, *b1_p, *r2_p, *g2_p, *b2_p, *a_p, *b_p, *c_p, *d_p, *lat_p, *oe_p, *clk_p;
int r1_s, g1_s, b1_s, r2_s, g2_s, b2_s, a_s, b_s, c_s, d_s, lat_s, oe_s, clk_s;
int row;
int color;

void set_row(int r) {
	if (r & 8){
		GPIO_PinWrite(a_p, a_s, 1);
	} else {
		GPIO_PinWrite(a_p, a_s, 0);
	}
	if (r & 4){
		GPIO_PinWrite(b_p, b_s, 1);
	} else {
		GPIO_PinWrite(b_p, b_s, 0);
	}
	if (r & 2){
		GPIO_PinWrite(c_p, c_s, 1);
	} else {
		GPIO_PinWrite(c_p, c_s, 0);
	}
	if (r & 1){
		GPIO_PinWrite(d_p, d_s, 1);
	} else {
		GPIO_PinWrite(d_p, d_s, 0);
	}
}

void clear_row(){
	GPIO_PinWrite(a_p, a_s, 0);
	GPIO_PinWrite(b_p, b_s, 0);
	GPIO_PinWrite(c_p, c_s, 0);
	GPIO_PinWrite(d_p, d_s, 0);
}

void clear_rgb() {
	GPIO_PinWrite(r1_p, r1_s, 0);
	GPIO_PinWrite(g1_p, g1_s, 0);
	GPIO_PinWrite(b1_p, b1_s, 0);
	GPIO_PinWrite(r2_p, r2_s, 0);
	GPIO_PinWrite(g2_p, g2_s, 0);
	GPIO_PinWrite(b2_p, b2_s, 0);
}

void set_rgb(int c){
	if (c & 4){
		GPIO_PinWrite(r1_p, r1_s, 1);
		GPIO_PinWrite(r2_p, r2_s, 1);
	} else {
		GPIO_PinWrite(r1_p, r1_s, 0);
		GPIO_PinWrite(r2_p, r2_s, 0);
	}
	if (c & 2){
		GPIO_PinWrite(g1_p, g1_s, 1);
		GPIO_PinWrite(g2_p, g2_s, 1);
	} else {
		GPIO_PinWrite(g1_p, g1_s, 0);
		GPIO_PinWrite(g2_p, g2_s, 0);
	}
	if (c & 1){
		GPIO_PinWrite(b1_p, b1_s, 1);
		GPIO_PinWrite(b2_p, b2_s, 1);
	} else {
		GPIO_PinWrite(b1_p, b1_s, 0);
		GPIO_PinWrite(b2_p, b2_s, 0);
	}
}

void timer_setup() {
	NVIC_EnableIRQ(PIT0_IRQn); //enable PIT0 interrupts
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = PIT_MCR_MDIS(0);
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1); //clear interrupt flag
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 3; //set timer to 1/3 second
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE(1); //enable interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN(1);
}

void pin_setup() { 
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //Enable the clock to port B
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	gpio_pin_config_t config;
	config.pinDirection = kGPIO_DigitalOutput;
	config.outputLogic = 0;	
	
	PORTB->PCR[9] = PORT_PCR_MUX(001); //R1
	r1_p = GPIOB;
	r1_s = 9;
	GPIO_PinInit(r1_p, r1_s, &config);
	
	PORTC->PCR[17] = PORT_PCR_MUX(001); //G1
	g1_p = GPIOC;
	g1_s = 17;
	GPIO_PinInit(g1_p, g1_s, &config);

	PORTB->PCR[23] = PORT_PCR_MUX(001); //B1
	b1_p = GPIOB;
	b1_s = 23;
	GPIO_PinInit(b1_p, b1_s, &config);
	
	PORTC->PCR[16] = PORT_PCR_MUX(001); //R2
	r2_p = GPIOC;
	r2_s = 16;
	GPIO_PinInit(r2_p, r2_s, &config);
	
	PORTC->PCR[2] = PORT_PCR_MUX(001); //G2
	g2_p = GPIOC;
	g2_s = 2;
	GPIO_PinInit(g2_p, g2_s, &config);
	
	PORTC->PCR[3] = PORT_PCR_MUX(001); //B2
	b2_p = GPIOC;
	b2_s = 3;
	GPIO_PinInit(b2_p, b2_s, &config);
	
	PORTC->PCR[0] = PORT_PCR_MUX(001); //A
	a_p = GPIOC;
	a_s = 0;
	GPIO_PinInit(a_p, a_s, &config);
	
	PORTC->PCR[9] = PORT_PCR_MUX(001); //B
	b_p = GPIOC;
	b_s = 9;
	GPIO_PinInit(b_p, b_s, &config);
	
	PORTC->PCR[8] = PORT_PCR_MUX(001); //C
	c_p = GPIOC;
	c_s = 8;
	GPIO_PinInit(c_p, c_s, &config);
	
	PORTC->PCR[1] = PORT_PCR_MUX(001); //D
	d_p = GPIOC;
	d_s = 1;
	GPIO_PinInit(d_p, d_s, &config);
	
	PORTD->PCR[0] = PORT_PCR_MUX(001); //LAT
	lat_p = GPIOD;
	lat_s = 0;
	GPIO_PinInit(lat_p, lat_s, &config);
	
	PORTC->PCR[4] = PORT_PCR_MUX(001); //OE
	oe_p = GPIOC;
	oe_s = 4;
	GPIO_PinInit(oe_p, oe_s, &config);
	
	PORTC->PCR[12] = PORT_PCR_MUX(001); //CLK
	clk_p = GPIOC;
	clk_s = 12;
	GPIO_PinInit(clk_p, clk_s, &config);
		
}

void begin() {
	clear_clk();
	clear_lat();
	clear_rgb();
	clear_row();
	set_oe();
}

int main() {
 
	row = 0; 
	color = 0;
	pin_setup();
	begin();
	//timer_setup();
	
	set_oe(); //disable LED output
	set_lat(); //latch data from previous interrupt
	
	clear_row();
	
	//set_row(row);
	GPIO_PinWrite(a_p, a_s, 0);
	GPIO_PinWrite(b_p, b_s, 0);
	GPIO_PinWrite(c_p, c_s, 1);
	GPIO_PinWrite(d_p, d_s, 0);
	
	clear_lat(); //latch down
	clear_oe(); //reenable output
	
	for (int i = 0; i < 128; i++){
		clear_clk();
		clear_rgb();
		GPIO_PinWrite(r1_p, r1_s, 1);
		GPIO_PinWrite(r2_p, r2_s, 1);
		set_clk();
	}
	
	clear_clk();
	
	set_lat();
	
	while(1);
	

}


//extern "C" 
void PIT0_IRQHandler(void)
{
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(0); //disable interrupts
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1); //reset flag
	
	set_oe(); //disable LED output
	set_lat(); //latch data from previous interrupt
	
	set_row(row);
	
	clear_lat(); //latch down
	clear_oe(); //reenable output
	
	for (int i = 0; i < 32; i++){
		clear_clk();
		clear_rgb();
		set_rgb(color);
		set_clk();
	}
	
	if (row == 15){
		row = 0;
		if (color == 7){
			color = 1;
		} else {
			color ++;
		}
	} else {
		row ++;
	}
	
	clear_clk();
	
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(1); //reenable interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE(1);
}

 
