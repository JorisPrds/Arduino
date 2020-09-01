/*******************************************************************************

 * Main Source File
 
 * JorisPrds, August 31, 2020

*******************************************************************************/

// *****************************************************************************
// Section: Included Files
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <string.h>
#include "drivers/ArduCAM.h"

// *****************************************************************************
// Section: Global Variables
// *****************************************************************************

// File System
SYS_FS_HANDLE filehandle;
bool fs_ready = false;

// Camera
bool capture_done = false;

// *****************************************************************************
// Section: Local Functions Definition
// *****************************************************************************

// File System
uint8_t FS_Init (void);

// Camera
void CAM_Init (void);
void CAM_Application (void);

// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    // Wait for File System Initialization
    while (0 == FS_Init ())
    {
        // Keep the SDSPI driver and file system service running,
        // otherwise, the initialization would always fail
        SYS_Tasks();
    }
    
    CAM_Init();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        if (!capture_done) CAM_Application();
        else SYS_FS_Unmount("/mnt/myDrive");
        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

// *****************************************************************************
// Section: Application Functions Source code
// *****************************************************************************

void CAM_Application (void)
{
    char str[8];
    uint8_t buf[256];
    static int i = 0;
    static int k = 0;
    uint8_t temp = 0, temp_last = 0;
    uint32_t length = 0;
    bool is_header = false;
    SYS_FS_HANDLE outFile = SYS_FS_HANDLE_INVALID;
    //Flush the FIFO
    ArduCAM_flush_fifo();
    //Clear the capture done flag
    ArduCAM_clear_fifo_flag();
    //Start capture
    ArduCAM_start_capture();
    printf("start Capture.\n");
    while(!ArduCAM_get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
    printf("Capture Done.\n");  
    length = ArduCAM_read_fifo_length();
    printf("The fifo length is :");
    printf ("%d\n", length);
    if (length >= MAX_FIFO_SIZE) 
    {
        printf("Over size.\n");
        return ;
    }
    if (length == 0 ) //0 kb
    {
        printf("Size is 0.\n");
        return ;
    }
    //Construct a outFile name
//    k = k + 1;
//    itoa(str, k, 10);
//    strcat(str, ".jpg");
    //Open the new outFile
    while (1)
    {
        outFile = SYS_FS_FileOpen("photo.jpg", (SYS_FS_FILE_OPEN_WRITE));
        if (SYS_FS_HANDLE_INVALID == outFile)
        {
            int err = SYS_FS_Error();
            SYS_Tasks();
        }
        else
        {
            break;
        }
    }
    i = 0;
    ArduCAM_CS_LOW();
    ArduCAM_set_fifo_burst();
    //Read JPEG data from FIFO
    while (length --)
    {
        temp_last = temp;
        uint8_t cmd = 0x00;
        SPI3_WriteRead(&cmd, 1, &temp, 1);
        //Read JPEG data from FIFO
        if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
        {
            buf[i] = temp;  //save the last  0XD9     
            i ++;
            //Write the remain bytes in the buffer
            ArduCAM_CS_HIGH();
            SYS_FS_FileWrite(outFile, buf, i);
            //Close the file
            SYS_FS_FileClose(outFile);
            printf("Image Save Done!\n");
            is_header = false;
            i = 0;
        }  
        if (is_header == true)
        { 
            //Write image data to buffer if not full
            if (i < 256)
            {
                buf[i] = temp;
                i ++;
            }
            else
            {
                //Write 256 bytes image data to file
                ArduCAM_CS_HIGH();
                SYS_FS_FileWrite(outFile, buf, 256);
                i = 0;
                buf[i] = temp;
                i ++;
                ArduCAM_CS_LOW();
                ArduCAM_set_fifo_burst();
            }        
        }
        else if ((temp == 0xD8) & (temp_last == 0xFF))
        {
            is_header = true;
            buf[i] = temp_last;
            i ++;
            buf[i] = temp;   
            i ++;
        } 
    }
    printf ("Photo taken \n");
    capture_done = true;
}

// *****************************************************************************
// Section: Init Functions Source code
// *****************************************************************************

/*
 Camera Initialization
*/

void CAM_Init (void)
{
    uint8_t vid = 0, pid = 0;
    uint8_t temp = 0;
    
    ArduCAM_write_reg(0x07, 0x80);
    delay(100);
    ArduCAM_write_reg(0x07, 0x00);
    delay(100); 
    
    printf("ArduCAM Start!\n");

    while(1)
    {
        //Check if the ArduCAM SPI bus is OK
        ArduCAM_write_reg(ARDUCHIP_TEST1, 0x55);
        temp = ArduCAM_read_reg(ARDUCHIP_TEST1);
        if(temp != 0x55)
        {
            printf("SPI interface Error!\n");
            delay(1000);continue;
        }
        else
        {
            printf("SPI interface OK.\n");break;
        }
    }
    
    while(1)
    {
        //Check if the camera module type is OV5642
        ArduCAM_wrSensorReg16_8(0xff, 0x01);
        ArduCAM_rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
        ArduCAM_rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
        if((vid != 0x56) || (pid != 0x42))
        {
            printf("Can't find OV5642 module!\n");
            delay(1000);
            continue;
        } 
        else
        {
            printf("OV5642 detected.\n"); 
            break;
        }
    }
    ArduCAM_set_format(JPEG);
    ArduCAM_InitCAM();
    ArduCAM_write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
    OV5642_set_JPEG_size(OV5642_1280x960);
    delay(1000);
}

/*
 File System Initialization
*/

uint8_t FS_Init (void)
{
    static uint8_t state = 0;
    switch (state)
    {
        case 0 :
            // Drive mounting
            if (SYS_FS_RES_SUCCESS == SYS_FS_Mount("/dev/mmcblka1", "/mnt/myDrive", FAT, 0, NULL)) 
            {
                printf ("SD Card mounted ! \n");
                state ++;
            }
            else 
            {
                printf ("Error while mounting the SD Card \n");
                return 0;
            }
            break;
        case 1:
            // Current drive selection
            if (SYS_FS_RES_SUCCESS == SYS_FS_CurrentDriveSet("mnt/myDrive"))
            {
                printf ("Drive Selected : SD Card\n");
                return 1;
            }
            else 
            {
                printf ("Error selecting the SD Card as current drive\n");
                return 0;
            }
            break;

        case 2:
            // Opening the directory
            filehandle = SYS_FS_DirOpen("Test");
            if (SYS_FS_HANDLE_INVALID != filehandle)
            {
                printf ("Directory Opened \n");
                fs_ready = true;
                return 1;
            }
            else 
            {
                printf ("Error opening the directory\n");
                return 0;
            }
    }
    return 0;
}


/*******************************************************************************
 End of File
*/

