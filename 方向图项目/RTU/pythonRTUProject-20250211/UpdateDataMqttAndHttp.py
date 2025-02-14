import datetime
import os
import threading
import paho.mqtt.client as mqtt
import configparser
import PostFile as pf
import curfiles as cf
import globalinfo
import urllib.request
import zipfile
import struct
from threading import Thread
import STJNetMain
import DOcontrol
import time

config = configparser.ConfigParser()

client = None


def testSTJ():
    globalinfo.g_test_PostData = True
    DOcontrol.DO1set(True)

    QuitMqtt() # 采集原始数据时停止MQTT服务

    time.sleep(30)  # 具体时间待测试
    print("获取雷达数据")
    STJNetMain.tcp_recv = True
    STJNetMain.tcp_savedatatofile = False
    test1 = STJNetMain.STJNet()
    test1.connect()

    while globalinfo.g_test_PostData is True:
        time.sleep(1)

    time.sleep(10)
    DOcontrol.DO1set(False)


def Mqtt_post_FFFF(): # 雷达数据上传
    if globalinfo.g_data_lastData == None:
        return False

    info = (b'\xFF\xFF' + globalinfo.g_MQTT_clientid.to_bytes(4, "little") + globalinfo.g_data_lastData)
    globalinfo.g_data_lastData = None
    
    return Mqtt_postMessage(info)


def Mqtt_post_FFFE():
    print("Mqtt上传FFFE指令信息")

    """
    swrt各个值表示：
    1.蓄电池电量：0-100
    2.蓄电池电压：value*0.1
    3.蓄电池充电电流：value*0.01
    4.控制器温度
    5.蓄电池温度
    6.负载电压：value*0.1
    7.负载电流：value*0.01
    8.负载功率
    9.太阳能板电压：value*0.1
    10.太阳能板电流：value*0.01
    11.太阳能板充电功率
    """
    data = (globalinfo.g_MQTT_clientid.to_bytes(4, byteorder='little')              # 客户端ID
            + b'\x01\x00'                                                           # 该设备类型 （目前我默认为1）
            + globalinfo.g_V.to_bytes(4, byteorder='little')                        # 设备软件版本
            + globalinfo.g_4G_CSQ.to_bytes(2, byteorder='little')                   # 4G信号强度 （0~31，,99表示异常）
            + len(globalinfo.g_4G_ICCID).to_bytes(2, byteorder='little')     # ICCID卡号长度
            + globalinfo.g_4G_ICCID.encode("utf-8")                                 # ICCID卡号
            + (globalinfo.g_RAINING and b"\x01" or b"\x00") # python的问号表达式方式   # 是否下雨状态，是则发1，没下雨则是0
            + globalinfo.g_SWRT[0].to_bytes(2, byteorder='little')                  # 蓄电池电量：0-100
            + globalinfo.g_SWRT[1].to_bytes(2, byteorder='little')                  # 蓄电池电压：value*0.1
            + globalinfo.g_SWRT[2].to_bytes(2, byteorder='little')                  # 蓄电池充电电流：value*0.01
            + globalinfo.g_SWRT[3].to_bytes(2, byteorder='little')                  # 控制器温度
            + globalinfo.g_SWRT[4].to_bytes(2, byteorder='little')                  # 蓄电池温度
            + globalinfo.g_SWRT[5].to_bytes(2, byteorder='little')                  # 负载电压：value*0.1
            + globalinfo.g_SWRT[6].to_bytes(2, byteorder='little')                  # 负载电流：value*0.01
            + globalinfo.g_SWRT[7].to_bytes(2, byteorder='little')                  # 负载功率
            + globalinfo.g_SWRT[8].to_bytes(2, byteorder='little')                  # 太阳能板电压：value*0.1
            + globalinfo.g_SWRT[9].to_bytes(2, byteorder='little')                  # 太阳能板电流：value*0.01
            + globalinfo.g_SWRT[10].to_bytes(2, byteorder='little')                 # 太阳能板充电功率
            )

    data = b"\xFF\xFE" + len(data).to_bytes(2, byteorder='little') + data

    Mqtt_postMessage(data)


