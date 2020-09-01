/*******************************************************************************
 
 * ArduCAM.h
 
 * ArduCAM Library porting for OV5642 only on PIC32
 
 * Header File
 
 * Provides constants and function definitions
 
 * This file is strongly inspired by the original ArduCAM Arduino library
 * See : https://github.com/ArduCAM/Arduino
 
 * Joris PAREDES, August 31, 2020
 
 * CYBENERGY

*******************************************************************************/

#ifndef ArduCAM_H
#define ArduCAM_H

#include "definitions.h"

#define fontbyte(x) pgm_read_byte(&cfont.font[x])  


#define swap(type, i, j) {type t = i; i = j; j = t;}

#define regtype volatile uint32_t
#define regsize uint32_t

#define PROGMEM

#define pgm_read_byte(x)        (*((char *)x))
#define pgm_read_word(x)        ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
#define pgm_read_byte_near(x)   (*((char *)x))
#define pgm_read_byte_far(x)    (*((char *)x))
#define pgm_read_word_near(x)   ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
#define pgm_read_word_far(x)    ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x))))
#define PSTR(x)  x



/****************************************************/
/* Sensor related definition 												*/
/****************************************************/
#define BMP 	0
#define JPEG	1
#define RAW	  2

#define OV5642		3





#define OV5642_320x240 		0	//320x240
#define OV5642_640x480		1	//640x480
#define OV5642_1024x768		2	//1024x768
#define OV5642_1280x960 	3	//1280x960
#define OV5642_1600x1200	4	//1600x1200
#define OV5642_2048x1536	5	//2048x1536
#define OV5642_2592x1944	6	//2592x1944
#define OV5642_1920x1080  7



//Light Mode

#define Auto                 0
#define Sunny                1
#define Cloudy               2
#define Office               3
#define Home                 4

#define Advanced_AWB         0
#define Simple_AWB           1
#define Manual_day           2
#define Manual_A             3
#define Manual_cwf           4
#define Manual_cloudy        5



//Color Saturation 

#define Saturation4          0
#define Saturation3          1
#define Saturation2          2
#define Saturation1          3
#define Saturation0          4
#define Saturation_1         5
#define Saturation_2         6
#define Saturation_3         7
#define Saturation_4         8

//Brightness

#define Brightness4          0
#define Brightness3          1
#define Brightness2          2
#define Brightness1          3
#define Brightness0          4
#define Brightness_1         5
#define Brightness_2         6
#define Brightness_3         7
#define Brightness_4         8


//Contrast

#define Contrast4            0
#define Contrast3            1
#define Contrast2            2
#define Contrast1            3
#define Contrast0            4
#define Contrast_1           5
#define Contrast_2           6
#define Contrast_3           7
#define Contrast_4           8



#define degree_180            0
#define degree_150            1
#define degree_120            2
#define degree_90             3
#define degree_60             4
#define degree_30             5
#define degree_0              6
#define degree30              7
#define degree60              8
#define degree90              9
#define degree120             10
#define degree150             11



//Special effects

#define Antique                      0
#define Bluish                       1
#define Greenish                     2
#define Reddish                      3
#define BW                           4
#define Negative                     5
#define BWnegative                   6
#define Normal                       7
#define Sepia                        8
#define Overexposure                 9
#define Solarize                     10
#define  Blueish                     11
#define Yellowish                    12

#define Exposure_17_EV                    0
#define Exposure_13_EV                    1
#define Exposure_10_EV                    2
#define Exposure_07_EV                    3
#define Exposure_03_EV                    4
#define Exposure_default                  5
#define Exposure03_EV                     6
#define Exposure07_EV                     7
#define Exposure10_EV                     8
#define Exposure13_EV                     9
#define Exposure17_EV                     10

#define Auto_Sharpness_default              0
#define Auto_Sharpness1                     1
#define Auto_Sharpness2                     2
#define Manual_Sharpnessoff                 3
#define Manual_Sharpness1                   4
#define Manual_Sharpness2                   5
#define Manual_Sharpness3                   6
#define Manual_Sharpness4                   7
#define Manual_Sharpness5                   8



#define Sharpness1                         0
#define Sharpness2                         1
#define Sharpness3                         2
#define Sharpness4                         3
#define Sharpness5                         4
#define Sharpness6                         5
#define Sharpness7                         6
#define Sharpness8                         7
#define Sharpness_auto                       8




