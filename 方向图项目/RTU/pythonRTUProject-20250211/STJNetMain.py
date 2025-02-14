import os
import zipfile
import socket
import time
import binascii
import SlopeRadarAlgorithmV5 as sra
from datetime import datetime
import globalinfo
from threading import Thread
import threading
import PostFile as pf


tcpMessage = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpData = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_recv = True
tcp_savedatatofile = False
tcp_datasavepath = None


def TestPostFile():
    print("上传原始数据压缩文件")

    # file = open("/mnt/sdcard/infotest.dat", "ab+")
    # str_hex = datetime.strftime("%Y-%m-%d %H:%M:%S ") + "上传压缩文件\n"
    # file.write(str_hex.encode())
    # file.close()

    for root, dirs, files in os.walk(globalinfo.g_testpath):  # './mnt/sdcard/video/'
        # 指定目录，返回一个三元组（起始路径，起始路径下的文件夹名，起始路径下的文件名）
        if len(files) > 0:
            zipname = datetime.now().strftime("%Y_%m_%d_%H_%M_%S") + ".zip"
            zp = zipfile.ZipFile(globalinfo.g_testpath + zipname, "w", zipfile.ZIP_DEFLATED)

            for file in files:
                zp.write(globalinfo.g_testpath + file)
            zp.close()

            for file in files:
                os.remove(globalinfo.g_testpath + file)

            ret = pf.post_file(globalinfo.g_test_PostHost, globalinfo.g_MQTT_clientid, globalinfo.g_testpath + zipname,
                               True)

            if ret is True:
                os.system("reboot -f")



def recvMessageThread():
    global tcp_recv
    global tcpMessage
    
    print("开始接收雷达消息")

    while tcp_recv:
        # message = tcpMessage.recv(1024)
        try:
            message = tcpMessage.recv(2048)
        except socket.timeout:
            return False
        except (OSError, BrokenPipeError):
            return False
        except AttributeError:
            return False
        except Exception as e:
            return False
        else:
            if message == 0:
                return True
            else:
                # 接收消息处理
                return True


def recvDataThread():
    global tcp_datasavepath
    global tcp_recv
    global tcpData

    iNum = 0
    print("开始获取雷达数据")

    # file = open("/mnt/sdcard/infotest.dat", "ab+")
    # str_hex = datetime.strftime("%Y-%m-%d %H:%M:%S ") + "准备进行数据采集\n"
    # file.write(str_hex.encode())
    # file.close()

    sral = sra.Algorithm()
    pathname = tcp_datasavepath + datetime.utcnow().strftime('%Y_%m_%d_%H_%M_%S-')

    recvnum = 0
    while tcp_recv and recvnum < globalinfo.g_GATHER_degree:
        try:
            message = tcpData.recv(62000)
            data = sral.bytetoinfo(message)
            
            if data != None:
                globalinfo.g_data_lastData = data

            if tcp_savedatatofile is True:
                recvnum += 1
                file = open(pathname + str(recvnum) + ".dat", "ab+")
                str_hex = str(binascii.b2a_hex(message))[2:-1] + "\n"
                file.write(str_hex.encode())
                file.close()

                # file = open(pathname + str(recvnum) + ".mud", "ab+")
                # str_hex = str(binascii.b2a_hex(data))[2:-1]
                # file.write(str_hex.encode())
                # file.close()

                iNum += 1
            elif globalinfo.g_test_PostData is True:
                filepath = globalinfo.g_testpath + datetime.utcnow().strftime('%Y_%m_%d_%H_%M_%S.%f')[:-3] + ".dat"
                file = open(filepath, "ab+")
                file.write(message)
                file.close()

                globalinfo.g_test_SaveFileNum += 1

                if globalinfo.g_test_SaveFileNum >= globalinfo.g_test_SaveFileToPost:
                    print("压缩待上传原始数据")
                    globalinfo.g_test_SaveFileNum = 0
                    subscribe_thread = threading.Thread(target=TestPostFile)
                    subscribe_thread.start()
                    tcp_recv = False

            elif globalinfo.g_test_PostData is False:
                break
        except socket.timeout:
            continue
        except (OSError, BrokenPipeError):
            continue
        except AttributeError:
            continue
        except Exception as e:
            continue
        else:
            if message == 0:
                pass
            else:
                # 接收消息处理
                pass
    print("Thread data stop\n")

    tcp_recv = False


class STJNet:
    def __init__(self):
        global tcpMessage
        global tcpData
        global tcp_recv

        tcp_recv = False
        self.messageAddress = ('192.168.1.2', 6006)
        self.dataAddress = ('192.168.1.2', 9009)
        self.bufferSize = 65000

        self.thread_message = None
        self.thread_data = None

        pass

    def setAddress(self, ip, portmessage, portdata):
        self.messageAddress = (ip, portmessage)
        self.dataAddress = (ip, portdata)


    def setBufferSize(self, size):
        self.bufferSize = size


    def connect(self):
        tcpMessage.connect(self.messageAddress)
        tcpData.connect(self.dataAddress)
        # gv.set_value('STJ', 1)

        global tcp_recv
        tcp_recv = True
        self.thread_message = threading.Thread(target=recvMessageThread, name="STJrecvMessage")
        self.thread_message.start()

        self.thread_data = threading.Thread(target=recvDataThread, name="STJrecvData")
        self.thread_data.start()

    def disconnect(self):
        global tcp_recv
        tcp_recv = False
        tcpMessage.close()
        tcpData.close()


    def sendMessage(self, message):
        try:
            send_value = tcpMessage.send(message.encode('utf-8'))
        except socket.timeout:
            return False
        except (OSError, BrokenPipeError):
            self.disconnect()
            return False
        except AttributeError:
            return False
        except Exception as e:
            return False
        else:
            if send_value == 0:
                self.disconnect()
                return False
        return True


if __name__ == '__main__':
    print("开始连接")
    test1 = STJNet()
    test1.connect()
    time.sleep(120)
    # tcp_recv = False
    # time.sleep(1)
    test1.disconnect()

    print("STJ stop\n")
