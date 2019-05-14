
#include <fsl_device_registers.h>
#include <fsl_gpio.h>

/*
General scheme for sending data:
(1) clear (sets output to 0) : GPION->PCOR = (1 << bitnum);
(2) set (sets output to 1) : GPION->PSOR = (1 << bitnum);

*/


GPIO_Type *r1_p, *g1_p, *b1_p, *r2_p, *g2_p, *b2_p, *a_p, *b_p, *c_p, *d_p, *lat_p, *oe_p, *clk_p;
int r1_s, g1_s, b1_s, r2_s, g2_s, b2_s, a_s, b_s, c_s, d_s, lat_s, oe_s, clk_s;
int row;
int color;

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
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; //Enable the clock to port A
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	PORTB->PCR[9] = PORT_PCR_MUX(001); //R1
	PTB->PDDR |= (1 << 9);
	r1_p = GPIOB;
	r1_s = 9;
	
	PORTA->PCR[1] = PORT_PCR_MUX(001); //G1
	PTA->PDDR |= (1 << 1);
	g1_p = GPIOA;
	g1_s = 1;

	PORTB->PCR[23] = PORT_PCR_MUX(001); //B1
	PTB->PDDR |= (1 << 23);
	b1_p = GPIOB;
	b1_s = 23;
	
	PORTA->PCR[2] = PORT_PCR_MUX(001); //R2
	PTA->PDDR |= (1 << 2);
	r2_p = GPIOA;
	r2_s = 2;
	
	PORTC->PCR[2] = PORT_PCR_MUX(001); //G2
	PTC->PDDR |= (1 << 2);
	g2_p = GPIOC;
	g2_s = 2;
	
	PORTC->PCR[3] = PORT_PCR_MUX(001); //B2
	PTC->PDDR |= (1 << 3);
	b2_p = GPIOC;
	b2_s = 3;
	
	PORTB->PCR[2] = PORT_PCR_MUX(001); //A
	PTB->PDDR |= (1 << 2);
	a_p = GPIOB;
	a_s = 2;
	
	PORTB->PCR[3] = PORT_PCR_MUX(001); //B
	PTB->PDDR |= (1 << 3);
	b_p = GPIOB;
	b_s = 3;
	
	PORTB->PCR[11] = PORT_PCR_MUX(001); //C
	PTB->PDDR |= (1 << 11);
	c_p = GPIOB;
	c_s = 11;
	
	PORTC->PCR[10] = PORT_PCR_MUX(001); //D
	PTC->PDDR |= (1 << 10);
	d_p = GPIOC;
	d_s = 10;
	
	PORTD->PCR[0] = PORT_PCR_MUX(001); //LAT
	PTD->PDDR |= (1);
	lat_p = GPIOD;
	lat_s = 0;
	
	PORTC->PCR[4] = PORT_PCR_MUX(001); //OE
	PTC->PDDR |= (1 << 4);
	oe_p = GPIOC;
	oe_s = 4;
	
	PORTA->PCR[0] = PORT_PCR_MUX(001); //CLK
	PTA->PDDR |= (1);
	clk_p = GPIOA;
	clk_s = 1;
	
	GPIOA->PDDR = GPIO_PDDR_PDD(1);
	GPIOB->PDDR = GPIO_PDDR_PDD(1);
	GPIOC->PDDR = GPIO_PDDR_PDD(1);
	GPIOD->PDDR = GPIO_PDDR_PDD(1);
		
}

int main() {
 
	pin_setup();
	
	while(1){
		r1_p->PSOR = (1 << r1_s);
	}
	

}

void set_row(int r) {
	if (r & 8){
		a_p->PSOR |= (1 << a_s);
	} else {
		a_p->PCOR |= (1 << a_s);
	}
	if (r & 4){
		b_p->PSOR |= (1 << b_s);
	} else {
		b_p->PCOR |= (1 << b_s);
	}
	if (r & 2){
		c_p->PSOR |= (1 << c_s);
	} else {
		c_p->PCOR |= (1 << c_s);
	}
	if (r & 1){
		d_p->PSOR |= (1 << d_s);
	} else {
		d_p->PCOR |= (1 << d_s);
	}
}

void clear_rgb() {
	r1_p->PCOR |= (1 << r1_s);
	g1_p->PCOR |= (1 << g1_s);
	b1_p->PCOR |= (1 << b1_s);	
	r2_p->PCOR |= (1 << r2_s);	
	g2_p->PCOR |= (1 << g2_s);	
	b2_p->PCOR |= (1 << b2_s);
}

void set_rgb(int c){
	if (c & 4){
		r1_p->PSOR |= (1 << r1_s);
		r2_p->PSOR |= (1 << r2_s);
	} else {
		r1_p->PCOR |= (1 << r1_s);
		r2_p->PCOR |= (1 << r2_s);
	}
	if (c & 2){
		g1_p->PSOR |= (1 << g1_s);
		g2_p->PSOR |= (1 << g2_s);
	} else {
		g1_p->PCOR |= (1 << g1_s);
		g2_p->PCOR |= (1 << g2_s);
	}
	if (c & 1){
		b1_p->PSOR |= (1 << b1_s);
		b2_p->PSOR |= (1 << b2_s);
	} else {
		b1_p->PCOR |= (1 << b1_s);
		b2_p->PCOR |= (1 << b2_s);
	}
}


#define clear_clk() (clk_p->PCOR |= (1 << clk_s))
#define set_clk() (clk_p->PSOR |= (1 << clk_s))
#define clear_lat() (lat_p->PCOR |= (1 << lat_s))
#define set_lat() (lat_p->PSOR |= (1 << lat_s))
#define clear_oe() (oe_p->PCOR |= (1 << oe_s))
#define set_oe() (oe_p->PSOR |= (1 << oe_s))

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

 
