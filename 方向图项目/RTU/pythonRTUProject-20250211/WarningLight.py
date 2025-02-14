import serial
import time

serial485_1 = '/dev/tty485_1'
serial485_2 = '/dev/tty485_2'
serial485_3 = '/dev/tty485_3'
serial485_4 = '/dev/tty485_4'
serial232_1 = '/dev/tty232_1'
serial232_2 = '/dev/tty232_2'

serialPort = 'COM5'


class WarningLight(object):
    def __init__(self, port, baudrate=9600, bytesize=8, parity='N', stopbits=1, timeout=0.5):
        try:
            self.serial = serial.Serial(port=port, baudrate=baudrate, bytesize=bytesize, parity=parity,
                                        stopbits=stopbits, timeout=timeout)
        except Exception as e:
            self.serial = None
            print(e)

    # 播放指定文件夹下的指定文件
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def playfolderfile(self, foldername, filename, feedback=b'\x00'):
        self.basewritecommand(b'\x0F', feedback=feedback, para1=foldername, para2=filename)

    # 下一首
    def playnextfile(self):
        self.basewritecommand(command=b'\x01')

    # 上一首
    def playlastfile(self):
        self.basewritecommand(command=b'\x02')

    # 播放指定曲目
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def playspecifiedfile(self, filename, feedback=b'\x00'):
        self.basewritecommand(command=b'\x03', feedback=feedback, para2=filename)

    # 音量增加1
    def volumeup(self):
        self.basewritecommand(command=b'\x04')

    # 音量减小1
    def volumedown(self):
        self.basewritecommand(command=b'\x05')

    # 指定音量，最大30，默认30
    def setvolume(self, volume):
        self.basewritecommand(command=b'\x06', para2=volume)

    # 指定曲目循环播放
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def loopplaybackfile(self, file, feedback=b'\x00'):
        self.basewritecommand(command=b'\x08', feedback=feedback, para2=file)

    # 指定播放设备
    def assignplaydevice(self, flash):
        self.basewritecommand(command=b'\x09', para2=flash)

    # 设定波特率
    # 1:9600  2:19200  3:38400  4:57600  5:115200  6:256000  9:4800
    def setbaudrate(self, baudrate):
        self.basewritecommand(command=b'\x0B', para2=baudrate)

    # 设备复位
    def resetdevice(self):
        self.basewritecommand(command=b'\x0C')

    # 继续播放
    def goonplay(self):
        self.basewritecommand(command=b'\x0D')

    # 暂停
    def pauseplay(self):
        self.basewritecommand(command=b'\x0E')

    # 指定文件夹文件循环播放
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def loopplaybackfolderfile(self, folder, file, feedback=b'\x00'):
        self.basewritecommand(command=b'\x10', feedback=feedback, para1=folder, para2=file)

    # 循环播放全部曲目
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def loopplaybackallplay(self, feedback=b'\x00'):
        self.basewritecommand(command=b'\x11', feedback=feedback, para2=b'\x01')

    # 停止循环播放全部曲目
    def loopplaybackallstop(self):
        self.basewritecommand(command=b'\x11')

    # 停止播放
    def stopplay(self):
        self.basewritecommand(command=b'\x16')

    # 文件夹循环
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def loopplayfolder(self, folder, feedback=b'\x00'):
        self.basewritecommand(command=b'\x17', feedback=feedback, para2=folder)

    # 随机播放
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def randomplay(self, feedback=b'\x00'):
        self.basewritecommand(command=b'\x18', feedback=feedback)

    # 单曲循环
    # feedback: 0X:爆闪 1X:慢闪 2X:常亮 3X:爆闪，播放完保持  4X:慢闪，播放完保持  5X:常亮，播放完保持  6X:不亮灯
    #           X0:无反馈，播放完反馈  X1：有反馈，播放完反馈  X2：无反馈，播放完无反馈
    def singlecycleplay(self, feedback=b'\x00'):
        self.basewritecommand(command=b'\x19', feedback=feedback)

    # 单曲循环关闭
    def singlecyclestop(self):
        self.basewritecommand(command=b'\x19', para2=b'\x01')

    # 设定地址
    def setaddress(self, address):
        self.basewritecommand(command=b'\xC0', para2=address)

    # 单独设定报警灯
    # 3:爆闪  4：慢闪  5：常亮  6：关闭
    def setlightstate(self, lightstate):
        self.basewritecommand(command=b'\xC2', para2=lightstate)

    # 设备状态
    def devicestatus(self):
        if self.serial is not None:
            return self.serial.is_open
        return False

    def connect(self):
        if self.serial is None:
            return False
        if not self.serial.is_open:
            return self.serial.open()
        else:
            return True

    def disconnect(self):
        if self.serial is not None:
            self.serial.close()

    def basewritecommand(self, command=b'\xC2', feedback=b'\x00', para1=b'\x00', para2=b'\x03'):
        vb = 65536
        va = vb - ord(b'\xFF') - ord(b'\x06') - ord(command) - ord(feedback) - ord(para1) - ord(para2)
        data = b'\x7E\xFF\x06' + bytes(command) + bytes(feedback) + bytes(para1) + bytes(para2) + va.to_bytes(2,
                                                                                                              'big') + b'\xEF'
        if self.serial is not None:
            self.serial.write(data)

    def getrecv(self):
        if self.serial is None:
            return False

        data = self.serial.read(100)

        if data[0] == b'\x7E' and data[1] == b'\xFF' and data[2] == b'\x06' and data[3] == b'\x3E':
            return True
        else:
            return False


if __name__ == '__main__':
    com = WarningLight(serialPort)
    com.connect()
    com.loopplaybackfolderfile(b'\x01',b'\x01')
    time.sleep(5)
    com.singlecyclestop()
    com.disconnect()
