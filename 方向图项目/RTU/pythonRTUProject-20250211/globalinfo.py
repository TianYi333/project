# -*- coding: utf-8 -*-
import configparser
from datetime import datetime

isdebug = False

"""
初始默认路径：
log路径：/mnt/sdcard/log
视频路径：/mnt/sdcard/video
照片路径：/mnt/sdcard/picture
数据路径：/mnt/sdcard/data
升级包路径：/mnt/sdcard/update
状态信息路径：/mnt/sdcard/status
配置文件路径：/mnt/sdcard/ini

DO控制：
DO1：雷达
DO2：雨雪传感器
DO3：报警灯
"""
g_logpath = "/mnt/sdcard/log/"
g_videopath = "/mnt/sdcard/video/"
g_picpath = "/mnt/sdcard/picture/"
g_datapath = "/mnt/sdcard/data/"
g_updatepath = "/mnt/sdcard/update/"
g_statuspath = "/mnt/sdcard/status/"
g_inipath = "/mnt/sdcard/ini/"
g_testpath = "/mnt/sdcard/test/"
g_V = 1


"""
SIM卡状态
4G网络状态
ICCID字符串值
信号强度
IMEI值
"""
g_4G_SIM = False
g_4G_net = False
g_4G_ICCID = ""
g_4G_CSQ = 0
g_4G_IMEI = ""

"""
太阳能板返回值
雨雪状态
是否为报警状态
"""
g_SWRT = b""
g_RAINING = False
g_ISWARING = False

"""
状态传输周期
雷达信号采集周期
单次采集采集次数
拍照
视频拍摄时间段与频率
节能模式
MQTT连接地址
端口
用户名
密码
"""
g_STATUS_period = 60 # 这里是雷达数据上传的周期，不是状态
g_GATHER_period = 60
g_GATHER_degree = 10
g_one_PhotoGraph = False
g_one_PicorVideoGraph = False # 拍照还是录像，1是拍照，2是录像
g_one_width = 1080
g_one_height = 720
g_one_VIDEO_timelen = 1000
g_one_TASKID = 0
g_one_time = 0
g_VIDEO_beginVideo = False
g_VIDEO_begintime = datetime.strptime("2024-12-12 00:00:00", "%Y-%m-%d %H:%M:%S")
g_VIDEO_endtime = datetime.strptime("2024-12-12 23:59:59", "%Y-%m-%d %H:%M:%S")
g_VIDEO_width = 1080
g_VIDEO_height = 768
g_VIDEO_timelen = 5000
g_VIDEO_VCRtime = [] # "09:20:00", "10:20:00"]
g_ISENERGY = False
g_MQTT_clientid = 10086
g_MQTT_host = ""
g_MQTT_port = 1883
g_MQTT_user = ""
g_MQTT_password = ""


# 重启服务，在更新连接信息后
g_RESTARTMQTT = False

# 最新拍照文件存储路径
g_pic_lastFilePath = None
g_video_lastFilePath = None

# 上传历史数据时间范围及post路径
g_data_begintime = datetime.strptime("2024-12-12 00:00:00", "%Y-%m-%d %H:%M:%S")
g_data_endtime = datetime.strptime("2024-12-12 23:59:59", "%Y-%m-%d %H:%M:%S")
g_data_posthost = None  # 数据上传网址
g_data_SendUp = False  # 数据是否上传
g_data_taskID = None	# 数据上传所需的ID号

g_data_lastData = None  # 最后一帧的数据

g_test_PostData = False  # 测试进行原始数据采集
g_test_PostHost = None   # 测试数据发送地址
g_test_SaveFileNum = 0   # 测试数据已经存储数量
g_test_SaveFileToPost = 100 # 测试数据文件达到该值则进行压缩上传

# MQTT服务是否已经启动
g_RUN_MQTT = False
# SCHEDULE定时任务是否已经启动
g_RUN_SCHEDULE = False
g_RESTART_SCHEDULE = False


