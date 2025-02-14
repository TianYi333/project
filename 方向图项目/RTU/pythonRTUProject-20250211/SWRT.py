from pymodbus.client.serial import ModbusSerialClient as ModbusClient


serial485_1 = '/dev/tty485_1'
serial485_2 = '/dev/tty485_2'
serial485_3 = '/dev/tty485_3'
serial485_4 = '/dev/tty485_4'
serial232_1 = '/dev/tty232_1'
serial232_2 = '/dev/tty232_2'

# 太阳能控制器
class SWRT(object):
    def __init__(self, port, baudrate = 9600, bytesize = 8, parity = 'N', stopbits = 1,timeout = 0.5):
        self.serial = ModbusClient(port= port, baudrate= baudrate, bytesize= bytesize, parity= parity,
                                   stopbits= stopbits, timeout= timeout)

    @staticmethod
    def getdevicetypeinfo():
        return "SWRT1024N-T"

    def devicestatus(self):
        return self.serial.state

    def connect(self):
        return self.serial.connect()

    def disconnect(self):
        self.serial.close()

    """
    获取数据如果为空，表示状态异常
    如果不为空，则表示：
    1.蓄电池电量
    2.蓄电池电压
    3.蓄电池充电电流
    4.控制器温度
    5.蓄电池温度
    6.负载电压
    7.负载电流
    8.负载功率
    9.太阳能板电压
    10.太阳能板电流
    11.太阳能板充电功率
    """
    def get_readinfo(self):
        recv = self.serial.read_holding_registers(0x0100, 10, 1)

        if recv.isError():
            return None

        tmp = recv.registers[3] % 256

        if tmp & 0x80 > 0: # 当前温度值为负
            tmp = tmp & 0x7f * -1

        tmp1 = recv.registers[3] >> 8

        if tmp1 & 0x80 > 0:
            tmp1 = tmp1 & 0x7f * -1

        data = [recv.registers[0], recv.registers[1], recv.registers[2], tmp1, tmp,
                recv.registers[4], recv.registers[5], recv.registers[6],
                recv.registers[7], recv.registers[8], recv.registers[9]]
        print(data)
        return data


if __name__ == '__main__':
    dw = SWRT(serial485_1)
    dw.connect()
    data = dw.get_readinfo()
    dw.disconnect()