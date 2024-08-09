#ifndef SpineMotor_HPP
#define SpineMotor_HPP

#include "modbus_client.hpp"

typedef struct _SpineMotorCommand {
  double position_command = 0;
  double velocity_command = 0.1;
  double acceleration_time = 0.1;  //s
  double deceleration_time = 0.1;  //s
} SpineMotorCommand;

typedef struct _SpineMotorState {
  double position_state = 0;
  double velocity_state = 0;
  double error_id = 0;
} SpineMotorState;

class SpineMotor {
public:
  SpineMotor(ModbusClient &modbus);
  void update();
  void set_command(SpineMotorCommand command);
  SpineMotorState get_state();

private:
  enum ErrorState {
    NO_ERROR = 0,
    OVER_UPPER_POS_LIMIT = 3,
    AT_ORIGIN_POINT = 5,
    OVER_LOWER_POS_LIMIT = 9,
    BELT_SLIPING = 7
  };
  ModbusClient modbus_;
  SpineMotorCommand command_;
  SpineMotorState state_;
  int get_pos_state();
  int get_vel_state();
  int get_error_id();
};

#endif // SpineMotor_HPP
