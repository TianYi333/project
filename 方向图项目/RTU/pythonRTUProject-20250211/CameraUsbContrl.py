import subprocess

USBCameraInfo = None

def getimagetofile(width, height, filepath):
    global USBCameraInfo
    info = "--set-fmt-video=width=" + str(width) + ",height=" + str(height) + ",pixelformat=MJPG"
    path = "--stream-to=" + filepath

    if USBCameraInfo is None:
        USBCameraInfo = getUSBcameraparam()

    if USBCameraInfo is None:
        return False

    count = None
    for v in USBCameraInfo:
        if v[0] == width and v[1] == height:
            count = "--stream-count=1"
            break

    if count is None:
        return False

    try:
        result = subprocess.run(['v4l2-ctl', '--device', '/dev/video0', info, '--stream-mmap', path, count],
                                capture_output=True, text=True)

        # 检查命令是否成功执行
        if result.returncode == 0:
            return True
        else:
            return False
    except subprocess.CalledProcessError as e:
        print(e)
        return False


def getvideotofile(width, height, filepath, timelen=1000):
    global USBCameraInfo
    info = "--set-fmt-video=width=" + str(width) + ",height=" + str(height) + ",pixelformat=MJPG"
    path = "--stream-to=" + filepath

    if USBCameraInfo is None:
        USBCameraInfo = getUSBcameraparam()

    count = None
    for v in USBCameraInfo:
        if v[0] == width and v[1] == height:
            count = "--stream-count=" + str(int(timelen / 1000 * v[2]))
            break

    if count is None:
        return False

    try:
        result = subprocess.run(['v4l2-ctl', '--device', '/dev/video0', info, '--stream-mmap', path, count],
                                capture_output=True, text=True)

        # 检查命令是否成功执行
        if result.returncode == 0:
            return True
        else:
            return False
    except subprocess.CalledProcessError as e:
        print(e)
        return False


"""
# 反回二维列表，二级列表分别表示宽度，高度，帧率
"""
def getUSBcameraparam():
    result = subprocess.run(['v4l2-ctl', '--list-framesizes=YUYV'], capture_output=True, text=True)

    # 检查命令是否成功执行
    if result.returncode == 0:
        info = result.stdout.replace('\t', '')
        info = info.split('\n')

        value = []
        for s in info:
            data = s.split(' ')
            if (len(data) == 3):
                value.append(data[2])

        camerainfo = []

        for stc in value:
            scr = stc.split('x')
            info = "width=" + scr[0] + ",height=" + scr[1] + ",pixelformat=YUYV"
            result = subprocess.run(['v4l2-ctl', '--list-frameintervals', info], capture_output=True, text=True)

            # 检查命令是否成功执行
            if result.returncode == 0:
                cr = []
                widthandheight = stc.split('x')
                cr.append(int(widthandheight[0]))
                cr.append(int(widthandheight[1]))

                info = result.stdout.split('(')
                if len(info) == 2:
                    fps = info[1].split(' ')
                    if len(fps) > 1:
                        cr.append(int(float(fps[0])))
                        camerainfo.append(cr)

            else:
                return None

        return camerainfo

    else:
        return None


def Init():
    global USBCameraInfo
    # print("查看视频输入设备：")
    # 执行ls命令并捕获输出
    result = subprocess.run(['v4l2-ctl','-n'], capture_output=True, text=True)

    # 检查命令是否成功执行
    # if result.returncode == 0:
        # print(f"命令输出:\n{result.stdout}")
    # else:
        # print(f"命令执行失败，错误输出:\n{result.stderr}")

    # print("获取指定设备的所有信息：")
    result = subprocess.run(['v4l2-ctl','--all','--device','/dev/video0'], capture_output=True, text=True)

    # 检查命令是否成功执行
    # if result.returncode == 0:
        # print(f"命令输出:\n{result.stdout}")
    # else:
        # print(f"命令执行失败，错误输出:\n{result.stderr}")


    # print("查看支持的输出的格式：")
    result = subprocess.run(['v4l2-ctl','--list-formats'], capture_output=True, text=True)

    # 检查命令是否成功执行
    # if result.returncode == 0:
        # print(f"命令输出:\n{result.stdout}")
    # else:
        # print(f"命令执行失败，错误输出:\n{result.stderr}")

    # print("查看支持的分辨率：")
    result = subprocess.run(['v4l2-ctl', '--list-framesizes=YUYV'], capture_output=True, text=True)

    # 检查命令是否成功执行
    # if result.returncode == 0:
        # print(f"命令输出:\n{result.stdout}")
    # else:
        # print(f"命令执行失败，错误输出:\n{result.stderr}")

    info = result.stdout.replace('\t', '')
    info = info.split('\n')

    value = []
    for s in info:
        data = s.split(' ')
        # print(data)
        if (len(data) == 3):
            value.append(data[2])
    # print(value)

    # print("查看支持的帧率：")

    camerainfo = []

    for stc in value:
        scr = stc.split('x')
        info = "width=" + scr[0] + ",height=" + scr[1] + ",pixelformat=YUYV"
        result = subprocess.run(['v4l2-ctl', '--list-frameintervals', info], capture_output=True, text=True)

        cr = []
        cr.append(stc)

        info = result.stdout.split('(')
        # print("info:" + str(info))
        if len(info) == 2:
            fps = info[1].split(' ')
            if len(fps) > 1:
                cr.append(int(float(fps[0])))

                camerainfo.append(cr)
                # print(cr)
        # 检查命令是否成功执行
        # if result.returncode == 0:
            # print(stc + f"命令输出:\n{result.stdout}")
        # else:
            # print(f"命令执行失败，错误输出:\n{result.stderr}")

    # print(camerainfo)

    result = subprocess.run(['v4l2-ctl','--list-frameintervals','width=1920,height=1080,pixelformat=YUYV'], capture_output=True, text=True)

    # 检查命令是否成功执行
    # if result.returncode == 0:
        # print(f"命令输出:\n{result.stdout}")
    # else:
        # print(f"命令执行失败，错误输出:\n{result.stderr}")


    # print("设置index为0的设备作为视频输入设备：")
    result = subprocess.run(['v4l2-ctl','-i','0'], capture_output=True, text=True)

    # 检查命令是否成功执行
    # if result.returncode == 0:
        # print(f"命令输出:\n{result.stdout}")
    # else:
        # print(f"命令执行失败，错误输出:\n{result.stderr}")

    USBCameraInfo = getUSBcameraparam()

"""
getimagetofile(1920, 1080, '/mnt/sdcard/20241030204700.jpg')
getvideotofile(1920, 1080, '/mnt/sdcard/20241030205100.mjpg', timelen=5000)
"""

if __name__ == "__main__":
    Init()
    getvideotofile(1920, 1080, '/mnt/sdcard/20250116000005.mjpg', timelen=25000)
    # getimagetofile(1920, 1080, '/mnt/sdcard/20241213 test.jpg')