# 上传告警信息
def Mqtt_PostWarning_FFFC(warningtype, warninginfo):
    print("上传告警信息：",warninginfo)
    bytetype = warningtype.to_bytes(2, byteorder='little')[0:1]
    data = b"\xFF\xFC" + globalinfo.g_MQTT_clientid.to_bytes(4, byteorder='little') + bytetype + len(warninginfo).to_bytes(2, byteorder='little') + str.encode(warninginfo) # warninginfo.decode("utf-8")
    Mqtt_postMessage(data)


# 拍照或者录视频回复指令:任务ID?
def Mqtt_PostTakePicOrVideo_FFFB(path):
    print("拍照或录视频回复")
    data = b"\xFF\xFB" + globalinfo.g_MQTT_clientid.to_bytes(4, byteorder='little') + globalinfo.g_one_TASKID + len(path).to_bytes(2, byteorder='little') + str.encode(path) # path.decode("utf-8")
    Mqtt_postMessage(data)



# data = b'\xaa\n\x00\xa4\x9b\x86\x92\x01\x00\x00\x00\x00\xc2\x8b\x92\x01\x00\x00'
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc)) #打印连接状态
    if rc == 0:
        globalinfo.g_RUN_MQTT = True
    client.subscribe("$creq/" + str(globalinfo.g_MQTT_clientid) + "/cmd") #订阅消息


