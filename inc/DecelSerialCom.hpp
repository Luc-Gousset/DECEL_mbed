/**
 * \file 
 * \brief Defines interface for the serial communication within the Decel project.
 * 
 * \author Luc Gousset and Tristan Bienvenu
 */

#ifndef __DECEL_SERIAL_COM__
#define __DECEL_SERIAL_COM__

#include "mbed.h"
#include <cstdint>
#include "decel.pb.h"
#include <array>
#include "pb_decode.h"
#include "pb_encode.h"
#include <const.hpp>

/**
 * \class DecelSerialCom
 * \brief Provides an interface to control serial communication in the Decel project.
 * 
 * This class acts as a wrapper for mbed's BufferedSerial, providing a method to send 
 * data through a serial interface with thread safety (using a Mutex).
 */
class DecelSerialCom {
private:
  BufferedSerial &sbc_serial; ///< The underlying mbed BufferedSerial object.
  Mutex &crc_mutex;        ///< Mutex for thread safety.

  MbedCRC<POLY_8BIT_CCITT, 8> ct;

public:
  Mutex serial_mutex;        ///< Mutex for thread safety.

  /**
   * \brief Constructs a new DecelSerialCom object.
   * 
   * \param sbc_serial A reference to an mbed BufferedSerial object.
   * \param serial_mutex A reference to a Mutex for thread safety.
   */
  DecelSerialCom(BufferedSerial &sbc_serial, Mutex &crc_mutex)
    :sbc_serial{sbc_serial},crc_mutex{crc_mutex} {};

  /**
   * \brief Sends a buffer of data through the serial interface.
   * 
   * The method first sends a delimiter and the size of the buffer, followed by the data.
   * The send operation is protected by a mutex for thread safety.
   * 
   * \param data_to_send The array of data to be sent.
   * \param size The size of the buffer.
   */
  void send_to_serial(const std::array<uint8_t, DECEL_DECEL_ANSW_size> data_to_send , std::size_t size);


  void send_ack(const uint32_t message_id);


};

#endif