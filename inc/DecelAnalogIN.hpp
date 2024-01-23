/**
 * \file
 * \brief Defines the DecelGPIO class, a component of the Decel project that
 * handles GPIO operations.
 *
 * \author Luc Gousset and Tristan Bienvenu
 */

#ifndef __DECEL_ANALOG_IN__
#define __DECEL_ANALOG_IN__

#include "DecelComponent.hpp"
#include "DecelSerialCom.hpp"
#include "decel.pb.h"
#include "mbed.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>

/**
 * \class DecelGPIO
 * \brief Handles GPIO operations for the Decel project.
 *
 * This class inherits from DecelComponent, and provides functionality for
 * controlling a set of GPIO pins. It maintains an array of DigitalInOut_Decel
 * objects, each representing a GPIO pin, and provides methods to set pin modes,
 * read/write pin values, and handle ProtoBuf messages related to GPIO
 * operations.
 */
template <std::size_t T> class DecelAnalogIn : public DecelComponent {
private:
  std::array<AnalogIn, T> analog_array; ///< Array of GPIO pins.
  std::array<bool, T> analog_array_enable; ///< Array of GPIO pins.
  std::array<uint16_t, T> last_value = {NULL};
std::array<uint16_t, T> getValues();
public:
  /**
   * \brief Constructs a new DecelGPIO object.
   *
   * \param a An array of DigitalInOut_Decel objects, each representing a GPIO
   * pin. \param decelSerialCom A reference to a DecelSerialCom object for
   * handling serial communication.
   */
  DecelAnalogIn(std::array<AnalogIn, T> analog_array, DecelSerialCom &decelSerialCom)
      : DecelComponent(decelSerialCom), analog_array{analog_array} {};

  /**
   * \brief Handles ProtoBuf messages related to GPIO operations.
   *
   * The method reads the message, interprets the requested operation, and calls
   * the appropriate method to perform it. It then sends a response message with
   * the result of the operation.
   *
   * \param message The ProtoBuf message to be handled.
   */
  void handle_decel_msg(const DECEL_DECEL_ASK message);

  void cycle();
};

// Implementation of DecelGPIO methods goes here...

/**
 * \brief Handles a GPIO-related ProtoBuf message.
 *
 * This method examines the provided ProtoBuf message, determines the type of
 * GPIO operation it represents, and performs the appropriate operation by
 * calling the corresponding method. It also constructs and sends a ProtoBuf
 * response message with the result of the operation.
 *
 * \param message The ProtoBuf message to handle.
 *
 * \note This function is thread-safe.
 */
template <std::size_t T>
void DecelAnalogIn<T>::handle_decel_msg(const DECEL_DECEL_ASK message) {
  // check if the incoming message is GPIO-related
  if (message.which_msg == DECEL_DECEL_ANSW_analog_in_data_tag) {
    int id = message.msg.analog_in_ask.ANALOG_IN_id;
    switch (message.msg.analog_in_ask.type) {
    case (DECEL_ANALOG_IN_message_type_DISABLE_ANALOG_IN):
        analog_array_enable.at(id)=false;
      break;
    case (DECEL_ANALOG_IN_message_type_ENABLE_ANALOG_IN):
        analog_array_enable.at(id)=true;
      break;
    }
    decelSerialCom.send_ack(message.message_id);
  }
}

template <std::size_t T> std::array<uint16_t, T> DecelAnalogIn<T>::getValues() {

  std::array<uint16_t, T> result = {0};

  for (uint8_t i = 0; i < T; i++) {
    if(analog_array_enable.at(i)==true)
        result.at(i) = analog_array.at(i).read_u16();
    else 
        result.at(i) = 0;

  }

  return result;
}
template <std::size_t T> void DecelAnalogIn<T>::cycle() {
    printf("AnalogIn cycle\n");
  std::array<uint16_t, T> current_value = getValues();

  std::array<uint8_t, DECEL_DECEL_ANSW_size> buffer_output = {0};

  if (!std::equal(std::begin(last_value), std::end(last_value),
                  std::begin(current_value))) {
    pb_ostream_t ouput_stream;

    DECEL_DECEL_ANSW answ;
    answ.which_msg = DECEL_DECEL_ANSW_analog_in_data_tag;

    for (int i = 0; i < T; i++) {
      if (current_value.at(i) != -1 &&
          current_value.at(i) != last_value.at(i)) {
        answ.msg.analog_in_data.ANALOG_IN_id = i;
        answ.msg.analog_in_data.value = current_value.at(i);

        answ.message_id = 0;
        ouput_stream =
            pb_ostream_from_buffer(buffer_output.data(), buffer_output.size());
        pb_encode(&ouput_stream, DECEL_DECEL_ANSW_fields, &answ);
        decelSerialCom.send_to_serial(buffer_output,
                                      ouput_stream.bytes_written);

        buffer_output.fill(0);
      }
    }

    std::copy(std::begin(current_value), std::end(current_value), std::begin(last_value));
  }
    
}

#endif