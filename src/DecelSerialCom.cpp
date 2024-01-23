#include "DecelSerialCom.hpp"

  void DecelSerialCom::send_to_serial(const std::array<uint8_t, DECEL_DECEL_ANSW_size> data_to_send , std::size_t size) {
      printf("send\n");
      serial_mutex.lock();
    uint32_t  crc;

    crc_mutex.lock();
    ct.compute(data_to_send.data(), size, &crc);
    crc_mutex.unlock();

    sbc_serial.write(&SOM, 1);
    sbc_serial.write(&size, 1);
    sbc_serial.write(data_to_send.data(), size);
    sbc_serial.write(&crc, 1);
    sbc_serial.write(&EOM, 1);
        serial_mutex.unlock();

  }

  void DecelSerialCom::send_ack(const uint32_t message_id)
  {
        //printf("SEND ACK %d ", message_id);
        DECEL_DECEL_ANSW decel_asnw;
        DECEL_ACK decel_ack;
        decel_ack.message_id = message_id;
        decel_ack.status = DECEL_MESSAGE_ANSW_ERROR_OK;
        decel_asnw.message_id=message_id;
        decel_asnw.which_msg=DECEL_DECEL_ANSW_ack_tag;
        decel_asnw.msg.ack = decel_ack;

        std::array<uint8_t, DECEL_DECEL_ANSW_size> buffer_output = {0};
        pb_ostream_t ouput_stream = pb_ostream_from_buffer(buffer_output.data(), buffer_output.size());

        pb_encode(&ouput_stream, DECEL_DECEL_ANSW_fields, &decel_asnw);
        send_to_serial(buffer_output, ouput_stream.bytes_written);
  }
