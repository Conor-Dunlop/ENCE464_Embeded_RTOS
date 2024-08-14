/*
 * Acceleration Manager
 * Matt Suter, Tim Preston-Marshall, Daniel Rabbidge
 * ENCE361 2022
 *
 * FitnessThur9-1
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "acc.h"
#include "i2c_driver.h"
#include "circBufV.h"

#include "accl_manager.h"



//********************************************************
// Constants and static vars
//********************************************************
#define BUF_SIZE 20 // WARNING: If this is set too high, we run out of heap space and the z-buffer gets garbled data
static circBufVec_t* acclBuffer;



/*******************************************
 *      Local prototypes
 *******************************************/
void initAcclChip(void);
vector3_t getAcclData(void);



/*******************************************
 *      Global functions
 *******************************************/
// Init the library
void acclInit(void)
{
    initAcclChip(); // Init the chip over I2C

    acclBuffer = initVecCircBuf(BUF_SIZE);
}



// Run periodically to store acceleration to the circular buffer
void acclProcess(void)
{
    vector3_t acceleration = getAcclData();
    writeVecCircBuf(&acclBuffer, acceleration);
}



// Return the mean acceleration stored within the circular buffers
vector3_t acclMean(void)
{
    // Sum with 32-bit ints to prevent overflow, then dividing the total sum for better accuracy
    int32_t result_x = 0;
    int32_t result_y = 0;
    int32_t result_z = 0;

    uint8_t i = 0;
    for (i = 0; i < BUF_SIZE; i++) {
        vector3_t nextVector = readVecCircBuf(&acclBuffer);
        result_x = result_x + nextVector.x;
        result_y = result_y + nextVector.y;
        result_z = result_z + nextVector.z;
    }

    vector3_t result = {0};
    result.x = result_x / BUF_SIZE;
    result.y = result_y / BUF_SIZE;
    result.z = result_z / BUF_SIZE;

    return result;
}



/*******************************************
 *      Local Functions
 *******************************************/
// Init the accl chip via I2C
void initAcclChip(void)
{
    char    toAccl[] = {0, 0};  // parameter, value

    /*
    * Enable I2C Peripheral
    */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
    * Set I2C GPIO pins
    */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
    * Setup I2C
    */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    //Initialize ADXL345 Acceleromter

    // set +-16g, 13 bit resolution, active low interrupts
    toAccl[0] = ACCL_DATA_FORMAT;
    toAccl[1] = (ACCL_RANGE_16G | ACCL_FULL_RES);
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_PWR_CTL;
    toAccl[1] = ACCL_MEASURE;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_BW_RATE;
    toAccl[1] = ACCL_RATE_100HZ;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_INT;
    toAccl[1] = 0x00;       // Disable interrupts from accelerometer.
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_X;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Y;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Z;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);
}



// Read the accl chip
vector3_t getAcclData(void)
{
    char    fromAccl[] = {0, 0, 0, 0, 0, 0, 0}; // starting address, placeholders for data to be read.
    vector3_t acceleration;
    uint8_t bytesToRead = 6;

    fromAccl[0] = ACCL_DATA_X0;
    I2CGenTransmit(fromAccl, bytesToRead, READ, ACCL_ADDR);

    acceleration.x = (fromAccl[2] << 8) | fromAccl[1]; // Return 16-bit acceleration readings.
    acceleration.y = (fromAccl[4] << 8) | fromAccl[3];
    acceleration.z = (fromAccl[6] << 8) | fromAccl[5];

    return acceleration;
}


