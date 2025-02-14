from pymodbus.client.serial import ModbusSerialClient as ModbusClient


serial485_1 = '/dev/tty485_1'
serial485_2 = '/dev/tty485_2'
serial485_3 = '/dev/tty485_3'
serial485_4 = '/dev/tty485_4'
serial232_1 = '/dev/tty232_1'
serial232_2 = '/dev/tty232_2'

# 雨雪传感器
class ZHYX(object):
    def __init__(self, port, baudrate = 9600, bytesize = 8, parity = 'N', stopbits = 1,timeout = 0.5):
        self.serial = ModbusClient(port= port, baudrate= baudrate, bytesize= bytesize, parity= parity,
                                   stopbits= stopbits, timeout= timeout)

    @staticmethod
    def getdevicetypeinfo():
        return "ZH-YX-2"

    def devicestatus(self):
        return self.serial.state

    def connect(self):
        return self.serial.connect()

    def disconnect(self):
        self.serial.close()

    # 获取当前是否下雨的状态
    def rainingorsnowing(self):
        recv = self.serial.read_holding_registers(0x00, 1, 1)
        print(recv)

        if recv.isError():
            return False
        else:
            return recv.registers[0]

    def autouploadtime(self, time = 1):
        self.serial.write_register(0x0085, value=int(time))

    def headerbegin(self, T = 15.0):
        self.serial.write_register(0x00C0, value=int(T * 10))

    def headerend(self, T = 25.0):
        self.serial.write_register(0x00C2, value=int(T * 10))

    def setbaudrate(self, baudrate = 9600):
        values = {2400:0x2400, 4800:0x4800, 9600:0x9600, 14400:0x1440, 19200:0x1920, 38400:0x3840, 115200:0x1152}
        self.serial.write_register(0x0081, values.get(baudrate))
        
        
if __name__ == "__main__":
    zhyx485 = ZHYX('/dev/tty485_2')
    nR = zhyx485.connect()
    print(nR)
    print(zhyx485.devicestatus())

    if nR is True:
        # 获取当前雨雪状态
        g_RAINING = zhyx485.rainingorsnowing()
        print(g_RAINING)
        zhyx485.disconnect()