import time
import os
import CameraUsbContrl
import DOcontrol
import LEDcontrol
import UpdateDataMqttAndHttp
import globalinfo
import FourGcontrol
import schedule
import STJNetMain
from datetime import datetime
import ntpsettime
import ZHYX2 as zhyx
import SWRT as swrt
import WarningLight
import zipfile
import PostFile as pf
import subprocess
import threading
import multiprocessing


def setntptime():
    ntpsettime.sync_time_with_ntp()



# def printInfo(info):   
#    if globalinfo.isdebug is True:
#        print(info + datetime.utcnow().strftime('    %Y-%m-%d %H:%M:%S.%f')[:-3])


# 获取雨雪状态
def getzhyx():
    # 启动模块控制输出
    DOcontrol.DO2set(True)
    time.sleep(1) # 具体时间待测试
    print("获取雨雪状态")
    try:
        zhyx485 = zhyx.ZHYX(zhyx.serial485_2)
        nR = zhyx485.connect()
        if nR is True:
            # 获取当前雨雪状态
            globalinfo.g_RAINING = zhyx485.rainingorsnowing()
            zhyx485.disconnect()
    except e:
        UpdateDataMqttAndHttp.Mqtt_PostWarning_FFFC(1, "get 降雨传感器状态 failed")
        
    if globalinfo.g_ISENERGY is True:
        DOcontrol.DO2set(False)


# 获取太阳能控制器信息
def getswet():
    print("获取太阳能信息")
    try:
        swrt485 = swrt.SWRT(swrt.serial485_1)
        swrt485.connect()
        

        globalinfo.g_SWRT = swrt485.get_readinfo()
        
        swrt485.disconnect()

    except e:
        UpdateDataMqttAndHttp.Mqtt_PostWarning_FFFC(1, "get 太阳能控制器参数 failed")


def getonepic():
    
    if globalinfo.g_one_PhotoGraph is True and globalinfo.g_one_PicorVideoGraph == 1:
        print("拍照")
        globalinfo.g_one_PhotoGraph = False
        globalinfo.g_pic_lastFilePath = datetime.now().strftime("%Y%m%d%H%M%S") + ".jpg"
        nReturn = CameraUsbContrl.getimagetofile(globalinfo.g_one_width, globalinfo.g_one_height, globalinfo.g_picpath + globalinfo.g_pic_lastFilePath)
        if nReturn is True:
            print("拍照成功")
            UpdateDataMqttAndHttp.Mqtt_PostTakePicOrVideo_FFFB(globalinfo.g_pic_lastFilePath)
        else:
            UpdateDataMqttAndHttp.Mqtt_PostWarning_FFFC(1, "take pic failed")


def getonevideo():
    
    if globalinfo.g_one_PhotoGraph is True and globalinfo.g_one_PicorVideoGraph == 2:
        print("录像")
        globalinfo.g_one_PhotoGraph = False
        globalinfo.g_video_lastFilePath = datetime.now().strftime("%Y%m%d%H%M%S") + ".mjpg"
        nReturn = CameraUsbContrl.getvideotofile(globalinfo.g_one_width, globalinfo.g_one_height, globalinfo.g_videopath + globalinfo.g_video_lastFilePath, globalinfo.g_one_VIDEO_timelen)
        if nReturn is True:
            print("录像成功")
            UpdateDataMqttAndHttp.Mqtt_PostTakePicOrVideo_FFFB(globalinfo.g_video_lastFilePath)
        else:
            UpdateDataMqttAndHttp.Mqtt_PostWarning_FFFC(1, "take video failed")


def getschedulevideo():
    print("定时录像")
    globalinfo.g_video_lastFilePath = datetime.now().strftime("%Y%m%d%H%M%S") + ".mjpg"
    path = globalinfo.g_videopath + globalinfo.g_video_lastFilePath
    nReturn = CameraUsbContrl.getvideotofile(globalinfo.g_VIDEO_width, globalinfo.g_VIDEO_height,
                                             path, globalinfo.g_VIDEO_timelen)
    if nReturn is True:
        UpdateDataMqttAndHttp.Mqtt_PostTakePicOrVideo_FFFB(globalinfo.g_video_lastFilePath)
    else:
        UpdateDataMqttAndHttp.Mqtt_PostWarning_FFFC(1, "take schedule video failed")


