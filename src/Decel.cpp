#include "Decel.hpp"
#include "decel.pb.h"

void Decel::_sbc_serial_reader() {
  printf("enter decel thread \n");

  MbedCRC<POLY_8BIT_CCITT, 8> ct;

  constexpr size_t MAX_BUFFER_SIZE = (DECEL_DECEL_ASK_size + 5) * 6 ;
  std::array<uint8_t, MAX_BUFFER_SIZE> buffer_;
  size_t buffer_length = 0;
  printf("atfer \n");

  DECEL_DECEL_ASK message;
  pb_istream_t input_stream;
  uint32_t crc;
  std::array<uint8_t, DECEL_DECEL_ASK_size + 5> temp_buffer;

  while (true) {
    int n = sbc_serial.read(temp_buffer.data(), temp_buffer.size());

    // Append to main buffer if data is received
    if (n > 0) {
      if (buffer_length + n <= MAX_BUFFER_SIZE) {
        std::copy(temp_buffer.begin(), temp_buffer.begin() + n,
                  buffer_.begin() + buffer_length);
        buffer_length += n;
      } else {
        printf("Buffer overflow, reset buffer\n");
        // Buffer overflow, reset buffer
        buffer_.fill(0);
        buffer_length = 0;
        continue;
      }

      // Process buffer
      size_t start_idx;

      while ((start_idx = std::find(buffer_.begin(),
                                    buffer_.begin() + buffer_length, SOM) -
                          buffer_.begin()) < buffer_length) {

        if (buffer_length < start_idx + 3) {
          printf("// Not enough data for length field\n");
          break; // Not enough data for length field
        }

        size_t msg_length = buffer_.at(start_idx + 1);
        size_t end_idx = start_idx + 1 + msg_length + 2;

        if (end_idx >= buffer_length || buffer_.at(end_idx) != EOM) {
          break; // Incomplete message
        }
        crc_mutex.lock();
        ct.compute(&buffer_[start_idx + 2], msg_length, &crc);
        crc_mutex.unlock();

        // Extract message
        std::array<uint8_t, DECEL_DECEL_ASK_size> msg_buffer;
        std::copy(buffer_.begin() + start_idx + 2,
                  buffer_.begin() + end_idx - 1, msg_buffer.begin());

        if (crc != buffer_.at(end_idx-1)) {
          printf("WRONG CRC %d %d \n", crc, buffer_[end_idx - 1]);

        } else {

          // Process Protocol Buffers message
          input_stream = pb_istream_from_buffer(msg_buffer.data(), msg_length);
          pb_decode(&input_stream, DECEL_DECEL_ASK_fields, &message);

          // Handle message based on type
          switch (message.which_msg) {
          case (DECEL_DECEL_ASK_gpio_ask_tag):
            //printf("GPIO message received\n");
            decel_incoming_queue.call(
                &_decelGPIO, &DecelGPIO<GPIO_SIZE>::handle_decel_msg, message);
            break;
          case (DECEL_DECEL_ASK_mcp4922_ask_tag):
            //printf("MCP4922 message received\n");
            decel_incoming_queue.call(
                &_decelMCP4922, &DecelMCP4922<2>::handle_decel_msg, message);
            break;
          case (DECEL_DECEL_ASK_mcp41010_ask_tag):
           // printf("MCP41010 message received\n");
            decel_incoming_queue.call(
                &_decelMCP41010, &DecelMCP41010::handle_decel_msg, message);
            break;
        case (DECEL_DECEL_ASK_analog_in_ask_tag):
            //printf("Analog IN message received\n");
            decel_incoming_queue.call(
                &_decelAnalogIn, &DecelAnalogIn<ANALOG_IN_SIZE>::handle_decel_msg, message);
            break;

          }
          
        }
        // Remove processed message from buffer
        std::move(buffer_.begin() + end_idx + 1,
                  buffer_.begin() + buffer_length, buffer_.begin() + start_idx);
        buffer_length -= (end_idx + 1 - start_idx);
      }
    }
  }
}
