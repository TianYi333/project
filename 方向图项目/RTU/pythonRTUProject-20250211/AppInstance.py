import shutil
import curfiles
import os


# 程序初始入口 如果存在升级文件，则进行文件复制，否则启动正常程序
filelist = curfiles.findAllFiles("/mnt/sdcard/update/", ".py")

for filename in filelist:
    srcpath = "/mnt/sdcard/update/" + filename
    despath = "/root/" + filename
    shutil.move(srcpath, despath)
    os.remove(srcpath)

os.system("python3 /root/main.py")