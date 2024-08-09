import pymodbus.client as ModbusClient
from pymodbus import (
    ExceptionResponse,
    Framer,
    ModbusException,
    pymodbus_apply_logging_config,
)
import argparse
def limit_detect():
    try:
        data = client.read_holding_registers(0x0179, 1, slave=slave)
        #读取输入IO口的数据
        #读到3代表上限位触发 5代表原点信号触发 9代表下限位触发
        print(f"读取到的寄存器数据: {data.registers}")

    except ModbusException as exc:
        print(f"Received ModbusException({exc}) from library")
        client.close()
        return
def read_alarm():
    try:
        data = client.read_holding_registers(0x2203, 1, slave=slave)
        #读到7代表超差报警
        print(f"读取到的寄存器数据: {data.registers}")

    except ModbusException as exc:
        print(f"Received ModbusException({exc}) from library")
        client.close()
        return
def reset_alarm():
    try:
        result = client.write_register(0x1801,0x1111, slave=slave)
        #清除故障


    except ModbusException as exc:
        print(f"Received ModbusException({exc}) from library")
        client.close()
        return
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Modbus RTU Serial Client")
    # parser.add_argument("-p", "--port", default="/dev/ttyUSB0")
    parser.add_argument("-p", "--port", default="COM5")  # windows com
    parser.add_argument("-b", "--baudrate", type=int, default=38400)
    parser.add_argument("-sl", "--slave", type=int, default=1)
    args = parser.parse_args()
    port = args.port
    baudrate = args.baudrate
    slave = args.slave

    client = ModbusClient.ModbusSerialClient(
        port,
        framer=Framer.RTU,
        baudrate=baudrate,
        bytesize=8,
        parity="N",
        stopbits=1,
    )

    # read_alarm()
    # reset_alarm()
    limit_detect()