def ReadIni():
    class DefaultOption(dict):

        def __init__(self, config, section, **kv):
            self._config = config
            self._section = section
            dict.__init__(self, **kv)

        def items(self):
            _items = []
            for option in self:
                if not self._config.has_option(self._section, option):
                    _items.append((option, self[option]))
                else:
                    value_in_config = self._config.get(self._section, option)
                    _items.append((option, value_in_config))
            return _items

    config = configparser.ConfigParser()

    global g_STATUS_period  # 状态传输周期
    global g_GATHER_period  # 雷达信号采集周期
    global g_GATHER_degree  # 单次采集采集次数
    global g_VIDEO_begintime
    global g_VIDEO_endtime
    global g_VIDEO_width
    global g_VIDEO_height
    global g_VIDEO_timelen
    global g_VIDEO_VCRtime
    global g_ISENERGY  # 是否为节能模式
    global g_MQTT_clientid
    global g_MQTT_host
    global g_MQTT_port
    global g_MQTT_user
    global g_MQTT_password

    global g_inipath

    config.read(g_inipath + "config.ini")
    try:
        g_STATUS_period = config.getint("Status", "StatusPeriod", vars=DefaultOption(config, "Status", StatusPeriod=60))
        g_GATHER_period = config.getint("Status", "GatherPeriod", vars=DefaultOption(config, "Status", GatherPeriod=60))
        g_GATHER_degree = config.getint("Status", "GatherDegree", vars=DefaultOption(config, "Status", GatherDegree=10))

        tmp = config.get("Video", "BeginTime", vars=DefaultOption(config, "Video", BeginTime="2024-12-12 00:00:00"))
        g_VIDEO_begintime = datetime.strptime(tmp, "%Y-%m-%d %H:%M:%S")
        tmp = config.get("Video", "EndTime", vars=DefaultOption(config, "Video", EndTime="2024-12-12 23:59:59"))
        g_VIDEO_endtime = datetime.strptime(tmp, "%Y-%m-%d %H:%M:%S")
        g_VIDEO_width = config.getint("Video", "Width", vars=DefaultOption(config, "Video", Width=1080))
        g_VIDEO_height = config.getint("Video", "Height", vars=DefaultOption(config, "Video", Height=768))
        g_VIDEO_timelen = config.getint("Video", "Timelen", vars=DefaultOption(config, "Video", Timelen=1000))
        num = config.getint("Video", "VCRNumber", vars=DefaultOption(config, "Video", VCRNumber=0))

        for i in range(num):
            g_VIDEO_VCRtime = config.get("VCRTime", str(i), vars=DefaultOption(config, "VCRTime", i="09:00:00"))

        g_MQTT_host = config.get("Mqtt", "Host", vars=DefaultOption(config, "Mqtt", Host="bjfxt.mypattern.cn"))
        g_MQTT_port = config.getint("Mqtt", "Port", vars=DefaultOption(config, "Mqtt", Port=1883))
        g_MQTT_clientid = config.getint("Mqtt", "Client_id", vars=DefaultOption(config, "Mqtt", Client_id=10086))
        g_MQTT_user = config.get("Mqtt", "User", vars=DefaultOption(config, "Mqtt", User="admin"))
        g_MQTT_password = config.get("Mqtt", "Password", vars=DefaultOption(config, "Mqtt", Password="123456"))


    except configparser.NoOptionError:
        print("Error1")
        g_STATUS_period = 60  # 状态传输周期
        g_GATHER_period = 60  # 雷达信号采集周期
        g_GATHER_degree = 10  # 单次采集采集次数
        g_VIDEO_begintime = datetime.datetime.strptime("2024-12-12 00:00:00", "%Y-%m-%d %H:%M:%S")
        g_VIDEO_endtime = datetime.datetime.strptime("2024-12-12 23:59:59", "%Y-%m-%d %H:%M:%S")
        g_VIDEO_width = 1080
        g_VIDEO_height = 768
        g_VIDEO_timelen = 1000
        g_VIDEO_VCRtime = []
        g_ISENERGY = False  # 是否为节能模式
        g_MQTT_clientid = 10086
        g_MQTT_host = "bjfxt.mypattern.cn"
        g_MQTT_port = 1883
        g_MQTT_user = "admin"
        g_MQTT_password = "123456"

    except configparser.NoSectionError:
        print("Error2")
        g_STATUS_period = 60  # 状态传输周期
        g_GATHER_period = 60  # 雷达信号采集周期
        g_GATHER_degree = 10  # 单次采集采集次数
        g_VIDEO_begintime = datetime.strptime("2024-12-12 00:00:00", "%Y-%m-%d %H:%M:%S")
        g_VIDEO_endtime = datetime.strptime("2024-12-12 23:59:59", "%Y-%m-%d %H:%M:%S")
        g_VIDEO_width = 1080
        g_VIDEO_height = 768
        g_VIDEO_timelen = 1000
        g_VIDEO_VCRtime = []
        g_ISENERGY = False  # 是否为节能模式
        g_MQTT_clientid = 10086
        g_MQTT_host = "bjfxt.mypattern.cn"
        g_MQTT_port = 1883
        g_MQTT_user = "admin"
        g_MQTT_password = "123456"


