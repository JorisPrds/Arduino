/*******************************************************************************
 
 * ArduCAM.c
 
 * ArduCAM Library porting for OV5642 only on PIC32
 
 * Source File
 
 * Provides function implementation
 
 * This file is strongly inspired by the original ArduCAM Arduino library
 * See : https://github.com/ArduCAM/Arduino
 
 * Joris PAREDES, August 31, 2020
 
 * CYBENERGY

*******************************************************************************/

#include "ArduCAM.h"


uint8_t sensor_addr = 0x78,
        m_fmt = JPEG;

void ArduCAM_InitCAM()
{
	ArduCAM_wrSensorReg16_8(0x3008, 0x80);
	if (m_fmt == RAW){
		//Init and set 640x480;
		ArduCAM_wrSensorRegs16_8(OV5642_1280x960_RAW);	
		ArduCAM_wrSensorRegs16_8(OV5642_640x480_RAW);	
	}
	else{	
		ArduCAM_wrSensorRegs16_8(OV5642_QVGA_Preview);
		delay(100);
		if (m_fmt == JPEG)
		{
			delay(100);
			ArduCAM_wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA);
			ArduCAM_wrSensorRegs16_8(ov5642_320x240);
			delay(100);
			ArduCAM_wrSensorReg16_8(0x3818, 0xa8);
			ArduCAM_wrSensorReg16_8(0x3621, 0x10);
			ArduCAM_wrSensorReg16_8(0x3801, 0xb0);
			ArduCAM_wrSensorReg16_8(0x4407, 0x04);
			ArduCAM_wrSensorReg16_8(0x5888, 0x00);
			ArduCAM_wrSensorReg16_8(0x5000, 0xFF); 
		}
		else
		{
			uint8_t reg_val;
			ArduCAM_wrSensorReg16_8(0x4740, 0x21);
			ArduCAM_wrSensorReg16_8(0x501e, 0x2a);
			ArduCAM_wrSensorReg16_8(0x5002, 0xf8);
			ArduCAM_wrSensorReg16_8(0x501f, 0x01);
			ArduCAM_wrSensorReg16_8(0x4300, 0x61);
			ArduCAM_rdSensorReg16_8(0x3818, &reg_val);
			ArduCAM_wrSensorReg16_8(0x3818, (reg_val | 0x60) & 0xff);
			ArduCAM_rdSensorReg16_8(0x3621, &reg_val);
			ArduCAM_wrSensorReg16_8(0x3621, reg_val & 0xdf);
		}
	}
}

void ArduCAM_flush_fifo(void)
{
	ArduCAM_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void ArduCAM_start_capture(void)
{
	ArduCAM_write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void ArduCAM_clear_fifo_flag(void )
{
	ArduCAM_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

uint32_t ArduCAM_read_fifo_length(void)
{
	uint32_t len1,len2,len3,length=0;
	len1 = ArduCAM_read_reg(FIFO_SIZE1);
	len2 = ArduCAM_read_reg(FIFO_SIZE2);
	len3 = ArduCAM_read_reg(FIFO_SIZE3) & 0x7f;
	length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;	
}

void ArduCAM_set_fifo_burst()
{
	uint8_t data = BURST_FIFO_READ;
    SPI3_WriteRead (&data, 1, NULL, 0);
}

void ArduCAM_CS_HIGH(void)
{
	PS_CS_Set();	
}
void ArduCAM_CS_LOW(void)
{
	PS_CS_Clear();
}

uint8_t ArduCAM_read_fifo(void)
{
	uint8_t data;
	data = ArduCAM_bus_read(SINGLE_FIFO_READ);
	return data;
}

uint8_t ArduCAM_read_reg(uint8_t addr)
{
	uint8_t data;	
	data = ArduCAM_bus_read(addr & 0x7F);
	return data;
}

void ArduCAM_write_reg(uint8_t addr, uint8_t data)
{
	ArduCAM_bus_write(addr | 0x80, data);
}

//Set corresponding bit  
void ArduCAM_set_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = ArduCAM_read_reg(addr);
	ArduCAM_write_reg(addr, temp | bit);
}
//Clear corresponding bit 
void ArduCAM_clear_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = ArduCAM_read_reg(addr);
	ArduCAM_write_reg(addr, temp & (~bit));
}

//Get corresponding bit status
uint8_t ArduCAM_get_bit(uint8_t addr, uint8_t bit)
{
  uint8_t temp;
  temp = ArduCAM_read_reg(addr);
  temp = temp & bit;
  return temp;
}

//Set ArduCAM working mode
//MCU2LCD_MODE: MCU writes the LCD screen GRAM
//CAM2LCD_MODE: Camera takes control of the LCD screen
//LCD2MCU_MODE: MCU read the LCD screen GRAM
void ArduCAM_set_mode(uint8_t mode)
{
  switch (mode)
  {
    case MCU2LCD_MODE:
      ArduCAM_write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
      break;
    case CAM2LCD_MODE:
      ArduCAM_write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);
      break;
    case LCD2MCU_MODE:
      ArduCAM_write_reg(ARDUCHIP_MODE, LCD2MCU_MODE);
      break;
    default:
      ArduCAM_write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
      break;
  }
}

