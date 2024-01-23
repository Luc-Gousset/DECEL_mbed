#ifndef __DECEL__
#define __DECEL__

#include "mbed.h"

#include "DecelAnalogIN.hpp"
#include "DecelGPIO.hpp"
#include "DecelMCP41010.hpp"
#include "DecelMCP4922.hpp"
#include "DecelSerialCom.hpp"
#include "MCP41010.hpp"
#include "MCP4922.h"
#include "const.hpp"
#include "decel.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include <cmath>
#include <cstdint>

class Decel {
private:
  Mutex &crc_mutex;
  // Declaration bus
  BufferedSerial &sbc_serial;
  SPI &decelSPI;
  MCP41010 &mcp41010;
  // Composants
  DecelSerialCom &serial_com;

  DecelGPIO<16> &_decelGPIO;

  DecelMCP4922<2> &_decelMCP4922;

  DecelMCP41010 &_decelMCP41010;

  DecelAnalogIn<4> &_decelAnalogIn;

  void _sbc_serial_reader();

  Thread sbc_serial_thread;

  Thread CycleThread;

  // EventQueue cycle_components_queue;

  EventQueue decel_incoming_queue;

public:
  Decel(Mutex &crc_mutex, BufferedSerial &sbc_serial, SPI &decelSPI,
        MCP41010 &mcp41010, DecelSerialCom &serial_com,

        DecelGPIO<16> &_decelGPIO,

        DecelMCP4922<2> &_decelMCP4922,

        DecelMCP41010 &_decelMCP41010,

        DecelAnalogIn<4> &_decelAnalogIn)
      : sbc_serial(sbc_serial), crc_mutex(crc_mutex), decelSPI(decelSPI),
        mcp41010(mcp41010), serial_com(serial_com), _decelGPIO(_decelGPIO),
        _decelMCP4922(_decelMCP4922), _decelMCP41010(_decelMCP41010),
        _decelAnalogIn(_decelAnalogIn) {

    // init serial
    sbc_serial.set_baud(DEFAULT_BAUD);
    sbc_serial_thread.start(
        callback(&decel_incoming_queue, &EventQueue::dispatch_forever));

    decel_incoming_queue.call_every(GPIO_POOLING_MS, &_decelGPIO,
                                    &DecelGPIO<16>::cycle);
    decel_incoming_queue.call_every(ANALOG_IN_POOLING_MS, &_decelAnalogIn,
     &DecelAnalogIn<4>::cycle);

    printf("   a\n");
    DigitalOut latch(PB_2);
    DigitalOut shutdown(PC_1);
    shutdown = 1;
    DigitalOut shutdown2(PC_0);
    shutdown2 = 1;
    printf("   b\n");

    latch = 0;
    // init spi
    decelSPI.format(16, 0);
    decelSPI.frequency(10000000);

    _sbc_serial_reader();
  };
};

#endif