def SaveIni():
    global g_STATUS_period  # 状态传输周期
    global g_GATHER_period  # 雷达信号采集周期
    global g_GATHER_degree  # 单次采集采集次数
    global g_VIDEO_begintime
    global g_VIDEO_endtime
    global g_VIDEO_width
    global g_VIDEO_height
    global g_VIDEO_timelen
    global g_VIDEO_VCRtime
    global g_ISENERGY  # 是否为节能模式
    global g_MQTT_clientid
    global g_MQTT_host
    global g_MQTT_port
    global g_MQTT_user
    global g_MQTT_password

    global g_inipath

    config = configparser.ConfigParser()
    config.read(g_inipath + "config.ini")
    
    # try:
    #    config.add_section("Status")
    #    config.add_section("Video")
    #    config.add_section("Mqtt")
    # finally:
    config.set("Status", "StatusPeriod", str(g_STATUS_period))
    config.set("Status", "GatherPeriod", str(g_GATHER_period))
    config.set("Status", "GatherDegree", str(g_GATHER_degree))
    config.set("Video", "Begintime", g_VIDEO_begintime.strftime("%Y-%m-%d %H:%M:%S"))
    config.set("Video", "Endtime", g_VIDEO_endtime.strftime("%Y-%m-%d %H:%M:%S"))
    config.set("Video", "Width", str(g_VIDEO_width))
    config.set("Video", "Height", str(g_VIDEO_height))
    config.set("Video", "Timelen", str(g_VIDEO_timelen))
    config.set("Video", "VCRNumber", str(len(g_VIDEO_VCRtime)))

    i = 0
    for s in g_VIDEO_VCRtime:
        config.set("CVRTime", str(i), s)
        i += 1

    config.set("Mqtt", "Host", g_MQTT_host)
    config.set("Mqtt", "Port", str(g_MQTT_port))
    config.set("Mqtt", "Client_id", str(g_MQTT_clientid))
    config.set("Mqtt", "User", g_MQTT_user)
    config.set("Mqtt", "Password", g_MQTT_password)

    with open(g_inipath + "config.ini", 'w') as configfile:
        config.write(configfile)


def savestatus():
    global g_4G_IMEI
    global g_4G_SIM
    global g_4G_net
    global g_4G_ICCID
    global g_4G_CSQ
    global g_SWRT
    global g_RAINING
    global g_ISWARING
    global g_RUN_MQTT
    global g_RUN_SCHEDULE
    global g_statuspath


    config = configparser.ConfigParser()
    config.add_section("Status")
    config.set("Status", "4GSIM", str(g_4G_SIM))
    config.set("Status", "4GNET", str(g_4G_net))
    config.set("Status", "4GICCID", g_4G_ICCID)
    # config.set("Status", "4GIMEI", g_4G_IMEI)
    config.set("Status", "4GCSQ", str(g_4G_CSQ))

    i = 0
    for s in g_SWRT:
        config.set("Status", "SWRT" + str(i), str(s))
        i += 1

    config.set("Status", "RAINING", str(g_RAINING))
    config.set("Status", "ISWARING", str(g_ISWARING))
    config.set("Status", "RUNMQTT", str(g_RUN_MQTT))
    config.set("Status", "RUNSCHEDULE", str(g_RUN_SCHEDULE))

    with open(g_statuspath + datetime.now().strftime("%Y%m%d%H%M%S.txt"), 'w') as statusfile:
        config.write(statusfile)


if __name__ == "__main__":
    print("开始读取配置文件")
    ReadIni()
    print("读取配置文件结束")
    SaveIni()
    print("存储文件结束")