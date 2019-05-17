#include <fsl_device_registers.h>
#include <fsl_gpio.h>

#define clear_clk() (GPIO_PinWrite(clk_p, clk_s, 0))
#define set_clk() (GPIO_PinWrite(clk_p, clk_s, 1))
#define clear_lat() (GPIO_PinWrite(lat_p, lat_s, 0))
#define set_lat() (GPIO_PinWrite(lat_p, lat_s, 1))
#define clear_oe() (GPIO_PinWrite(oe_p, oe_s, 0))
#define set_oe() (GPIO_PinWrite(oe_p, oe_s, 1))

class MatrixController {
	
	public:	
	
	/**
		Begins the MatrixController. Allows RGB values to be written to
		the controller to display on the LED. Also starts the timer
		which periodically updates the LED matrix.
	*/
		static void init();
	
	/** Sets the specifed pixel to the given color.
				@param x row
				@param y col
				@param r RED value
				@param g GREEN value
				@param b BLUE value
	*/
		static void update_pixel(int x, int y, bool r, bool g, bool b);
	
	/** Draws a vertical line of the given color.
				@param start_x start row
				@param end_x end row
				@param y col
				@param r RED value
				@param g GREEN value
				@param b BLUE value
	*/
		static void draw_line_vert(int start_x, int end_x, int y, bool r, bool g, bool b);
	
	/** Draws a horizontal line of the given color.
				@param start_y start col
				@param end_y end col
				@param x row
				@param r RED value
				@param g GREEN value
				@param b BLUE value
	*/
		static void draw_line_horiz(int start_y, int end_y, int x, bool r, bool g, bool b);
		
	/** Draws a number of the given color in the 5x3 rectangle with the specified top-left corner.
				@param y row of the top-left corner of the 5x3 rectangle
				@param x col of the top-left corner of teh 5x3 rectangle
				@param r RED value
				@param g GREEN value
				@param b BLUE value
	*/
		static void draw_number(int num, int y, int x, bool r, bool g, bool b);
		
	/** 
		Updates the LED display to match the values specified by the data written
		to the MatrixController.
	*/	
		static void write_matrix();
		
	/**
		Internal representation for the RGB values of a pixel
	*/
		typedef struct {
			bool r;
			bool g;
			bool b;
		} pixel_t;

	private:
		
	/** Internal representation of the LED matrix; this gets written to the pins on the next timer interrupt. */
		static pixel_t matrix[32][32];
	
	/** Sets all entries in matrix to have R, G, & B values of 0. */
		static void init_matrix();
	
	/** Sets up the timer which periodically causes the LED matrix display to be updated. */
		static void init_timer();
	
	/** Sets up the pins which interface the board and the matrix to GPIO. */
		static void init_gpio();
	
	/** Clears the row pins A, B, C, D to logic 0. */
		static void clear_row();
	
	/** Sets the row pins A, B, C, D to the logic represented by the given row. */
		static void set_row(int row);
	
	/** Creates single 3-bit integer representing given RGB values. */ 
		static int rgb_to_bits(int r, int g, int b);
		
	/** Clears the R1, G1, B1, R2, G2, B2 pins to logic 0. */	
		static void clear_rgb();
		
	/** Sets the R1, G1, B1, pins to the logic represented by the given color. */
		static void set_rgb_top(int c);
		
	/** Sets the R2, G2, B2, pins to the logic represented by the given color. */
		static void set_rgb_bottom(int c);
};