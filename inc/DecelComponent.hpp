/**
 * \file 
 * \brief Defines a base class for components within the Decel project.
 * 
 * \author Luc Gousset and Tristan Bienvenu
 */

#ifndef __DECEL_COMPONENT__
#define __DECEL_COMPONENT__

#include "DecelSerialCom.hpp"
#include "decel.pb.h"
#include "mbed.h"
#include "pb_decode.h"
#include "pb_encode.h"

/**
 * \class DecelComponent
 * \brief Base class for components that can handle messages in the Decel project.
 * 
 * This class provides an interface for handling ProtoBuf messages. Derived classes 
 * must implement the handle_decel_msg method to define how they respond to these messages.
 */
class DecelComponent {
protected:
    DecelSerialCom & decelSerialCom; ///< Reference to a DecelSerialCom object for handling serial communication.

public:
  /**
   * \brief Constructs a new DecelComponent object.
   * 
   * \param decelSerialCom A reference to a DecelSerialCom object for handling serial communication.
   */
  DecelComponent(DecelSerialCom & decelSerialCom):decelSerialCom{decelSerialCom}{};

  /**
   * \brief Handles ProtoBuf messages.
   * 
   * This is a pure virtual method, meaning it must be implemented by any class 
   * that derives from DecelComponent. The implementation should define how the 
   * component responds to the message.
   * 
   * \param message The ProtoBuf message to be handled.
   */
  virtual void handle_decel_msg(const DECEL_DECEL_ASK message) = 0;
};

#endif