def getSTJdata():
    # 启动模块控制输出
    if globalinfo.g_test_PostData is False:
        DOcontrol.DO1set(True)
        time.sleep(30)  # 具体时间待测试
        print("获取雷达数据")
        STJNetMain.tcp_recv = True
        STJNetMain.tcp_savedatatofile = True
        STJNetMain.tcp_datasavepath = globalinfo.g_datapath
        test1 = STJNetMain.STJNet()
        test1.connect()

        while STJNetMain.tcp_recv is True:
            time.sleep(1)

        # tcp_recv = False
        # time.sleep(1)
        test1.disconnect()

        if globalinfo.g_ISENERGY is True:
            DOcontrol.DO1set(False)


def setWarninglight():
    if globalinfo.g_ISWARING:
        # 启动模块控制输出
        DOcontrol.DO3set(True)
        time.sleep(1)  # 具体时间待测试
        
        try:
            com = WarningLight.WarningLight(WarningLight.serial485_3)
            bReturn = com.connect()

            if bReturn is True:
                com.loopplaybackfolderfile(b'\x01', b'\x01')
                com.disconnect()
        except e:
            globalinfo.g_ISWARING = False
            UpdateDataMqttAndHttp.Mqtt_PostWarning_FFFC(1, "take warning light failed")

    else:
        DOcontrol.DO3set(False)


def getcat1csq():
    globalinfo.g_4G_CSQ = FourGcontrol.get_csq()
    
    
def setdatatozipAndUpdate():
    if globalinfo.g_data_SendUp is True:
        globalinfo.g_data_SendUp = False
        filelistIn = []
        for root, dirs, files in os.walk(globalinfo.g_datapath):  # './mnt/sdcard/video/'
            # 指定目录，返回一个三元组（起始路径，起始路径下的文件夹名，起始路径下的文件名）
            print(files)
            if len(files) > 0:

                for file in files:
                    namelist = file.split('.')
                    if namelist[1] == 'zip':
                        os.remove(globalinfo.g_datapath + file)
                        continue
                        
                    namelist = file.split('-')
                    info = datetime.strptime(namelist[0], "%Y_%m_%d_%H_%M_%S")

                    t1 = info - globalinfo.g_data_begintime.timestamp()
                    t2 = globalinfo.g_data_endtime.timestamp() - info
                    if t2 <= 0 <= t1:
                        filelistIn.append(file)
        zipname = datetime.now().strftime("%Y_%m_%d_%H_%M_%S") + ".zip"
        zp = zipfile.ZipFile(globalinfo.g_datapath + zipname, "w", zipfile.ZIP_DEFLATED)

        for file in filelistIn:
            zp.write(globalinfo.g_datapath + file)
        zp.close()

        for file in filelistIn:
            os.remove(globalinfo.g_datapath + file)

        ret = pf.post_file(globalinfo.g_data_posthost, globalinfo.g_MQTT_clientid, globalinfo.g_datapath + zipname, globalinfo.g_data_taskID, True)
        globalinfo.g_data_taskID = None


def create_dir_not_exist(path):
    if not os.path.exists(path):
        os.mkdir(path)
        