def message_handle(message, data):
    if message == b"\xaa\x01":  # 设置角度
        return
    elif message == b"\xaa\x02":  # 重启

        # file = open("/mnt/sdcard/infotest.dat", "ab+")
        # str_hex = datetime.strftime("%Y-%m-%d %H:%M:%S ") + "准备进行重启\n"
        # file.write(str_hex.encode())
        # file.close()
        
        globalinfo.SaveIni()

        if globalinfo.isdebug is False:
            os.system("reboot -f")
        return
    elif message == b"\xaa\x03":  # 传输周期(其实是数据传输周期)
        globalinfo.g_STATUS_period = data[0] * 60 * 60 + data[1] * 60 + data[2]
        print("传输周期", globalinfo.g_STATUS_period)
        globalinfo.g_RESTART_SCHEDULE = True
        return
    elif message == b"\xaa\x04":  # 采集周期
        globalinfo.g_GATHER_period = data[0] * 60 * 60 + data[1] * 60 + data[2]
        print("采集周期", globalinfo.g_GATHER_period)
        globalinfo.g_RESTART_SCHEDULE = True
        return
    elif message == b"\xaa\x05":  # 采集次数
        globalinfo.g_GATHER_degree = data[0]
        print("采集次数", globalinfo.g_GATHER_degree)
        return
    elif message == b"\xaa\x06":  # 固件升级
        datalengh = int.from_bytes(data[0:2], byteorder='little')
        url = data[2:datalengh + 2].decode("utf-8")

        # file = open("/mnt/sdcard/infotest.dat", "ab+")
        # str_hex = datetime.strftime("%Y-%m-%d %H:%M:%S ") + "准备进行固件升级\n"
        # file.write(str_hex.encode())
        # file.close()

        try:
            tmp = urllib.request.urlretrieve(url, globalinfo.g_updatepath + "update.zip")
            print("下载成功")

            info = b'download success'

            sendbytes = b'\xFF\xFC' + int(globalinfo.g_MQTT_clientid).to_bytes(4, byteorder='little') + b'\x01' + len(info).to_bytes(2, byteorder='little') + info

            Mqtt_postMessage(sendbytes)
        except Exception as e:
            info = b'download failed'
            sendbytes = b'\xFF\xFC' + int(globalinfo.g_MQTT_clientid).to_bytes(4, byteorder='little') + b'\xFF' + len(info).to_bytes(2, byteorder='little') + info
            Mqtt_postMessage(sendbytes)
            return

        try:
            zipfile.ZipFile(globalinfo.g_updatepath + "update.zip").extractall(globalinfo.g_updatepath)
            info = b'begin update'
            sendbytes = b'\xFF\xFC' + int(globalinfo.g_MQTT_clientid).to_bytes(4, byteorder='little') + b'\x01' + len(info).to_bytes(2, byteorder='little') + info
            Mqtt_postMessage(sendbytes)
            globalinfo.SaveIni()
            os.remove(globalinfo.g_updatepath + "update.zip")

            # file = open("/mnt/sdcard/infotest.dat", "ab+")
            # str_hex = datetime.strftime("%Y-%m-%d %H:%M:%S ") + "升级文件解压缩成功，准备重启\n"
            # file.write(str_hex.encode())
            # file.close()

            if globalinfo.isdebug is False:
                os.system("reboot -f")

            return

        except Exception as e:
            print(e)  # 异常状态
            info = b'unzip file failed'
            sendbytes = b'\xFF\xFC' + int(globalinfo.g_MQTT_clientid).to_bytes(4, byteorder='little') + b'\xFF' + len(info).to_bytes(2, byteorder='little') + info
            Mqtt_postMessage(sendbytes)

        # urlt = "http://222.74.26.58:9998/jeecg-boot//sys/common/static/upgrade/Update1.2.1.3.zip"
        # tmp = urllib.request.urlretrieve(urlt, "D:/test/update.zip")  # 如果指定文件不存在，这里会异常，需要使用try语句来执行下载

        return
    elif message == b"\xaa\x07":  # 控制拍照

        globalinfo.g_one_PicorVideoGraph = data[0:1]
        globalinfo.g_one_VIDEO_timelen = int.from_bytes(data[1:3], byteorder='little')
        globalinfo.g_one_width = int.from_bytes(data[3:5], byteorder='little')
        globalinfo.g_one_height = int.from_bytes(data[5:7], byteorder='little')
        globalinfo.g_one_TASKID = data[7:15]
        globalinfo.g_one_time = data[15:23]
        
        print("控制拍照")

        globalinfo.g_one_PhotoGraph = True
        return
    elif message == b"\xaa\x08":  # 视频拍摄时间段与频率

        begintime = str(int.from_bytes(data[0:2], byteorder='little')) + str(100 + data[2])[1:3] + str(100 + data[3])[1:3] + "000000"
        globalinfo.g_VIDEO_begintime = datetime.datetime.strptime(begintime, "%Y%m%d%H%M%S")
        endtime = str(int.from_bytes(data[4:6], byteorder='little')) + str(100 + data[6])[1:3] + str(100 + data[7])[1:3] + "000000"
        globalinfo.g_BIDEO_endtime = datetime.datetime.strptime(endtime, "%Y%m%d%H%M%S")

        globalinfo.g_VIDEO_width = int.from_bytes(data[8:10], byteorder='little')
        globalinfo.g_VIDEO_height = int.from_bytes(data[10:12], byteorder='little')
        globalinfo.g_VIDEO_timelen = int.from_bytes(data[12:14], byteorder='little')

        num = data[14]
        part = 15
        globalinfo.g_VIDEO_VCRtime = []
        for i in range(num):
            timev = str(100 + data[part + i * 3 + 0])[1:3] + ":" + str(100 + data[part + i * 3 + 1])[1:3] + ":" + str(100 + data[part + i * 3 + 2])[1:3]
            globalinfo.g_VIDEO_VCRtime.append(timev)
            
        globalinfo.g_RESTART_SCHEDULE = True
        return
    elif message == b"\xaa\x09":  # 设置节能模式
        globalinfo.g_ISENERGY = bool(data[0])
        print("设置节能模式", globalinfo.g_ISENERGY)
        # gv.set_value("Type_relses", 1)
        return
    elif message == b"\xaa\x0a":  # 照片上传与视频上传列表
        begintime = int.from_bytes(data[0:8], byteorder='little')
        endtime = int.from_bytes(data[8:16], byteorder='little')
        begintime_stamp = datetime.datetime.fromtimestamp(begintime * 0.001)
        endtime_stamp = datetime.datetime.fromtimestamp(endtime * 0.001)

        begintime = begintime_stamp.strftime("%Y%m%d%H%M%S")
        endtime = endtime_stamp.strftime("%Y%m%d%H%M%S")
        begintime = int(begintime)
        endtime = int(endtime)

        list = cf.findAllFiles(globalinfo.g_videopath, ".mjpg")
        print("视频列表", list)
        listname = []
        for strname in list:
            name = strname.split('.')
            listname.append(name[0])

        strname = ""
        for i in listname:
            if (begintime <= int(i) <= endtime):
                strname += i + ".mp4;"

        list = cf.findAllFiles(globalinfo.g_picpath, ".jpg")
        print("图片列表", list)
        for strname in list:
            name = strname.split('.')
            listname.append(name[0])

        strname = ""
        for i in listname:
            if (begintime <= int(i) <= endtime):
                strname += i + ".jpg;"

        strbytes = strname.encode('utf-8')
        lenbytes = len(strname).to_bytes(2, byteorder='little')
        sendbytes = b'\xff\xfd' + int(globalinfo.g_MQTT_clientid).to_bytes(4, "little") + data[0:16] + lenbytes + strbytes

        Mqtt_postMessage(sendbytes)
        return
    elif message == b"\xaa\x0b":  # 照片上传与视频上传文件
        namelen = int.from_bytes(data[0:2], byteorder='little') - 2   # 数据总长

        if len(data) != namelen:
            return

        namelen = int.from_bytes(data[2:4], byteorder='little')

        filename = data[4:namelen + 4].decode("utf-8")
        filelist = filename.split(';')

        piclist = []
        videolist = []

        for strname in filelist:
            if strname == "":
                continue

            tmp = strname.split(".")
            if tmp[1] == "mjpg":
                videolist.append(strname)
            if tmp[1] == "jpg":
                piclist.append(strname)

        httplen = int.from_bytes(data[namelen + 4: namelen + 6], byteorder='little')
        httppath = data[namelen + 6 : namelen + 6 + httplen].decode("utf-8")

        for strname in piclist:
            ret = pf.post_file(httppath, globalinfo.g_MQTT_clientid, globalinfo.g_picpath + strname)
            if ret == True:
                print("图片上传成功")
            else:
                print("图片上传失败")

        for strname in videolist:
            ret = pf.post_file(httppath, globalinfo.g_MQTT_clientid, globalinfo.g_picpath + strname)
            if ret == True:
                print("视频上传成功")
            else:
                print("视频上传失败")

        return
    elif message == b"\xaa\x0c":  # 获取状态
        print("获取状态")
        Mqtt_post_FFFE()
        return
    elif message == b"\xaa\x0d":  # 设置连接地址，端口，用户名，密码
        infolen = int.from_bytes(data[0:2], byteorder='little')# 数据总长
        strname = data[2:2 + infolen].decode("utf-8")
        strlist = strname.split(";")
        
        globalinfo.g_MQTT_host = strlist[0]
        globalinfo.g_MQTT_port = int(strlist[1])
        globalinfo.g_MQTT_user = strlist[2]
        globalinfo.g_MQTT_password = strlist[3]
        globalinfo.g_RESTARTMQTT = True
        
        if len(strlist) == 5:
            globalinfo.g_MQTT_clientid = int(strlist[4])
        
        print("MQTT重设连接参数",globalinfo.g_MQTT_host,globalinfo.g_MQTT_port,globalinfo.g_MQTT_user,globalinfo.g_MQTT_password)
        
        return
    elif message == b"\xaa\x0e": # 采集配置
        print("采集配置")
        globalinfo.g_STATUS_period = data[0] * 60 * 60 + data[1] * 60 + data[2]
        globalinfo.g_GATHER_period = data[3] * 60 * 60 + data[4] * 60 + data[5]
        globalinfo.g_GATHER_degree = data[6]
        globalinfo.g_RESTART_SCHEDULE = True
        return
    elif message == b"\xaa\x0f": # 本地数据文件上传
        begintime = int.from_bytes(data[0:8], byteorder='little')
        endtime = int.from_bytes(data[8:16], byteorder='little')
        globalinfo.g_data_begintime = datetime.datetime.fromtimestamp(begintime * 0.001)
        globalinfo.g_data_endtime = datetime.datetime.fromtimestamp(endtime * 0.001)
        globalinfo.g_data_taskID = int.from_bytes(data[16:24], byteorder='little')
        lengh = int.from_bytes(data[24:26], byteorder='little')
        globalinfo.g_data_posthost = data[26:26 + lengh].decode("utf-8")
        globalinfo.g_data_SendUp = True
        print("本地数据文件上传", globalinfo.g_data_posthost)
        return
    elif message == b"\xaa\x10": # 测试用，原始数据上传
        print("原始数据上传")
        STJNetMain.tcp_datasavepath = globalinfo.g_testpath
        STJNetMain.tcp_recv = True
        if data[0:1] == b'\x01' and globalinfo.g_test_PostData is False:
            globalinfo.g_test_PostData = True
            globalinfo.g_test_SaveFileToPost = int.from_bytes(data[1:3], byteorder='little')
            hostlen = int.from_bytes(data[3:5], byteorder='little')
            globalinfo.g_test_PostHost = data[5:hostlen].decode("utf-8")
            threading.Thread(target=testSTJ).start()
        elif data[0:1] == b'\x00' and globalinfo.g_test_PostData is True:
            globalinfo.g_test_PostData = False
            time.sleep(5)
            threading.Thread(target=STJNetMain.TestPostFile).start()
        return
    elif message == b"\xaa\x11": # 打开或关闭报警
        if data[0:1] == b'\x01':
            globalinfo.g_ISWARING = True
        else:
            globalinfo.g_ISWARING = False


