#include "spine_motor.hpp"
#include <iostream>

// 定义寄存器地址
#define REG_JOG_MOVE 0x1801
#define REG_MOVE_MODE 0x6200
#define REG_MOVE_SPEED 0x6203
#define REG_POS_FB_HIGH_BITS 0x602C
#define REG_POS_FB_LOW_BITS 0x602D
#define REG_VEL_FB_HIGH_BITS 0x1046
#define REG_VEL_FB_LOW_BITS 0x1047
#define REG_POS_HIGH_BITS 0x6201
#define REG_POS_LOW_BITS 0x6202
#define REG_ACCELERAT_TIME 0x6204
#define REG_DECELERAT_TIME 0x6205
#define REG_TRIGGER_FUNCTION 0x6002
#define REG_ERROR_STATE 0x2203
#define REG_LIMIT_STATE 0x0179
#define REG_OPERATE_ERROR 0x1801

// 定义命令选项
#define CMD_JOG_UP 0x4001
#define CMD_JOG_DOWN 0x4002
#define CMD_POSITION_MODE_ABSOLUTE 0x0001
#define CMD_POSITION_MODE_RELATIVE 0x0041
#define CMD_SPEED_MODE 0x0002
#define CMD_TRIGGER_FUNCTION_RUN 0x0010
#define CMD_TRIGGER_FUNCTION_EMERGENCY_STOP 0x0040
#define CMD_TRIGGER_FUNCTION_BACK_TO_ZERO 0x020
#define CMD_TRIGGER_FUNCTION_MANUAL_SET_ZERO 0x021
#define CMD_CLEAR_ERROR 0x1111

#define reduction_ratio 3
#define pos_rate (1000.0 / (52.5/ reduction_ratio) * 10000.0 * -1.0)
#define vel_rate (1000.0 / (52.5/ reduction_ratio) * 60.0)

double apply_limit(double value, const double min, const double max) {
  if (value < min) {
    value = min;
  } else if (value > max) {
    value = max;
  }
  return value;
}

SpineMotor::SpineMotor(ModbusClient &modbus) : modbus_(modbus) {
  modbus.write_register(REG_TRIGGER_FUNCTION,
                        CMD_TRIGGER_FUNCTION_BACK_TO_ZERO);
}

void SpineMotor::update() {
  // error check
  if (state_.error_id == BELT_SLIPING) {
    modbus_.write_register(REG_TRIGGER_FUNCTION,
                           CMD_TRIGGER_FUNCTION_BACK_TO_ZERO);
    state_.position_state = 0;
    state_.velocity_state = 0;
    state_.error_id = 0;
  }

  // std::cout << command_.position_command << std::endl;
  int pos = int(apply_limit(command_.position_command, 0, 1) * pos_rate);
  int vel = int(apply_limit(command_.velocity_command, 0, 0.25) * vel_rate);
  int acc_time = int(apply_limit(command_.acceleration_time, 0.02, 0.1) * 1000);
  int dec_time = int(apply_limit(command_.deceleration_time, 0.02, 0.1) * 1000);
  // update spine motor position
  modbus_.write_register(REG_MOVE_MODE, CMD_POSITION_MODE_ABSOLUTE);
  modbus_.write_register(REG_POS_HIGH_BITS, (pos >> 16) & 0xFFFF);
  modbus_.write_register(REG_POS_LOW_BITS, pos & 0xFFFF);
  modbus_.write_register(REG_MOVE_SPEED, vel);
  modbus_.write_register(REG_ACCELERAT_TIME, acc_time);
  modbus_.write_register(REG_DECELERAT_TIME, dec_time);
  modbus_.write_register(REG_TRIGGER_FUNCTION, CMD_TRIGGER_FUNCTION_RUN);

  // get spine motor state
  state_.position_state = get_pos_state() / pos_rate;
  state_.velocity_state = get_vel_state() / vel_rate;
  state_.error_id = get_error_id();
}

void SpineMotor::set_command(SpineMotorCommand command) { command_ = command; };
SpineMotorState SpineMotor::get_state() { return state_; };

int SpineMotor::get_pos_state() {
  int low_bits_value, high_bits_value;
  modbus_.read_register(REG_POS_FB_LOW_BITS, low_bits_value);
  modbus_.read_register(REG_POS_FB_HIGH_BITS, high_bits_value);
  if (high_bits_value > 32767) {
    low_bits_value -= 65535;
    high_bits_value -= 65535;
  }
  return high_bits_value * 65535 + low_bits_value;
}

int SpineMotor::get_vel_state() {
  int low_bits_value, high_bits_value;
  modbus_.read_register(REG_VEL_FB_LOW_BITS, low_bits_value);
  modbus_.read_register(REG_VEL_FB_HIGH_BITS, high_bits_value);
  if (high_bits_value > 32767) {
    low_bits_value -= 65535;
    high_bits_value -= 65535;
  }
  return high_bits_value * 65535 + low_bits_value;
}

int SpineMotor::get_error_id() {

  int err;

  if (!modbus_.read_register(REG_ERROR_STATE, err)) {
    return err;
  }

  if (!modbus_.read_register(REG_LIMIT_STATE, err)) {
    if (err == 5)
      return 0;
    return err;
  }

  return 0;
}