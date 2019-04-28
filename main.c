#include <fsl_i2c.h>

int main() {
		
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	I2C0->C1 = I2C_C1_IICEN(1);
	I2C0->C1 = I2C_C1_IICIE(1);
	
	PORTE->PCR[24] = PORT_PCR_MUX(5);
	PORTE->PCR[25] = PORT_PCR_MUX(5);
	
	I2C_Enable(I2C0, false);
	I2C_EnableInterrupts(I2C0, kI2C_GlobalInterruptEnable);
	
	i2c_master_config_t conf = {
		.enableMaster = true,
		.enableStopHold = false,
    .baudRate_Bps = 100000,
    .glitchFilterWidth = 0
	};
	
	I2C_MasterInit(I2C0, &conf, 21000000);
	
	i2c_master_transfer_t transfer = {
		.slaveAddress = 0,
		.direction = kI2C_Write,
		.subaddress = 0,
		.subaddressSize = 0,
		.data = NULL, // TODO fill with data. Type is uint8_t
		.dataSize = 8, // this is specified in the IMU library as "write8"
		.flags = kI2C_TransferDefaultFlag
	};
	
	I2C_MasterTransferBlocking(I2C0, &transfer);
	
	return 0;
}