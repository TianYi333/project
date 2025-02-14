import time
import os
import re
import sys
from os import stat
# import numpy as np


# 查找指定文件夹下文件
"""
查找指定路径下，指定后缀名的文件
path：路径
type：后缀
"""
def findAllFiles(path, type):
    filelist = []
    for root, dirs, files in os.walk(path):  # './mnt/sdcard/video/'
        # 指定目录，返回一个三元组（起始路径，起始路径下的文件夹名，起始路径下的文件名）
        for file_name in files:
            if file_name.endswith(type):
                print(file_name)
                filelist.append(file_name)

    # print(filelist)
    return filelist


# findAllFiles("D:/Python_Samples/pythonProject1", ".m")


"""
# 根据当前年份筛选文件
# 当前运行py的名称
curPyName = sys.argv[0][sys.argv[0].rfind(os.sep) + 1:]
# 筛选的文件类型
curFileName = (curPyName.split('_')[1]).split('.')[0]
# 当前年份
curYear = time.localtime(time.time())
# 当前路径
directory = os.getcwd()
# 筛选文件正则
fileReg = r'.*.' + curFileName
# 记录前一个文件夹路径，用于划分文件夹
prevFilePath = ''
# 生成文件的名称
newFileName = str(curYear.tm_year) + '_new_' + curFileName + '_file.html'
# 打开文件，未找到该文件时则自动创建，编码格式为utf8
f = open(newFileName, 'w', encoding='utf-8')
f.write('<html> <head> <title>【' + str(
    curYear.tm_year) + '年】' + curFileName + '文件列表</title> </head> <style> table{ font-size: 14px; color: #333;} </style> <body>')
# 遍历当前路径下的文件夹，通过正则表达式筛选出文件，再根据时间筛选出结果
for parent, dirnames, filenames in os.walk(directory):
    for i in filenames:
        if re.match(fileReg, i):
            filePath = os.path.join(parent, i)
            statInfo = stat(filePath)
            curTime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(statInfo.st_mtime))
            if curTime > str(curYear.tm_year) + "-01-01 00:00:00":
                if parent != prevFilePath:
                    if prevFilePath != '':
                        f.write('</table><hr>')
                    f.write('<table><tr style="text-align: left;"><th colspan=2>当前文件路径：' + parent + '</th></tr>')
                    f.write('<tr style="text-align: left;"><th>时间</th><th>文件名</th></tr>')
                f.write('<tr><td style="width: 150px;">' + curTime + '</td> <td>' + i + '</td></tr>')
                prevFilePath = parent

f.write('</table></body></html>')
# 关闭文件
f.close()
"""