def on_message(client, userdata, msg):
    print(msg.topic+" " + ":" + str(msg.payload)) #打印接受的消息
    message_handle(message=msg.payload[0:2], data=msg.payload[2:len(msg.payload)])


def Mqtt_postMessage(data):
    global client
    try:
        if client is not None:
            client.publish("$dp", data, 1, False)
        # publish.single("$dp", auth={"username": globalinfo.g_MQTT_user, "password": globalinfo.g_MQTT_password}, hostname=globalinfo.g_MQTT_host, payload=data, qos=1,
        #               retain=False, port=globalinfo.g_MQTT_port, client_id=str(globalinfo.g_MQTT_clientid), keepalive=10)
        print("发送指令成功")
        return True
    except Exception as e:
        print(e)
        return False


def on_disconnect(client, userdata, rc):
    # globalinfo.g_RUN_MQTT = False
    pass


def mqtt_subscribe():
    global client

    if client is not None:
        client.loop_start()
    #client.loop_forever()
    

def connect(client_ID, username="admin", password="123456", localhost="bjfxt.mypattern.cn", port=1883, keepalive=300):
    global client
    
    if not client or not client.is_connected():
        try:
            client = mqtt.Client(client_id=str(client_ID), clean_session=True)    # ClientId不能重复，所以使用当前时间
            client.username_pw_set(username=username, password=password)  # 必须设置，否则会返回「Connected with result code 4」
            client.on_connect = on_connect
            client.on_message = on_message
            client.on_disconnect = on_disconnect
            
            client.connect(host=localhost, port=port, keepalive=keepalive)
            
            subscribe_thread = Thread(target=mqtt_subscribe)
            subscribe_thread.start()
        except Exception as e:
            client = None
            print(e)
        
    

def getstatus():
    global client
    if client == None:
        return False
    else:
        return True


def connetMqtt():
    globalinfo.ReadIni()
    return connect(globalinfo.g_MQTT_clientid, username=globalinfo.g_MQTT_user, password=globalinfo.g_MQTT_password, localhost=globalinfo.g_MQTT_host, port=globalinfo.g_MQTT_port)


def QuitMqtt():
    global client

    if client is not None:
        client.disconnect()
        client = None

    globalinfo.SaveIni()



if __name__ == '__main__':
    connetMqtt()