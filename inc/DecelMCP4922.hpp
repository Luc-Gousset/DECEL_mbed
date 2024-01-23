#ifndef __DECEL4922__
#define __DECEL4922__

#include "DecelComponent.hpp"
#include "DecelSerialCom.hpp"
#include "MCP4922.h"
#include "mbed.h"

template <std::size_t T> class DecelMCP4922 : public DecelComponent {
private:
  std::array<MCP4922, T> mcpArray;

public:
  DecelMCP4922(std::array<MCP4922, T> mcpArray, DecelSerialCom &decelSerialCom)
      : DecelComponent(decelSerialCom), mcpArray{mcpArray} {
    for (int i = 0; i < T; i++) {
      mcpArray[i].referenceMode(MCP4922::DAC_A, MCP4922::REF_UNBUFFERED);
      mcpArray[i].gainMode(MCP4922::DAC_A, MCP4922::GAIN_1X);
      mcpArray[i].powerMode(MCP4922::DAC_A, MCP4922::POWER_NORMAL);

      mcpArray[i].referenceMode(MCP4922::DAC_B, MCP4922::REF_UNBUFFERED);
      mcpArray[i].gainMode(MCP4922::DAC_B, MCP4922::GAIN_1X);
      mcpArray[i].powerMode(MCP4922::DAC_B, MCP4922::POWER_NORMAL);
    }
  };
  void handle_decel_msg(const DECEL_DECEL_ASK message);
};

template <std::size_t T>
void DecelMCP4922<T>::handle_decel_msg(const DECEL_DECEL_ASK message) {
  if (message.which_msg == DECEL_DECEL_ASK_mcp4922_ask_tag) {
#ifdef _DEBUG
    printf("enter mcp %d\n", message.msg.mcp4922_ask.MCP4922_id);
#endif
    int mcp_id = message.msg.mcp4922_ask.MCP4922_id;
    MCP4922::DAC_MCP selected_dac = message.msg.mcp4922_ask.DAC_Chanel == 0
                                        ? MCP4922::DAC_A
                                        : MCP4922::DAC_B;
    switch (message.msg.mcp4922_ask.type) {
    case (DECEL_MCP4922_message_type_SET_VALUE_MCP4922):
#ifdef _DEBUG
      printf("set mcp value : %d", message.msg.mcp4922_ask.value);
#endif
      // printf("set mcp value : %d %d %d", message.msg.mcp4922_ask.value,
      // mcp_id , message.msg.mcp4922_ask.DAC_Chanel);

      if (mcpArray[mcp_id].powerMode(selected_dac) == MCP4922::POWER_SHUTDOWN) {
        mcpArray[mcp_id].powerMode(selected_dac, MCP4922::POWER_NORMAL);
      }
      mcpArray[mcp_id].write_u16(selected_dac, message.msg.mcp4922_ask.value);
      break;
    case (DECEL_MCP4922_message_type_SHUTDOWN_MCP4922):
      mcpArray[mcp_id].powerMode(selected_dac, MCP4922::POWER_SHUTDOWN);
      break;
    }
    decelSerialCom.send_ack(message.message_id);
  }
}

#endif