uint8_t ArduCAM_bus_write(int address,int value)
{	
	ArduCAM_CS_LOW();
	uint8_t data [2] = {address, value};
	SPI3_Write (data, 2);
	ArduCAM_CS_HIGH();
	return 1;
}

uint8_t ArduCAM_bus_read(int address)
{
	uint8_t value [2];
	ArduCAM_CS_LOW();
	SPI3_WriteRead(&address, 1, value, 2);
	// take the SS pin high to de-select the chip:
	ArduCAM_CS_HIGH();
	return value [1];
}

void OV5642_set_RAW_size(uint8_t size)
{
	switch (size)
	{
		case OV5642_640x480:
			ArduCAM_wrSensorRegs16_8(OV5642_1280x960_RAW);	
			ArduCAM_wrSensorRegs16_8(OV5642_640x480_RAW);	
		break;
		case OV5642_1280x960:
			ArduCAM_wrSensorRegs16_8(OV5642_1280x960_RAW);	
		break;
		case OV5642_1920x1080:
			ArduCAM_wrSensorRegs16_8(ov5642_RAW);
			ArduCAM_wrSensorRegs16_8(OV5642_1920x1080_RAW);
		break;
		case OV5642_2592x1944:
			ArduCAM_wrSensorRegs16_8(ov5642_RAW);
		break;
	} 		
}

void OV5642_set_JPEG_size(uint8_t size)
{
	uint8_t reg_val;

	switch (size)
	{
		case OV5642_320x240:
			ArduCAM_wrSensorRegs16_8(ov5642_320x240);
			break;
		case OV5642_640x480:
			ArduCAM_wrSensorRegs16_8(ov5642_640x480);
			break;
		case OV5642_1024x768:
			ArduCAM_wrSensorRegs16_8(ov5642_1024x768);
			break;
		case OV5642_1280x960:
			ArduCAM_wrSensorRegs16_8(ov5642_1280x960);
			break;
		case OV5642_1600x1200:
			ArduCAM_wrSensorRegs16_8(ov5642_1600x1200);
			break;
		case OV5642_2048x1536:
			ArduCAM_wrSensorRegs16_8(ov5642_2048x1536);
			break;
		case OV5642_2592x1944:
			ArduCAM_wrSensorRegs16_8(ov5642_2592x1944);
			break;
		default:
			ArduCAM_wrSensorRegs16_8(ov5642_320x240);
			break;
	}
}

