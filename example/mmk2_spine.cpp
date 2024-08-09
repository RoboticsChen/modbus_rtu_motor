#include "spine_motor.hpp"
#include <chrono>
#include <iostream>
#include <ncurses.h>
#include <thread>

auto awake_time(double rate) {
  using namespace std::chrono;
  auto now = steady_clock::now();
  auto period = duration<double>(1.0 / rate);
  return now + period;
}

int main(int argc, char *argv[]) {
  ModbusClient modbus("/dev/ttyUSB0", 38400, 1);
  SpineMotor spine(modbus);
  SpineMotorCommand command;
  SpineMotorState state;
  while (spine.get_state().position_state > 10) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // 初始化 ncurses
  filter();
  initscr();
  timeout(0); // 设置非阻塞输入
  noecho();   // 不显示输入字符
  cbreak();   // 立即读取字符而不是等待换行
  char ch;
  bool flag = false;
  while (!flag) {
    const auto awake = awake_time(100);

    int ch = getch();
    switch (ch) {
    case 'w':
      state = spine.get_state();
      command.position_command = state.position_state - 0.05;
      spine.set_command(command);
      break;
    case 's':
      state = spine.get_state();
      command.position_command = state.position_state + 0.05;
      spine.set_command(command);
      break;
    case 'q':
      flag = true;
      break;
    default:
      continue;
      break;
    }

    spine.update();
    std::this_thread::sleep_until(awake);
  }

  // 结束 ncurses 模式
  endwin();
  return 0;
}