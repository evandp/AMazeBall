#include "matrix_controller.h"

/** 
	This represents the GPIO port that each pin resides on.
*/
GPIO_Type *r1_p, *g1_p, *b1_p, *r2_p, *g2_p, *b2_p, *a_p, *b_p, *c_p, *d_p, *lat_p, *oe_p, *clk_p;

/** 
	This represents the pin number of each pin.
*/
int r1_s, g1_s, b1_s, r2_s, g2_s, b2_s, a_s, b_s, c_s, d_s, lat_s, oe_s, clk_s;

/**
	This is the internal representation of the LED display; this is the data
	that is sent to the physical LED matrix panel.
*/
MatrixController::pixel_t MatrixController::matrix[32][32] = {};

	
/**
	Begins the MatrixController. Initializes the necessary pins for GPIO,
	allowing data to be sent from the board to the matrix. Initializes the
	matrix to have R, G, and B values of 0 at all pixels. Begins the PIT0 timer
	which will periodically update the LED matrix to match the internal 
	representation stored in [matrix].
	*/	
void MatrixController::init() {
	init_matrix();
	init_gpio();
	init_timer();
}

/**
  Sets all entries in matrix to have R, G, & B values of 0.
	Effectively, this means that there is no color produced on the LED matrix.
*/
void MatrixController::init_matrix() {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			pixel_t p = {
				0, // red
				0, // green
				0, // blue
			};
			matrix[i][j] = p;
		}
	}
}

/** 
	Sets up the PIT0 timer which periodically causes the LED matrix display
	to be updated. The matrix gets updated over 2 times a second. */
void MatrixController::init_timer() {
	NVIC_EnableIRQ(PIT0_IRQn); //enable PIT0 interrupts
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = PIT_MCR_MDIS(0);
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1); //clear interrupt flag
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 250; //set timer to 1/60 seconds
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE(1); //enable interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN(1);
	NVIC_SetPriority(PIT0_IRQn, 0);
}

/** 
	Sets up the pins which interface the board and the matrix to GPIO. 
	Also initializes the X_s, X_p variables, which allow for easier 
	updating of the board later. See the project report for details
	about which pins connect to which ports on the LED matrix.
*/

