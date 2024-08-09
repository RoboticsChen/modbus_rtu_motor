#ifndef MODBUS_CLIENT_HPP
#define MODBUS_CLIENT_HPP

#include <modbus/modbus.h>
#include <string>

class ModbusClient {
public:
  ModbusClient(const std::string port, int baudrate, int slave);
  ~ModbusClient();
  int read_register(int address, int &value);
  int write_register(int address, int value);

private:
  modbus_t *client;
};

#endif // MODBUS_CLIENT_HPP
