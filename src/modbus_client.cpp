#include "modbus_client.hpp"
#include <cerrno>
#include <iostream>

ModbusClient::ModbusClient(const std::string port, int baudrate, int slave) {
  client = modbus_new_rtu(port.c_str(), baudrate, 'N', 8, 1);
  // modbus_rtu_set_serial_mode(client, MODBUS_RTU_RS485);
  modbus_set_slave(client, slave);
  modbus_connect(client);
}

ModbusClient::~ModbusClient() {
  modbus_close(client);
  modbus_free(client);
}

int ModbusClient::read_register(int address, int &value) {
  uint16_t reg[1];
  int rc = modbus_read_registers(client, address, 1, reg);
  if (rc == -1) {
    std::cerr << "Error reading holding registers: " << modbus_strerror(errno)
              << std::endl;
    return 1;
  }
  value = reg[0];
  return 0;
}

int ModbusClient::write_register(int address, int value) {
  int rc = modbus_write_register(client, address, value);
  if (rc == -1) {
    std::cout << "write_register: " << address << " = " << value <<
    std::endl;
    std::cerr << "write_register failed: " << modbus_strerror(errno)
              << std::endl;
    return 1;
  }
  return 0;
}