void MatrixController::init_gpio() {
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
	
	PORTB->PCR[2] = PORT_PCR_MUX(001); //A
	a_p = GPIOB;
	a_s = 2;
	GPIO_PinInit(a_p, a_s, &config);
	
	PORTB->PCR[3] = PORT_PCR_MUX(001); //B
	b_p = GPIOB;
	b_s = 3;
	GPIO_PinInit(b_p, b_s, &config);
	
	PORTB->PCR[10] = PORT_PCR_MUX(001); //C
	c_p = GPIOB;
	c_s = 10;
	GPIO_PinInit(c_p, c_s, &config);
	
	PORTB->PCR[11] = PORT_PCR_MUX(001); //D
	d_p = GPIOB;
	d_s = 11;
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

/**
	Clears the RBG pins, which are connected to the R1, G1, B1, R2, G2, B2 ports, to logic 0. 
*/	
void MatrixController::clear_rgb() {
	GPIO_PinWrite(r1_p, r1_s, 0);
	GPIO_PinWrite(g1_p, g1_s, 0);
	GPIO_PinWrite(b1_p, b1_s, 0);
	GPIO_PinWrite(r2_p, r2_s, 0);
	GPIO_PinWrite(g2_p, g2_s, 0);
	GPIO_PinWrite(b2_p, b2_s, 0);
}

/**
	Sets the R1, G1, B1 pins to the logic represented by the given color.
	@param c the 3-bit representation of the RGB value (R = MSB, B = LSB). 
*/
void MatrixController::set_rgb_top(int c) {
	if (c & 4){
		GPIO_PinWrite(r1_p, r1_s, 1);
	} else {
		GPIO_PinWrite(r1_p, r1_s, 0);
	}
	if (c & 2){
		GPIO_PinWrite(g1_p, g1_s, 1);
	} else {
		GPIO_PinWrite(g1_p, g1_s, 0);
	}
	if (c & 1){
		GPIO_PinWrite(b1_p, b1_s, 1);
	} else {
		GPIO_PinWrite(b1_p, b1_s, 0);
	}
}

/**
	Sets the R2, G2, B2 pins to the logic represented by the given color.
	@param c the 3-bit representation of the RGB value (R = MSB, B = LSB). 
*/
void MatrixController::set_rgb_bottom(int c) {
	if (c & 4){
		GPIO_PinWrite(r2_p, r2_s, 1);
	} else {
		GPIO_PinWrite(r2_p, r2_s, 0);
	}
	if (c & 2){
		GPIO_PinWrite(g2_p, g2_s, 1);
	} else {
		GPIO_PinWrite(g2_p, g2_s, 0);
	}
	if (c & 1){
		GPIO_PinWrite(b2_p, b2_s, 1);
	} else {
		GPIO_PinWrite(b2_p, b2_s, 0);
	}
}

	
/**
	Clears the row pins, which are connected to the A, B, C, D ports, to logic 0.
*/
void MatrixController::clear_row() {
	GPIO_PinWrite(a_p, a_s, 0);
	GPIO_PinWrite(b_p, b_s, 0);
	GPIO_PinWrite(c_p, c_s, 0);
	GPIO_PinWrite(d_p, d_s, 0);
}

/** 
	Sets the row pins, which are connected to the A, B, C, D ports,
	to the logic represented by the given row.
	@param row 4-bit representation of the desired row
*/
void MatrixController::set_row(int row) {
	if (row & 8){
		GPIO_PinWrite(d_p, d_s, 1);
	} else {
		GPIO_PinWrite(d_p, d_s, 0);
	}
	if (row & 4){
		GPIO_PinWrite(c_p, c_s, 1);
	} else {
		GPIO_PinWrite(c_p, c_s, 0);
	}
	if (row & 2){
		GPIO_PinWrite(b_p, b_s, 1);
	} else {
		GPIO_PinWrite(b_p, b_s, 0);
	}
	if (row & 1){
		GPIO_PinWrite(a_p, a_s, 1);
	} else {
		GPIO_PinWrite(a_p, a_s, 0);
	}
}

/**
	Combines the given RGB values into a single 3-bit integer.
	@param r RED value
	@param g GREEN value
	@param b BLUE value
	@return 3-bit representation of RGB values (R = MSB, B = LSB)
*/
int MatrixController::rgb_to_bits(int r, int g, int b) {
	return (r << 2) | (g << 1) | b;
}

/** 
	Sends data to the pins to updates the LED display to match 
	the values specified by [matrix]. The data is sent in this fashion:
	(1) The matrix output is disabled.
	(2) The next row to write to (1, .. 16) is selected (call it R)
	(3) Output is enabled and the latch is cleared.
	(4) The RGB values for the pixels at rows R and R + 16 are sent
			column-by-column
	(5) The latch is set to specify the end of this row of data.
*/
void MatrixController::write_matrix() {
	for (int row = 0; row < 16; row++) {
		set_oe();
		clear_row();
		set_row((row-1) % 16);
		clear_lat();
		clear_oe();
		for (int col = 0; col < 32; col++) {
			clear_clk();
			clear_rgb();
			pixel_t p1 = matrix[row][col];
			pixel_t p2 = matrix[row+16][col];
			int c1 = rgb_to_bits(p1.r, p1.g, p1.b);
			int c2 = rgb_to_bits(p2.r, p2.g, p2.b);
			set_rgb_top(c1);
			set_rgb_bottom(c2);
			set_clk();
		}
		set_lat();
	}
	set_oe();
}

/** 
	Sets the specifed pixel to the given color.
		@param x row
		@param y col
		@param r RED value
		@param g GREEN value
		@param b BLUE value
*/
void MatrixController::update_pixel(int x, int y, bool r, bool g, bool b) {
	pixel_t p = {r, g, b};
	matrix[x][y] = p;
}

/**
	Draws a vertical line starting at row = start_x, ending at row = end_x.
	Requires: start_x <= end_x
*/
void MatrixController::draw_line_vert(int start_x, int end_x, int y, bool r, bool g, bool b) {
	for (int i = start_x; i <= end_x; i++) {
		MatrixController::update_pixel(i, y, r, g, b);
	}
}

/**
	Draws a horizontal line starting at col = start_y, ending at col = end_y.
	Requires: start_y <= end_y
*/
void MatrixController::draw_line_horiz(int start_y, int end_y, int x, bool r, bool g, bool b) {
	for (int j = start_y; j <= end_y; j++) {
		MatrixController::update_pixel(x, j, r, g, b);
	}
}

/* Define macros for drawing segments of numbers. Inspired by 7 segment displays (used for analog clocks) */

#define draw_a_seg(x, y, r, g, b) (draw_line_horiz(x, x+2, y, r, g, b))
#define draw_d_seg(x, y, r, g, b) (draw_line_horiz(x, x+2, y+4, r, g, b))
#define draw_g_seg(x, y, r, g, b) (draw_line_horiz(x, x+2, y+2, r, g, b))
#define draw_b_seg(x, y, r, g, b) (draw_line_vert(y, y+2, x+2, r, g, b))
#define draw_f_seg(x, y, r, g, b) (draw_line_vert(y, y+2, x, r, g, b))
#define draw_c_seg(x, y, r, g, b) (draw_line_vert(y+2, y+4, x+2, r, g, b))
#define draw_e_seg(x, y, r, g, b) (draw_line_vert(y+2, y+4, x, r, g, b))


/** 
	Draws a number of the given color in the 5x3 rectangle with the specified top-left corner.
		@param y row of the top-left corner of the 5x3 rectangle
		@param x col of the top-left corner of teh 5x3 rectangle
		@param r RED value
		@param g GREEN value
		@param b BLUE value
*/
void MatrixController::draw_number(int num, int y, int x, bool r, bool g, bool b){
	switch (num){
		case 0:
			draw_a_seg(x, y, r, g, b);
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_d_seg(x, y, r, g, b);
			draw_e_seg(x, y, r, g, b);
			draw_f_seg(x, y, r, g, b);
			break;
		case 1:
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			break;
		case 2:
			draw_a_seg(x, y, r, g, b);
			draw_b_seg(x, y, r, g, b);
			draw_d_seg(x, y, r, g, b);
			draw_e_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
		case 3:
			draw_a_seg(x, y, r, g, b);
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_d_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
		case 4:
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_f_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
		case 5:
			draw_a_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_d_seg(x, y, r, g, b);
			draw_f_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
		case 6:
			draw_a_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_d_seg(x, y, r, g, b);
			draw_e_seg(x, y, r, g, b);
			draw_f_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
		case 7:
			draw_a_seg(x, y, r, g, b);
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			break;
		case 8:
			draw_a_seg(x, y, r, g, b);
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_d_seg(x, y, r, g, b);
			draw_e_seg(x, y, r, g, b);
			draw_f_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
		case 9:
			draw_a_seg(x, y, r, g, b);
			draw_b_seg(x, y, r, g, b);
			draw_c_seg(x, y, r, g, b);
			draw_f_seg(x, y, r, g, b);
			draw_g_seg(x, y, r, g, b);
			break;
	}
}

/**
	Handler for PIT0 interrupt, which updates the LED matrix display
	to match the internal representation stored by MatrixController.
*/
extern "C" void PIT0_IRQHandler(void)
{
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(0); //disable interrupts
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1); //reset flag
	MatrixController::write_matrix();
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(1); //reenable interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE(1);
}