if __name__ == '__main__':
    """
    # 初始化
    """
    # file = open("/mnt/sdcard/infotest.dat", "ab+")
    # str_hex = "设备启动，进入程序\n"
    # file.write(str_hex.encode())
    # file.close()
    
    create_dir_not_exist(globalinfo.g_logpath)
    create_dir_not_exist(globalinfo.g_videopath)
    create_dir_not_exist(globalinfo.g_picpath)
    create_dir_not_exist(globalinfo.g_datapath)
    create_dir_not_exist(globalinfo.g_updatepath)
    create_dir_not_exist(globalinfo.g_statuspath)
    create_dir_not_exist(globalinfo.g_inipath)
    create_dir_not_exist(globalinfo.g_testpath)

    os.system("ifconfig eth1 up")
    os.system("ifconfig eth2 up")
    os.system("ifconfig eth1 192.168.137.10")
    os.system("ifconfig eth2 192.168.1.10")
    
    
    # 启动4G网络
    FourGcontrol.cat1_mount()
    
    try:
        # 启动cat命令并获取进程对象
        process = subprocess.Popen(['cat', '/dev/ttyUSB2'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                   bufsize=1, universal_newlines=True)
        # 创建管道
        parent_conn, child_conn = multiprocessing.Pipe()

        # 创建读取数据的线程
        read_thread = threading.Thread(target=FourGcontrol.read_data, args=(child_conn, process))

        # 创建处理数据的线程
        process_thread = threading.Thread(target=FourGcontrol.process_data, args=(parent_conn,))

        # 启动线程
        read_thread.start()
        process_thread.start()

        # 拨号
        while True:
            # 拨号
            if FourGcontrol.cat1_dialing() is True:
                break
            else:
                FourGcontrol.cat1_umount()
                FourGcontrol.ca1_remount()
                FourGcontrol.cat1_mount()

        # 等待读取线程结束
        read_thread.join()

        # 关闭管道的发送端
        child_conn.close()

        # 等待处理线程结束
        process_thread.join()
    except Exception as e:
        print(f"{e}")
    
    LEDcontrol.LED2set(True)

    
    # 获取IMEI号
    globalinfo.g_4G_IMEI = FourGcontrol.get_imei()
    
    time.sleep(1)

    # 获取信号值
    globalinfo.g_4G_CSQ = FourGcontrol.get_csq()
    if globalinfo.g_4G_CSQ is None:
        globalinfo.g_4G_CSQ = b'\xFF'
    time.sleep(1)

    LEDcontrol.LED3set(True)

    # 获取ICCID
    globalinfo.g_4G_ICCID = FourGcontrol.get_ICCID()

    if globalinfo.g_4G_ICCID is None:
        globalinfo.g_4G_ICCID = b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'

    time.sleep(5)

    LEDcontrol.LED4set(True)

    
    # NTP时间同步
    while ntpsettime.sync_time_with_ntp() is False:
        time.sleep(1)
    
    
    CameraUsbContrl.Init()
    
    LEDcontrol.LED5set(True)
    
    while True:
        if globalinfo.g_test_PostData is True:
            break

        if globalinfo.g_RESTARTMQTT is True:
            globalinfo.g_RUN_MQTT = False
            globalinfo.g_RESTARTMQTT = False
            UpdateDataMqttAndHttp.QuitMqtt()
            print("重启MQTT")

        if globalinfo.g_RUN_MQTT is False:
            globalinfo.g_RUN_MQTT = True
            UpdateDataMqttAndHttp.connetMqtt()
            print("启动MQTT")


        if globalinfo.g_RESTART_SCHEDULE is True:
            globalinfo.g_RUN_SCHEDULE = False
            globalinfo.g_VIDEO_beginVideo = False
            globalinfo.g_RESTART_SCHEDULE = False
            schedule.clear()
            print("重启定时任务")

        if globalinfo.g_RUN_SCHEDULE is False:
            globalinfo.g_RUN_SCHEDULE = True
            schedule.every(1).minutes.do(getswet)  # 获取太阳能控制器状态任务
            schedule.every(1).minutes.do(getzhyx)  # 获取雨雪状态任务
            schedule.every(globalinfo.g_STATUS_period).seconds.do(UpdateDataMqttAndHttp.Mqtt_post_FFFF)
            schedule.every(10).seconds.do(getonepic) # 单次拍照任务
            schedule.every(10).seconds.do(getonevideo) # 单次录像任务
            schedule.every(10).seconds.do(setdatatozipAndUpdate) # 压缩数据上传任务
            schedule.every(1).minutes.do(getcat1csq) # 获取信号质量任务
            schedule.every(5).seconds.do(setWarninglight) # 设置报警灯状态任务
            schedule.every().day.at("00:00").do(setntptime) # 自动校时任务
            schedule.every(1).minutes.do(globalinfo.savestatus) # 自动保存状态任务
            print("启动定时任务")

        if globalinfo.g_RUN_SCHEDULE is True and globalinfo.g_VIDEO_beginVideo is False:
            globalinfo.g_VIDEO_beginVideo = True
            nowtime = datetime.now().timestamp()
            t1 = nowtime - globalinfo.g_VIDEO_begintime.timestamp()
            t2 = globalinfo.g_VIDEO_endtime.timestamp() - nowtime
            if t2 < 0 <= t1:
                for ina in globalinfo.g_VIDEO_VCRtime:
                    schedule.every().at(ina).do(getSTJdata).until(globalinfo.g_VIDEO_endtime)

        schedule.run_pending()

