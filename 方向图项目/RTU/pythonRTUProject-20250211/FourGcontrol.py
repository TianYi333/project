import os, time
import serial
import globalinfo


#板子有4个485和2个232接口，定义如下
serial485_1 = '/dev/tty485_1'
serial485_2 = '/dev/tty485_2'
serial485_3 = '/dev/tty485_3'
serial485_4 = '/dev/tty485_4'
serial232_1 = '/dev/tty232_1'
serial232_2 = '/dev/tty232_2'
serialUSB_1 = '/dev/ttyUSB1'
serialUSB_2 = '/dev/ttyUSB2'
baudrate = 115200

# 重新插拔卡后需要复位(重新挂载)一下USB网卡，否则会检卡失败及驻网失败

import os, time
import subprocess
import multiprocessing

import subprocess
import threading

# SIM卡状态
g_sim_sta = False
# 驻网状态
g_net_sta = False


def read_data(conn, process):
    global g_net_sta
    while g_net_sta is False:
        # output = process.stdout.readline().decode('utf-8').strip()
        data = process.stdout.readline()
        if data == '' and process.poll() is not None:
            break
        if data:
            conn.send(data)


def process_data(conn):
    global g_sim_sta, g_net_sta
    while True:
        try:
            data = conn.recv()
            # print(f"处理数据: {data}")
            if '+CPIN: READY' in data:
                print('SIM card detected.')
                g_sim_sta = True
            if '+COPS:' in data:
                fields = data.split(',')
                if (len(fields) >= 3):
                    print('network register success.')
                    g_net_sta = True
                    break
            if '+CGSN:' in data:
                s = data.split(':')
                print('IMEI号：', s[1].strip())
            if '+MCCID:' in data:
                s = data.split(':')
                print('ICCID号：', s[1].strip())
            if '+CSQ:' in data:
                s = data.split(":")[1].split(",")[0].strip()
                print('信号值：', s)

        except EOFError:
            print("管道已关闭，处理线程退出")
            break

        # 挂载


def cat1_mount():
    # cat1上电，rst引脚低有效
    os.system('echo 0 > /sys/class/gpio/gpio11/value')
    time.sleep(3)


# 拨号
def cat1_dialing():
    # 关闭回显，避免AT口大量换行
    os.system('echo -e \'ATE0\r\n\' > /dev/ttyUSB2')

    last_print_time = time.time()
    i = 10
    while i > 0:
        # 检查SIM卡状态
        os.system('echo -e \'AT+CPIN?\r\n\' > /dev/ttyUSB2')

        if g_sim_sta == False:
            current_time = time.time()
            # 每隔10s打印一次
            if current_time - last_print_time >= 10:
                print('No SIM card detected....')
                last_print_time = current_time
            time.sleep(1)
        else:
            break

        i -= 1

    if i <= 1:
        return False

    for i in range(20):
        # 检查驻网状态，超时20s
        os.system('echo -e \'AT+COPS?\r\n\' > /dev/ttyUSB2')
        if g_net_sta == False:
            time.sleep(1)
            if i > 16:
                raise SystemExit('network register fail...')
                return False
        else:
            break

    # 查询PDP上下文配置
    os.system('echo -e \'AT+CGDCONT?\r\n\' > /dev/ttyUSB2')

    # 查询拨号设置
    os.system('echo -e \'AT+MDIALUP?\r\n\' > /dev/ttyUSB2')

    # 开启拨号
    os.system('echo -e \'AT+MDIALUP=1,1\r\n\' > /dev/ttyUSB2')

    os.system('udhcpc -i eth0')
    time.sleep(3)

    return True


# 取消挂载
def cat1_umount():
    os.system('ifconfig eth0 down')
    os.system('kill -9 $(ps | grep "cat /dev/ttyUSB2" | grep -v grep | awk \'{print $1}\')')
    os.system('echo 1 > /sys/class/gpio/gpio11/value')
    time.sleep(0.3)


