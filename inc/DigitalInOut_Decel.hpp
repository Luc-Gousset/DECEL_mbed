/**
 * \file 
 * \brief Defines interface for a DigitalInOut class within the Decel project.
 * 
 * \author Luc Gousset and Tristan Bienvenu
 */

#ifndef __DigitalInOut_Decel__
#define __DigitalInOut_Decel__

#include "mbed.h"
#include "const.hpp"

/**
 * \class DigitalInOut_Decel
 * \brief Wrapper around the mbed's DigitalInOut class, providing additional functionality.
 * 
 * This class acts as a wrapper for mbed's DigitalInOut, allowing direction control and 
 * reading/writing values to digital pins.
 */
class DigitalInOut_Decel{
    private:
        DigitalInOut baseInOut; ///< The underlying mbed DigitalInOut object.

        PinDirection direction; ///< Current direction of the pin (input or output).

        int id;                 ///< Identifier for the object.

        bool disable = false;

    public:
        /**
         * \brief Constructs a new DigitalInOut_Decel object.
         * 
         * \param pin The name of the pin to control.
         * \param id The identifier for this object.
         * \param _direction The initial direction for the pin.
         * \param mode The initial mode for the pin.
         */
        DigitalInOut_Decel(PinName pin, int id, PinDirection _direction, PinMode mode)
            :baseInOut{DigitalInOut(pin, _direction, mode, 0)},id{id},direction{_direction}{}

        virtual ~DigitalInOut_Decel() = default;

        /** \brief Sets the pin direction to input. */
        void setInput() {baseInOut.input(); direction=PIN_INPUT; disable = false;}

        /** \brief Sets the pin direction to output. */
        void setOutput() {baseInOut.output(); direction=PIN_OUTPUT; disable = false;}

        void setDisable() {baseInOut.input(); direction=PIN_INPUT; disable = true;}

        /** \brief Returns the current pin direction. */
        PinDirection_DECEL getDirection() 
        {
            if (disable == true)
                return PinDirection_DECEL::DECEL_PIN_DISABLE;
            else if (direction == PIN_INPUT)
                return PinDirection_DECEL::DECEL_PIN_INPUT;
            else 
                return PinDirection_DECEL::DECEL_PIN_OUTPUT;
        }


        /**
         * \brief Returns the current value on the pin.
         * 
         * Reads the value on the pin if it's set as an input; otherwise, returns -1.
         */
        int getValue() {if (direction == PIN_INPUT) return baseInOut.read(); else return -1;}

        /**
         * \brief Writes a value to the pin.
         * 
         * \param val The value to be written.
         */
        void setValue(int val) {baseInOut.write(val);}

        /**
         * \brief Sets the mode of the pin.
         * 
         * \param mode The mode to be set.
         */
        void setMode(PinMode mode) {baseInOut.mode(mode);}
};

#endif