#define EV3                                 0
#define EV2                                 1
#define EV1                                 2
#define EV0                                 3
#define EV_1                                4
#define EV_2                                5
#define EV_3                                6

#define MIRROR                              0
#define FLIP                                1
#define MIRROR_FLIP                         2




#define high_quality                         0
#define default_quality                      1
#define low_quality                          2

#define Color_bar                      0
#define Color_square                   1
#define BW_square                      2
#define DLI                            3


#define Night_Mode_On                  0
#define Night_Mode_Off                 1

#define Off                            0
#define Manual_50HZ                    1
#define Manual_60HZ                    2
#define Auto_Detection                 3

/****************************************************/
/* I2C Control Definition 													*/
/****************************************************/
#define I2C_ADDR_8BIT 0
#define I2C_ADDR_16BIT 1
#define I2C_REG_8BIT 0
#define I2C_REG_16BIT 1
#define I2C_DAT_8BIT 0
#define I2C_DAT_16BIT 1

/* Register initialization tables for SENSORs */
/* Terminating list entry for reg */
#define SENSOR_REG_TERM_8BIT                0xFF
#define SENSOR_REG_TERM_16BIT               0xFFFF
/* Terminating list entry for val */
#define SENSOR_VAL_TERM_8BIT                0xFF
#define SENSOR_VAL_TERM_16BIT               0xFFFF

//Define maximum frame buffer size
#if (defined OV2640_MINI_2MP)
#define MAX_FIFO_SIZE		0x5FFFF			//384KByte
#elif (defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined ARDUCAM_SHIELD_REVC)
#define MAX_FIFO_SIZE		0x7FFFF			//512KByte
#else
#define MAX_FIFO_SIZE		0x7FFFFF		//8MByte
#endif 

/****************************************************/
/* ArduChip registers definition 											*/
/****************************************************/
#define RWBIT									0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1       	0x00  //TEST register

#if !(defined OV2640_MINI_2MP)
	#define ARDUCHIP_FRAMES			  0x01  //FRAME control register, Bit[2:0] = Number of frames to be captured																		//On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#endif

#define ARDUCHIP_MODE      		0x02  //Mode register
#define MCU2LCD_MODE       		0x00
#define CAM2LCD_MODE       		0x01
#define LCD2MCU_MODE       		0x02

#define ARDUCHIP_TIM       		0x03  //Timming control
#if !(defined OV2640_MINI_2MP)
	#define HREF_LEVEL_MASK    		0x01  //0 = High active , 		1 = Low active
	#define VSYNC_LEVEL_MASK   		0x02  //0 = High active , 		1 = Low active
	#define LCD_BKEN_MASK      		0x04  //0 = Enable, 					1 = Disable
	#if (defined ARDUCAM_SHIELD_V2)
		#define PCLK_REVERSE_MASK  	0x08  //0 = Normal PCLK, 		1 = REVERSED PCLK
	#else
		#define PCLK_DELAY_MASK  		0x08  //0 = data no delay,		1 = data delayed one PCLK
	#endif
	//#define MODE_MASK          		0x10  //0 = LCD mode, 				1 = FIFO mode
#endif
//#define FIFO_PWRDN_MASK	   		0x20  	//0 = Normal operation, 1 = FIFO power down
//#define LOW_POWER_MODE			  0x40  	//0 = Normal mode, 			1 = Low power mode

#define ARDUCHIP_FIFO      		0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO			  0x06  //GPIO Write Register
#if !(defined (ARDUCAM_SHIELD_V2) || defined (ARDUCAM_SHIELD_REVC))
#define GPIO_RESET_MASK			0x01  //0 = Sensor reset,							1 =  Sensor normal operation
#define GPIO_PWDN_MASK			0x02  //0 = Sensor normal operation, 	1 = Sensor standby
#define GPIO_PWREN_MASK			0x04	//0 = Sensor LDO disable, 			1 = sensor LDO enable
#endif

#define BURST_FIFO_READ			0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ		0x3D  //Single FIFO read operation

#define ARDUCHIP_REV       		0x40  //ArduCHIP revision
#define VER_LOW_MASK       		0x3F
#define VER_HIGH_MASK      		0xC0