# 重新挂载（复位）
def ca1_remount():
    os.system('echo 1 > /sys/class/gpio/gpio11/value')
    time.sleep(0.3)
    os.system('echo 0 > /sys/class/gpio/gpio11/value')
    time.sleep(0.3)


def get_imei():
    global serialUSB_2,baudrate
    serialUSB = serial.Serial(port=serialUSB_2, baudrate=baudrate, timeout=0.1)

    print("4G获取IMEI值")

    os.system('echo -e \'AT+CGSN=1\r\n\' > /dev/ttyUSB2')
    read_data = serialUSB.read(100)
    serialUSB.close()
    read_data = read_data.decode("UTF-8")
    n = read_data.find('OK')

    if n > 0:  # 是正常OK的回复
        read_data = read_data.replace("OK", '')
        read_data = read_data.replace('\r\n', '')
        read_data = read_data.replace(' ', '')
        infolist = read_data.split(':')
        value = infolist[1]
        print("imei:" + str(value))
        return value

    return None


def get_ICCID():
    global serialUSB_2,baudrate
    serialUSB = serial.Serial(port=serialUSB_2, baudrate=baudrate, timeout=0.1)

    print("4G获取ICCID")

    os.system('echo -e \'AT+MCCID\r\n\' > /dev/ttyUSB2')
    read_data = serialUSB.read(100)
    serialUSB.close()
    read_data = read_data.decode("UTF-8")
    # info = b'\r\n+ICCID: 89860619140024127464\r\n\r\nOK\r\n'
    n = read_data.find('OK')

    if n > 0:  # 是正常OK的回复
        read_data = read_data.replace("OK", '')
        read_data = read_data.replace('\r\n', '')
        read_data = read_data.replace(' ', '')
        infolist = read_data.split(':')
        value = infolist[1]
        print("iccid:" + str(value))
        return value

    return None


def get_csq():  # 信号强度
    global serialUSB_2,baudrate
    serialUSB = serial.Serial(port=serialUSB_2, baudrate=baudrate, timeout=0.1)

    print("4G获取信号强度")

    os.system('echo -e \'AT+CSQ\r\n\' > /dev/ttyUSB2')
    read_data = serialUSB.read(100)
    serialUSB.close()
    read_data = read_data.decode("UTF-8")
    print("信号强度：" + str(read_data))
    # data = b'\r\n+CSQ: 30,99\r\n\r\nOK\r\n'
    n = read_data.find('OK')

    if n > 0:  # 是正常OK的回复
        read_data = read_data.replace("OK", '')
        read_data = read_data.replace('\r\n', '')
        read_data = read_data.replace(' ', '')
        infolist = read_data.split(':')
        value = infolist[1].split(',')
        value1 = int(value[0])
        value2 = int(value[1])
        return value1

    return None


if __name__ == "__main__":
    # 挂载cat1
    cat1_mount()
    try:
        # 启动cat命令并获取进程对象
        process = subprocess.Popen(['cat', '/dev/ttyUSB2'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                   bufsize=1, universal_newlines=True)
        # 创建管道
        parent_conn, child_conn = multiprocessing.Pipe()

        # 创建读取数据的线程
        read_thread = threading.Thread(target=read_data, args=(child_conn, process))

        # 创建处理数据的线程
        process_thread = threading.Thread(target=process_data, args=(parent_conn,))

        # 启动线程
        read_thread.start()
        process_thread.start()

        # 拨号
        cat1_dialing()

        # 获取IMEI号
        get_imei()
        time.sleep(1)

        # 获取信号值
        get_csq()
        time.sleep(1)

        # 获取ICCID
        get_iccid()
        time.sleep(1)

        # 等待读取线程结束
        read_thread.join()

        # 关闭管道的发送端
        child_conn.close()

        # 等待处理线程结束
        process_thread.join()
    except Exception as e:
        print(f"{e}")
