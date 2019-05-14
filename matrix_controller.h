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
		static void update_pixel(int x, int y, bool r, bool g, bool b);
		static void write_line_vert(int start_x, int end_x, int y, bool r, bool g, bool b);
		static void write_line_horiz(int start_y, int end_y, int x, bool r, bool g, bool b);
		static void write_matrix();
		static void init_matrix();
		static void init_timer();
		static void init_gpio();
		typedef struct {
			bool r;
			bool g;
			bool b;
		} pixel_t;

	private:
		static pixel_t matrix[32][32];
		static void set_rgb();
		static void clear_row();
		static void set_row(int row);
		static int rgb_to_bits(int r, int g, int b);
		static void clear_rgb();
		static void set_rgb_top(int c);
		static void set_rgb_bottom(int c);
};