#ifndef __DECEL491010__
#define __DECEL491010__

#include "mbed.h"
#include "DecelComponent.hpp"
#include "DecelSerialCom.hpp"
#include "MCP41010.hpp"


class DecelMCP41010: public DecelComponent {
    private:
        MCP41010 _MCP41010;

    public:
        DecelMCP41010(MCP41010 &mcp41010, DecelSerialCom & decelSerialCom):DecelComponent(decelSerialCom),_MCP41010(mcp41010){};
        void handle_decel_msg(const DECEL_DECEL_ASK message);


};

inline void DecelMCP41010::handle_decel_msg(const DECEL_DECEL_ASK message){
      if (message.which_msg == DECEL_DECEL_ASK_mcp41010_ask_tag) {
          #ifdef _DEBUG
            printf("enter mcp41010 %d\n", message.msg.mcp41010_ask.MCP41010_id);
          #endif
            switch(message.msg.mcp41010_ask.type) {
                case(DECEL_MCP41010_message_type_SET_VALUE_MCP41010):
                    #ifdef _DEBUG
                        printf("set mcp value : %d", message.msg.mcp41010_ask.value);
                    #endif
                    _MCP41010.write_u8(message.msg.mcp41010_ask.value);
                    break;
                case(DECEL_MCP41010_message_type_SHUTDOWN_MCP41010):
                    _MCP41010.shutdown();
                    break;
            }
            decelSerialCom.send_ack(message.message_id);

      }
      
}


#endif