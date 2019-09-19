#include "address_map_arm.h"
#include "GSInterface.h"

volatile int lookUpTable[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7c, 0x39, 0x5E, 0x79, 0x71}; //hex disp
volatile unsigned char *(HEX3_HEX0_BASE_ptr) = (unsigned char *)HEX3_HEX0_BASE; //first 4
volatile unsigned char *(HEX5_HEX4_BASE_ptr) = (unsigned char *)HEX5_HEX4_BASE; //next 2
volatile int ledLookUpTable[9] = {0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010,0x00000020, 0x00000040, 0x00000080, 0x00000100,}; //input for specific lights
volatile int *LED_ptr = (int *)LED_BASE; //for LED's
volatile int DELAY_LENGTH = 7000000; 
volatile int delay_count;
unsigned char x[2]; //two bytes (char is one byte, size 2)

void GSInit(void)
{
	// Configure for full resolution, +/- 2G operation
	WriteGSRegister(0x31, 8);
	
	// Configure for 200Hz sampling rate.  Sampling rate of 10 == 100Hz.  Each incremement after that is a doubling.
	WriteGSRegister(0x2c, 11);
	
	// Configure to begin measurement
	WriteGSRegister(0x2d, 8);
}

//display the 3 axes in 6 hex displays
void AxiesDisplayHex( unsigned char xValues[] /*,unsigned char yValues[],unsigned char zValues[]*/)
{
  // displaying x values (first byte)
  *(HEX3_HEX0_BASE_ptr+2) = lookUpTable[(int)(xValues[0] & 0x0F)];
  *(HEX3_HEX0_BASE_ptr +3) = lookUpTable[(int)((xValues[0] & 0xF0) >> 4)];

  //displaying x values (second byte)
  *(HEX3_HEX0_BASE_ptr +0) = lookUpTable[(int)(xValues[1] & 0x0F)];
  *(HEX3_HEX0_BASE_ptr +1) = lookUpTable[(int)((xValues[1] & 0xF0)) >> 4];
}

int main(void) 
{
	I2C0Init(); //initialize communications

	// Read the GS DEVID here to make sure you are talking to it
	if(ReadGSRegister(GS_DEVID) == 0xE5)
	{
		// Initialize the GS unit for sampling
		GSInit();
	
		while (1)
		{		
			MultiReadGS(GS_DATAX0, x, 2); //read the x position (GS_DATAX0 and GS_DATAX1) of Grav-sensor
			
			signed short xInt = ((x[1] << 8) | x[0]); //concat the two together
			
			unsigned int v;
			v = x[0];
			x[0] = x[1];
			x[1]=v;
			
			
			AxiesDisplayHex(x);
			
			int i = 0;
			printf("xInt: %d \n",xInt);

			for (i = 0; i < 150000000;i++){}
   
			// OK, start sampling and displaying
		}
	}
}