void ArduCAM_set_format(uint8_t fmt)
{
  if (fmt == BMP)
    m_fmt = BMP;
  else if(fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}
	
	
void OV5642_set_Light_Mode(uint8_t Light_Mode)
{
	switch(Light_Mode)
	{
		case Advanced_AWB:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x5192 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5191 ,0xf8);
			ArduCAM_wrSensorReg16_8(0x518d ,0x26);
			ArduCAM_wrSensorReg16_8(0x518f ,0x42);
			ArduCAM_wrSensorReg16_8(0x518e ,0x2b);
			ArduCAM_wrSensorReg16_8(0x5190 ,0x42);
			ArduCAM_wrSensorReg16_8(0x518b ,0xd0);
			ArduCAM_wrSensorReg16_8(0x518c ,0xbd);
			ArduCAM_wrSensorReg16_8(0x5187 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5188 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5189 ,0x56);
			ArduCAM_wrSensorReg16_8(0x518a ,0x5c);
			ArduCAM_wrSensorReg16_8(0x5186 ,0x1c);
			ArduCAM_wrSensorReg16_8(0x5181 ,0x50);
			ArduCAM_wrSensorReg16_8(0x5184 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5182 ,0x11);
			ArduCAM_wrSensorReg16_8(0x5183 ,0x0 );	
		break;
		case Simple_AWB:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5183 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5191 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5192 ,0x00);
		break;
		case Manual_day:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x7 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x32);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x5 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0x36);
		break;
		case Manual_A:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x88);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x8 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0xb6);
		break;
		case Manual_cwf:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x6 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x13);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x7 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0xe2);
		break;
		case Manual_cloudy:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x7 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x88);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x5 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0x0);
		break;
		default :
		break; 
	}
}


	
void OV5642_set_Color_Saturation(uint8_t Color_Saturation)
{
	switch(Color_Saturation)
	{
		case Saturation4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
		case Saturation3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x70);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x70);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
		case Saturation2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x60);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x60);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
		case Saturation1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x50);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x50);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
		case Saturation0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;		
		case Saturation_1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
			case Saturation_2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
			case Saturation_3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
			case Saturation_4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
		break;
	}
}
	
	
void OV5642_set_Brightness(uint8_t Brightness)
{
	switch(Brightness)
	{
		case Brightness4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Brightness3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;	
		case Brightness2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Brightness1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Brightness0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;	
		case Brightness_1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
		break;	
		case Brightness_2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
		break;	
		case Brightness_3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
		break;	
		case Brightness_4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
		break;	
	}
}	
	
void OV5642_set_Contrast(uint8_t Contrast)
{
	switch(Contrast)
	{
		case Contrast4:
		ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x30);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x2c);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x2c);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x28);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x28);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x24);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x24);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x20);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast_1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x1C);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x1C);
			ArduCAM_wrSensorReg16_8(0x558a ,0x1C);
		break;
		case Contrast_2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x18);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast_3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x14);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x14);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
		case Contrast_4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x10);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
		break;
	}		
}	
	
void OV5642_set_hue(uint8_t degree)
{
	switch(degree)
	{
		case degree_180:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x00);
			ArduCAM_wrSensorReg16_8(0x558a ,0x32);
		break;
		case degree_150:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x32);
		break;
		case degree_120:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x32);
		break;
		case degree_90:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x80);
			ArduCAM_wrSensorReg16_8(0x558a ,0x02);
		break;
		case degree_60:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x02);
		break;
		case degree_30:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x02);
		break;
		case degree_0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x00);
			ArduCAM_wrSensorReg16_8(0x558a ,0x01);
		break;
		case degree30:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x01);
		break;
		case degree60:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x01);
		break;
		case degree90:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x80);
			ArduCAM_wrSensorReg16_8(0x558a ,0x31);
		break;
		case degree120:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x31);
		break;
		case degree150:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x31);
		break;
	}
}
	
	
void OV5642_set_Special_effects(uint8_t Special_effect)
{
	switch(Special_effect)
	{
		case Bluish:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0xa0);
			ArduCAM_wrSensorReg16_8(0x5586 ,0x40);
		break;
		case Greenish:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x60);
			ArduCAM_wrSensorReg16_8(0x5586 ,0x60);
		break;
		case Reddish:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5586 ,0xc0);
		break;
		case BW:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5586 ,0x80);
		break;
		case Negative:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x40);
		break;
		
			case Sepia:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5586 ,0xa0);
		break;
		case Normal:
			ArduCAM_wrSensorReg16_8(0x5001 ,0x7f);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x00);		
		break;		
	}
}
	
void OV5642_set_Exposure_level(uint8_t level)
{
	switch(level)
	{
		case Exposure_17_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x08);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x08);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure_13_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure_10_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x41);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure_07_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x51);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure_03_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x61);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure_default:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x61);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure03_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x71);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
		break;
		case Exposure07_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x80);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
		break;
		case Exposure10_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x90);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
		break;
		case Exposure13_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x91);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
		break;
		case Exposure17_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x60);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0xa0);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x60);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
		break;
	}
}
	
