#include "mbed.h"

#include "Decel.hpp"


const std::array<DigitalInOut_Decel, 16> default_array2 = {
    DigitalInOut_Decel(PC_9, 0, PIN_OUTPUT, PushPullNoPull),
    DigitalInOut_Decel(PB_8, 1, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PB_9, 2, PIN_OUTPUT, PullNone),
    DigitalInOut_Decel(PA_6, 3, PIN_OUTPUT, PullNone),
    DigitalInOut_Decel(PA_7, 4, PIN_OUTPUT, PullNone),
    DigitalInOut_Decel(PB_6, 5, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PC_7, 6, PIN_OUTPUT, PullNone),
    DigitalInOut_Decel(PA_9, 7, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PA_8, 8, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PB_10, 9, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PB_4, 10, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PB_5, 11, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PA_10, 12, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PC_8, 13, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PC_6, 14, PIN_INPUT, PullNone),
    DigitalInOut_Decel(PC_5, 15, PIN_INPUT, PullNone)};

const std::array<DigitalInOut_Decel, 2> default_array = {
    DigitalInOut_Decel(PC_9, 0, PIN_OUTPUT, PushPullNoPull),
    DigitalInOut_Decel(PB_8, 1, PIN_INPUT, PullNone)};
const std::array<AnalogIn, 4> default_analogin_array = {
    AnalogIn(PB_0),
    AnalogIn(PA_4),
    AnalogIn(PA_1),
    AnalogIn(PA_0),
    };

  Mutex crc_mutex;

  BufferedSerial sbc_serial{PC_10, PC_11};
  SPI decelSPI{PB_15, PB_14, PB_13};
  MCP41010 mcp41010{decelSPI, PC_4};
  // Composants
  DecelSerialCom serial_com{sbc_serial, crc_mutex};
  std::array<MCP4922, 2> mcp_ARRAY = {MCP4922{decelSPI, PB_1},
                                      MCP4922{decelSPI, PA_12}};

  DecelGPIO<16> _decelGPIO{default_array2, serial_com};

  DecelMCP4922<2> _decelMCP4922{mcp_ARRAY, serial_com};

  DecelMCP41010 _decelMCP41010{mcp41010, serial_com};

  DecelAnalogIn<4> _decelAnalogIn{default_analogin_array, serial_com};

int main()
{
    printf("ENTER DECEL LAND\n");
    Decel mainApp(crc_mutex,sbc_serial,decelSPI,mcp41010,serial_com,_decelGPIO,_decelMCP4922,_decelMCP41010,_decelAnalogIn );

    while(true);
}