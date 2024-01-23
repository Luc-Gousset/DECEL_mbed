/**
 * \file 
 * \brief Defines interface for MCP41010, a digital potentiometer.
 * 
 * \author Luc Gousset and Tristan Bienvenu
 */

#ifndef __MPC41010__
#define __MPC41010__

#include "mbed.h"

/**
 * \class MCP41010
 * \brief Provides an interface to control MCP41010 devices.
 * 
 * This class acts as a driver for the MCP41010 digital potentiometer, providing methods 
 * for writing values and shutting down the device.
 */
class MCP41010 {
    private:
        SPI & spi;        ///< Reference to an SPI object.
        DigitalOut CS;    ///< Digital output for the Chip Select (CS) pin.

    public:
        /**
         * \brief Constructs a new MCP41010 object.
         * 
         * \param spi A reference to an SPI object.
         * \param cs_pin The pin name for the Chip Select (CS) pin.
         */
        MCP41010(SPI & spi, PinName cs_pin):spi{spi},CS(cs_pin,1){};

        /**
         * \brief Writes a 8-bit value to the MCP41010 device.
         * 
         * \param value The 8-bit value to be written.
         */
        void write_u8(uint8_t value) {
            CS = 0;

            uint16_t command_byte = 0b00010001;
            spi.write(value | command_byte<<8);

            CS = 1;
        }

        /**
         * \brief Shuts down the MCP41010 device.
         */
        void shutdown() {
            CS = 0;

            spi.write(00100000<<8);
            CS = 1;
        }
};

#endif