void OV5642_set_Sharpness(uint8_t Sharpness)
{
	switch(Sharpness)
	{
		case Auto_Sharpness_default:
			ArduCAM_wrSensorReg16_8(0x530A ,0x00);
			ArduCAM_wrSensorReg16_8(0x530c ,0x0 );
			ArduCAM_wrSensorReg16_8(0x530d ,0xc );
			ArduCAM_wrSensorReg16_8(0x5312 ,0x40);
		break;
		case Auto_Sharpness1:
			ArduCAM_wrSensorReg16_8(0x530A ,0x00);
			ArduCAM_wrSensorReg16_8(0x530c ,0x4 );
			ArduCAM_wrSensorReg16_8(0x530d ,0x18);
			ArduCAM_wrSensorReg16_8(0x5312 ,0x20);
		break;
		case Auto_Sharpness2:
			ArduCAM_wrSensorReg16_8(0x530A ,0x00);
			ArduCAM_wrSensorReg16_8(0x530c ,0x8 );
			ArduCAM_wrSensorReg16_8(0x530d ,0x30);
			ArduCAM_wrSensorReg16_8(0x5312 ,0x10);
		break;
		case Manual_Sharpnessoff:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x00);
			ArduCAM_wrSensorReg16_8(0x531f ,0x00);
		break;
		case Manual_Sharpness1:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x04);
			ArduCAM_wrSensorReg16_8(0x531f ,0x04);
		break;
		case Manual_Sharpness2:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x08);
			ArduCAM_wrSensorReg16_8(0x531f ,0x08);
		break;
		case Manual_Sharpness3:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x0c);
			ArduCAM_wrSensorReg16_8(0x531f ,0x0c);
		break;
		case Manual_Sharpness4:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x0f);
			ArduCAM_wrSensorReg16_8(0x531f ,0x0f);
		break;
		case Manual_Sharpness5:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x1f);
			ArduCAM_wrSensorReg16_8(0x531f ,0x1f);
		break;
	}
}
	
void OV5642_set_Mirror_Flip(uint8_t Mirror_Flip)
{

	uint8_t reg_val;
	switch(Mirror_Flip)
	{
		case MIRROR:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = reg_val|0x00;
			reg_val = reg_val&0x9F;
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = reg_val|0x20;
			ArduCAM_wrSensorReg16_8(0x3621, reg_val );
		
		break;
		case FLIP:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = reg_val|0x20;
			reg_val = reg_val&0xbF;
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = reg_val|0x20;
			ArduCAM_wrSensorReg16_8(0x3621, reg_val );
		break;
		case MIRROR_FLIP:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = reg_val|0x60;
			reg_val = reg_val&0xFF;
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = reg_val&0xdf;
			ArduCAM_wrSensorReg16_8(0x3621, reg_val );
		break;
		case Normal:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = reg_val|0x40;
			reg_val = reg_val&0xdF;
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = reg_val&0xdf;
			ArduCAM_wrSensorReg16_8(0x3621, reg_val );
		break;
	}
}
	
	
void OV5642_set_Compress_quality(uint8_t quality)
{
	switch(quality)
	{
		case high_quality:
			ArduCAM_wrSensorReg16_8(0x4407, 0x02);
			break;
		case default_quality:
			ArduCAM_wrSensorReg16_8(0x4407, 0x04);
			break;
		case low_quality:
			ArduCAM_wrSensorReg16_8(0x4407, 0x08);
			break;
	}
}
	
void OV5642_Test_Pattern(uint8_t Pattern)
{
	switch(Pattern)
	{
		case Color_bar:
			ArduCAM_wrSensorReg16_8(0x503d , 0x80);
			ArduCAM_wrSensorReg16_8(0x503e, 0x00);
			break;
		case Color_square:
			ArduCAM_wrSensorReg16_8(0x503d , 0x85);
			ArduCAM_wrSensorReg16_8(0x503e, 0x12);
			break;
		case BW_square:
			ArduCAM_wrSensorReg16_8(0x503d , 0x85);
			ArduCAM_wrSensorReg16_8(0x503e, 0x1a);
			break;
		case DLI:
			ArduCAM_wrSensorReg16_8(0x4741 , 0x4);
			break;
	}
}

	// Write 8 bit values to 8 bit register address
int ArduCAM_wrSensorRegs8_8(const struct sensor_reg reglist[])
{	
	int err = 0;
	uint16_t reg_addr = 0;
	uint16_t reg_val = 0;
	const struct sensor_reg *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff))
	{
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
		err = ArduCAM_wrSensorReg8_8(reg_addr, reg_val);
		next++;
	}
	return 1;
}

	// Write 16 bit values to 8 bit register address
int ArduCAM_wrSensorRegs8_16(const struct sensor_reg reglist[])
{
	int err = 0;
	unsigned int reg_addr, reg_val;
	const struct sensor_reg *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xffff))
	{
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
		err = ArduCAM_wrSensorReg8_16(reg_addr, reg_val);
		next++;
	}
	return 1;
}