#define ARDUCHIP_TRIG      		0x41  //Trigger source
#define VSYNC_MASK         		0x01
#define SHUTTER_MASK       		0x02
#define CAP_DONE_MASK      		0x08

#define FIFO_SIZE1				0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  //Camera write FIFO size[18:16]


/****************************************************/


/****************************************************************/
/* define a structure for sensor register initialization values */
/****************************************************************/
struct sensor_reg {
	uint16_t reg;
	uint16_t val;
};



/****************************************************************/
/* define a structure for sensor register initialization values */
/****************************************************************/



void ArduCAM_InitCAM( void );

void ArduCAM_CS_HIGH(void);
void ArduCAM_CS_LOW(void);

void ArduCAM_flush_fifo(void);
void ArduCAM_start_capture(void);
void ArduCAM_clear_fifo_flag(void);
uint8_t ArduCAM_read_fifo(void);

uint8_t ArduCAM_read_reg(uint8_t addr);
void ArduCAM_write_reg(uint8_t addr, uint8_t data);	

uint32_t ArduCAM_read_fifo_length(void);
void ArduCAM_set_fifo_burst(void);

void ArduCAM_set_bit(uint8_t addr, uint8_t bit);
void ArduCAM_clear_bit(uint8_t addr, uint8_t bit);
uint8_t ArduCAM_get_bit(uint8_t addr, uint8_t bit);
void ArduCAM_set_mode(uint8_t mode);

uint8_t ArduCAM_bus_write(int address, int value);
uint8_t ArduCAM_bus_read(int address);	

// Write 8 bit values to 8 bit register address
int ArduCAM_wrSensorRegs8_8(const struct sensor_reg*);

// Write 16 bit values to 8 bit register address
int ArduCAM_wrSensorRegs8_16(const struct sensor_reg*);

// Write 8 bit values to 16 bit register address
int ArduCAM_wrSensorRegs16_8(const struct sensor_reg*);

// Write 16 bit values to 16 bit register address
int ArduCAM_wrSensorRegs16_16(const struct sensor_reg*);

// Read/write 8 bit value to/from 8 bit register address	
uint8_t ArduCAM_wrSensorReg8_8(int regID, int regDat);
uint8_t ArduCAM_rdSensorReg8_8(uint8_t regID, uint8_t* regDat);

// Read/write 16 bit value to/from 8 bit register address
uint8_t ArduCAM_wrSensorReg8_16(int regID, int regDat);
uint8_t rdSensorReg8_16(uint8_t regID, uint16_t* regDat);

// Read/write 8 bit value to/from 16 bit register address
uint8_t ArduCAM_wrSensorReg16_8(int regID, int regDat);
uint8_t ArduCAM_rdSensorReg16_8(uint16_t regID, uint8_t* regDat);

// Read/write 16 bit value to/from 16 bit register address
uint8_t ArduCAM_wrSensorReg16_16(int regID, int regDat);
uint8_t ArduCAM_rdSensorReg16_16(uint16_t regID, uint16_t* regDat);


void OV5642_set_JPEG_size(uint8_t size);


void OV5642_set_RAW_size (uint8_t size);

void OV5642_set_Light_Mode(uint8_t Light_Mode);

void OV5642_set_Color_Saturation(uint8_t Color_Saturation);

void OV5642_set_Brightness(uint8_t Brightness);

void OV5642_set_Contrast(uint8_t Contrast);
void OV5642_set_Special_effects(uint8_t Special_effect);



void OV5642_set_hue(uint8_t degree);
void OV5642_set_Exposure_level(uint8_t level);
void OV5642_set_Sharpness(uint8_t Sharpness);
void OV5642_set_Mirror_Flip(uint8_t Mirror_Flip);
void OV5642_set_Compress_quality(uint8_t quality);
void OV5642_Test_Pattern(uint8_t Pattern);



void ArduCAM_set_format(uint8_t fmt);



void transferBytes_(uint8_t * out, uint8_t * in, uint8_t size);
void transferBytes(uint8_t * out, uint8_t * in, uint32_t size);
inline void setDataBits(uint16_t bits);


#include "ov5642_regs.h"

void delay (uint32_t period_ms)
{
    CORETIMER_DelayMs(period_ms);
}



#endif
