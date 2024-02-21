/**
 * \file
 * \brief Defines the DecelGPIO class, a component of the Decel project that
 * handles GPIO operations.
 *
 * \author Luc Gousset and Tristan Bienvenu
 */

#ifndef __DECEL_GPIO__
#define __DECEL_GPIO__

#include "DecelComponent.hpp"
#include "DecelSerialCom.hpp"
#include "DigitalInOut_Decel.hpp"
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
template <std::size_t T> class DecelGPIO : public DecelComponent {
private:
  Mutex gpio_array_mutex;                       ///< Mutex for thread safety.
  std::array<DigitalInOut_Decel, T> gpio_array; ///< Array of GPIO pins.
  std::array<int8_t, T> last_value = {NULL};

  void setOutput(int pin_id);
  void setInput(int pin_id);
  void setValue(int pin_id, int value);
  void setDisable(int pin_id);
  void setMode(int pin_id, PinMode mode);
  int getValue(int id){};

public:
  std::array<int8_t, T> getValues();

  /**
   * \brief Constructs a new DecelGPIO object.
   *
   * \param a An array of DigitalInOut_Decel objects, each representing a GPIO
   * pin. \param decelSerialCom A reference to a DecelSerialCom object for
   * handling serial communication.
   */
  DecelGPIO(std::array<DigitalInOut_Decel, T> a, DecelSerialCom &decelSerialCom)
      : DecelComponent(decelSerialCom), gpio_array{a} {};

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
void DecelGPIO<T>::handle_decel_msg(const DECEL_DECEL_ASK message) {
  // check if the incoming message is GPIO-related
  if (message.which_msg == DECEL_DECEL_ASK_gpio_ask_tag) {
    int id = message.msg.gpio_ask.gpio_id;
    // create response message and content
    DECEL_DECEL_ANSW answ;
    DECEL_GPIO_ANSW content;
    std::array<uint8_t, DECEL_DECEL_ANSW_size> buffer_output = {0};
    pb_ostream_t ouput_stream =
        pb_ostream_from_buffer(buffer_output.data(), buffer_output.size());

    // determine the type of GPIO operation requested in the message and perform
    // it
    switch (message.msg.gpio_ask.type_message) {
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_GET_STATE):
      // get GPIO pin direction
      answ.which_msg = DECEL_DECEL_ANSW_gpio_data_tag;
      content.gpio_id = id;
      content.has_value = true;
      content.type_message = DECEL_GPIO_MESSAGE_ANSW_TYPE_DIRECTION;
      content.value = gpio_array.at(id).getDirection();
      answ.msg.gpio_data = content;
      // send response message
      pb_encode(&ouput_stream, DECEL_DECEL_ANSW_fields, &answ);
      decelSerialCom.send_to_serial(buffer_output,
                                    ouput_stream.bytes_written);
      break;
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_SET_STATE_O):
      // set GPIO pin to output
     // printf("set as output %d\n", id);
      setOutput(id);
      break;
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_SET_STATE_I):
      // set GPIO pin to input
    //printf("set as input %d\n", id);

      setInput(id);
      break;
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_GET_READ_VALUE):
      // get GPIO pin value
      answ.which_msg = DECEL_DECEL_ANSW_gpio_data_tag;
      content.gpio_id = id;
      content.has_value = true;
      content.type_message = DECEL_GPIO_MESSAGE_ANSW_TYPE_VALUE;
      content.value = gpio_array.at(id).getValue();
      answ.msg.gpio_data = content;
      // send response message
      pb_encode(&ouput_stream, DECEL_DECEL_ANSW_fields, &answ);
      decelSerialCom.send_to_serial(buffer_output,
                                   ouput_stream.bytes_written);

      break;
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_OUTPUT_SET):
      // set GPIO pin value to 1
     // printf("set output %d\n", id);
      setValue(id, 1);
      break;
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_OUTPUT_CLEAR):
      // clear GPIO pin (set value to 0)
      setValue(id, 0);
      break;
    case (DECEL_GPIO_MESSAGE_ASK_TYPE_SET_DISABLE):
      // clear GPIO pin (set value to 0)
      setDisable(id);
      break;
    }

    //decelSerialCom.event_queue.call(decelSerialCom, &DecelSerialCom::send_ack, message.message_id);

    decelSerialCom.send_ack(message.message_id);

  }
}

template <std::size_t T> void DecelGPIO<T>::setInput(int pin_id) {
  gpio_array_mutex.lock();

  gpio_array.at(pin_id).setInput();

  gpio_array_mutex.unlock();
}

template <std::size_t T> void DecelGPIO<T>::setDisable(int pin_id) {
  gpio_array_mutex.lock();

  gpio_array.at(pin_id).setDisable();

  gpio_array_mutex.unlock();
}

template <std::size_t T> void DecelGPIO<T>::setOutput(int pin_id) {
  gpio_array_mutex.lock();

  gpio_array.at(pin_id).setOutput();

  gpio_array_mutex.unlock();
}

template <std::size_t T> void DecelGPIO<T>::setValue(int pin_id, int value) {
  if (gpio_array.at(pin_id).getDirection() == DECEL_PIN_OUTPUT) {
    gpio_array.at(pin_id).setValue(value);
  }
}

template <std::size_t T> std::array<int8_t, T> DecelGPIO<T>::getValues() {

  std::array<int8_t, T> result = {0};

  for (uint8_t i = 0; i < T; i++) {
    result.at(i) = gpio_array.at(i).getValue();
  }

  return result;
}

template <std::size_t T> void DecelGPIO<T>::cycle() {
  std::array<int8_t, T> current_value = getValues();

  std::array<uint8_t, DECEL_DECEL_ANSW_size> buffer_output = {0};

  if (!std::equal(std::begin(last_value), std::end(last_value),
                  std::begin(current_value))) {
    pb_ostream_t ouput_stream;

    DECEL_DECEL_ANSW answ;
    answ.which_msg = DECEL_DECEL_ANSW_gpio_data_tag;

    for (int i = 0; i < T; i++) {
      if (current_value.at(i) != -1 &&
          current_value.at(i) != last_value.at(i)) {
        answ.msg.gpio_data.gpio_id = i;
        answ.msg.gpio_data.type_message = DECEL_GPIO_MESSAGE_ANSW_TYPE_VALUE;

        answ.msg.gpio_data.has_value = true;
        answ.msg.gpio_data.value = current_value.at(i);

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