// Write 8 bit values to 16 bit register address
int ArduCAM_wrSensorRegs16_8(const struct sensor_reg reglist[])
{
	int err = 0;
	unsigned int reg_addr;
	unsigned char reg_val;
	const struct sensor_reg *next = reglist;
	while ((reg_addr != 0xffff) | (reg_val != 0xff))
	{
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
		err = ArduCAM_wrSensorReg16_8(reg_addr, reg_val);
		next++;
	}
	return 1;
}

//I2C Array Write 16bit address, 16bit data
int ArduCAM_wrSensorRegs16_16(const struct sensor_reg reglist[])
{
	int err = 0;
	unsigned int reg_addr, reg_val;
	const struct sensor_reg *next = reglist;
	reg_addr = pgm_read_word(&next->reg);
	reg_val = pgm_read_word(&next->val);
	while ((reg_addr != 0xffff) | (reg_val != 0xffff))
	{
		err = ArduCAM_wrSensorReg16_16(reg_addr, reg_val);
		next++;
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
	}
	return 1;
}



// Read/write 8 bit value to/from 8 bit register address	
uint8_t ArduCAM_wrSensorReg8_8(int regID, int regDat)
{
	uint8_t data [2] = {regID & 0x00FF, regDat & 0x00FF};
	I2C2_Write (sensor_addr >> 1, data, 2);
    while (I2C2_IsBusy());
	if (I2C_ERROR_NONE != I2C2_ErrorGet()) return 0;
	delay(1);
	return 1;	
}
uint8_t ArduCAM_rdSensorReg8_8(uint8_t regID, uint8_t* regDat)
{	
	uint8_t data = regID & 0x00FF;
	I2C2_WriteRead (sensor_addr >> 1, &data, 1, regDat, 1);
    while (I2C2_IsBusy());
	delay(1);
	return 1;
}
// Read/write 16 bit value to/from 8 bit register address
uint8_t ArduCAM_wrSensorReg8_16(int regID, int regDat)
{
	uint8_t data [3] = {regID & 0x00FF, regDat >> 8, regDat & 0x00FF};
	I2C2_Write (sensor_addr >> 1, data, 3);
    while (I2C2_IsBusy());
	if (I2C_ERROR_NONE != I2C2_ErrorGet()) return 0;
	delay(1);
	return 1;
}
uint8_t ArduCAM_rdSensorReg8_16(uint8_t regID, uint16_t* regDat)
{
	uint8_t buf [2];
	I2C2_WriteRead (sensor_addr >> 1, &regID, 1, buf, 2);
    while (I2C2_IsBusy());
	*regDat = (buf [0] << 8) | buf [1];
	delay(1);
	return 1;
}

// Read/write 8 bit value to/from 16 bit register address
uint8_t ArduCAM_wrSensorReg16_8(int regID, int regDat)
{
	uint8_t data [3] = {regID >> 8, regID & 0x00FF, regDat & 0x00FF};
	I2C2_Write (sensor_addr >> 1, data, 3);
    while (I2C2_IsBusy());
	if (I2C_ERROR_NONE != I2C2_ErrorGet()) return 0;
	delay(1);
	return 1;
}
uint8_t ArduCAM_rdSensorReg16_8(uint16_t regID, uint8_t* regDat)
{
	uint8_t data [2] = {regID >> 8, regID & 0x00FF};
	I2C2_WriteRead (sensor_addr >> 1, data, 2, regDat, 1);
    while (I2C2_IsBusy());
	delay(1);
	return 1;
}

//I2C Write 16bit address, 16bit data
uint8_t ArduCAM_wrSensorReg16_16(int regID, int regDat)
{
	uint8_t data [4] = {regID >> 8, regID & 0x00FF, regDat >> 8, regDat & 0x00FF};
	I2C2_Write (sensor_addr >> 1, data, 4);
    while (I2C2_IsBusy());
	if (I2C_ERROR_NONE != I2C2_ErrorGet()) return 0;
	delay(1);
	return (1);
}

//I2C Read 16bit address, 16bit data
uint8_t ArduCAM_rdSensorReg16_16(uint16_t regID, uint16_t* regDat)
{
	uint8_t data	[2] = {regID >> 8, regID & 0x00FF},
			buf 	[2];
	
	I2C2_WriteRead (sensor_addr >> 1, data, 2, buf, 2);
    while (I2C2_IsBusy());
	*regDat = (buf [0] << 8) | buf [1];
	delay(1);
	return (1);
}
