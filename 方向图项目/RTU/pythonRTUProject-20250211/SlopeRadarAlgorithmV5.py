#!/usr/bin/python
# -*- coding:utf-8 -*-
# @author  : 内蒙古方向图有限责任公司-研发部（杨景）
# @createTime    : 2024年12月4日
# @modifyTime    : 2024年1月8日
# @version : V5
# @file    : SlopeRadarAlgorithmV5.py
# @describe: 符合协议的输出128个点，去掉DBSCAN算法
# @license : Copyright © 2024 内蒙古方向图有限责任公司

import os
import json
from copy import deepcopy
import struct
from datetime import datetime



class DBSCAN:
    def __init__(self, eps, min_samples):
        self.eps = eps
        self.min_samples = min_samples
        self.labels_ = None

    def fit_predict(self, X):
        """
        Perform DBSCAN clustering from data or coordinates.

        Parameters
        ----------
        X : list of lists, shape (n_samples, n_features)
            Input data.

        Returns
        -------
        labels : list of ints
            Cluster labels for each point. Noisy samples are given the label -1.
        """
        n_samples = len(X)
        self.labels_ = [-1] * n_samples
        
        for i in range(n_samples):
            if self.labels_[i] != -1:
                continue
            
            neighbors = self._region_query(X[i], X)
            if len(neighbors) < self.min_samples:
                self.labels_[i] = -1
            else:
                cluster_id = max(self.labels_) + 1
                self._expand_cluster(i, neighbors, X, cluster_id)
        
        return self.labels_

    def _region_query(self, point, X):
        """
        Perform a region query and return the indices of points within distance eps of point.

        Parameters
        ----------
        point : list of floats, shape (n_features,)
            The reference point.
        X : list of lists, shape (n_samples, n_features)
            The dataset to query.

        Returns
        -------
        neighbors : list of ints
            Indices of the points within eps of point.
        """
        distances = [sum((x_i - point_i) ** 2 for x_i, point_i in zip(x, point)) ** 0.5 for x in X]
        return [i for i, distance in enumerate(distances) if distance <= self.eps]

    def _expand_cluster(self, seed_id, neighbors, X, cluster_id):
        """
        Expand a single cluster from a seed point.

        Parameters
        ----------
        seed_id : int
            The index of the seed point.
        neighbors : list of ints
            Indices of points within eps of the seed point.
        X : list of lists, shape (n_samples, n_features)
            The dataset.
        cluster_id : int
            The identifier of the new cluster.
        """
        stack = list(neighbors)  # Use a list for stack to allow pop and append
        while stack:
            i = stack.pop()
            if self.labels_[i] != -1:
                continue
            self.labels_[i] = cluster_id
            
            region_neighbors = self._region_query(X[i], X)
            if len(region_neighbors) >= self.min_samples:
                for j in region_neighbors:
                    if self.labels_[j] == -1:
                        stack.append(j)


class Algorithm:
    def __init__(self, inputfile=None):
        self.inputfile = inputfile
        self.allresult = []
        self.dataS = {'id': 0, 'goudao': 0, 'width': 0, 'height': 0, 'v': 0}
        self.allresult.append(self.dataS)
        self.Module_data_length = {}
        self.Length = {}
        self.cumulative_r_x = []
        self.cumulative_r_y = []
        self.cumulative_speed = []
        self.cumulative_angle = []
        self.cumulative_SNR = []
        self.cumulative_height = []
        self.combinedData = {}
        self.dataTables = dict()
        self.frame_no = {}
        self.first_cfar_point_number = {}
        self.Beam_number = {}
        self.Speed_ambiguity = {}
        self.Distance_Index = {}
        self.Doppler_index = {}
        self.r_x = {}
        self.r_y = {}
        self.speed = {}
        self.angle = {}
        self.SNR = {}
        self.amplitude = {}
        self.R_D = {}
        self.P_A = {}
        self.height = {}
        self.k = 1
        self.y = 1
        self.index = 0
        self.jindex = 0
        self.cluster = []

    def read_data(self):
        # 因测试没有二进制流文件，传入为16进制文件，再转二进制
        # 如果是二进制流文件，直接'rb' 不需要 bytes.fromhex
        with open(self.inputfile, 'r') as fileID:
            linecache = fileID.readlines()
            byte_data = bytes.fromhex(linecache[0])
        return byte_data

    def function_classification(self, data=None):
        if data is None:
            return {}
        
        step=1.5
        final=[]
        classified_data = {}
        min_y = min(data, key=lambda x: x[1])[1]
        max_y = max(data, key=lambda x: x[1])[1]

        segmented_data = []
        for i in range(1,129):
            if i*step<min_y:
                classified_data= {"duanmianID": i, "width": 0,"v": 0,"h": 0}
                final.append(classified_data)
            elif i*step>max_y:
                classified_data= {"duanmianID": i, "width": 0,"v": 0,"h": 0}
                final.append(classified_data)
            else:
                for row in data:
                    if (i+1)*step>row[1]>=i*step:
                        segmented_data.append(row)
                    
                max_len = len(segmented_data[0])
                col_sums = [0] * max_len

                for ros in segmented_data:
                    for col_index in range(len(ros)):
                        col_sums[col_index] += ros[col_index]
                l = len(segmented_data)
                col_sums = [round(i / l, 4) for i in col_sums]

                w = round(max(segmented_data, key=lambda x: x[0])[0] - min(segmented_data, key=lambda x: x[0])[0],1)

                classified_data= {"duanmianID": i, "width": w,"v": col_sums[2],"h": col_sums[4]}
                final.append(classified_data)
        return final



    def process_data(self, value_bytes,devide):
        # 从字节数据中提取两个字节（假设每个整数占用两个字节）
        # value_bytes = byte_data[(72 + i * 28):(74 + i * 28)]
        # 使用self.bytes2dec将字节数据转换为整数
        value = self.bytes2dec(value_bytes)
        # 模拟np.uint16的行为，取模以避免超出16位无符号整数的范围
        value_uint16 = value % (2**16)
        # 模拟np.int16的行为，检查并处理溢出
        if value_uint16 > 32767:  # 16位有符号整数的最大值
            value_int16 = value_uint16 - 65536  # 进行“包裹”处理，模拟溢出
        else:
            value_int16 = value_uint16
        # 将整数转换为浮点数
        value_float = float(value_int16)
        # 将浮点数除以10
        result = value_float / devide
        return result
    

    def bytes2dec(self, i):
        if isinstance(i, bytes):
            return int.from_bytes(i, byteorder="little")

    def bytetoinfo(self, byte_data, ifexport):
        '''
        main函数
        :input: 输入为byte_data
        :ifexport 是否输出json文件
        :return: json结果与bytes结果
        '''
        data_return = b''
        # 数据的点数 128
        # data_return += int(128).to_bytes(4,byteorder="little")
        # 距离分辨率 1.5
        # data_return += struct.pack('<f', 1.5)
        y = 1
        k = 1
        cluster = []
        while y <= k:
            if y % 11 == 0:
                self.cumulative_r_x = []
                self.cumulative_r_y = []
                self.cumulative_speed = []
                self.cumulative_angle = []
                self.cumulative_SNR = []
                self.cumulative_height = []
                self.frame_no = {}
                self.first_cfar_point_number = {}
                self.Beam_number = {}
                self.Speed_ambiguity = {}
                self.Distance_Index = {}
                self.Doppler_index = {}
                self.r_x = {}
                self.r_y = {}
                self.speed = {}
                self.angle = {}
                self.SNR = {}
                self.amplitude = {}
                self.R_D = {}
                self.P_A = {}
                self.height = {}
            
            # 4-1 3-1
            self.Length[y] = self.bytes2dec(byte_data[2:4])
            # print(f"{y}  {self.Length[y]}")
            ## transform
            # combinedData[y,6] = np.array([hex2dec(np.array([hexData[14-1],hexData[13-1],hexData[12-1],hexData[11-1]]))])
            ## cfar 信息模块
            # Cfar_frame_header[y] = np.array([hexData[61-1],hexData[62-1]])
            self.Module_data_length[y] = self.bytes2dec(byte_data[62:64])
            for i in range(int((self.Module_data_length[y] - 5) / 28)):
                self.r_x[i] = self.process_data(byte_data[(72+i*28):(74+i*28)],10)
                # self.r_x[i] = self.bytes2dec(byte_data[(72+i*28):(74+i*28)]) / 10
                self.r_y[i] = self.bytes2dec(byte_data[74+i*28:76+i*28]) / 10
                self.speed[i] = self.process_data(byte_data[76+i*28:78+i*28],100)
                # self.speed[i] = self.bytes2dec(byte_data[76+i*28:78+i*28]) / 100
                self.SNR[i] = self.bytes2dec(byte_data[80+i*28:82+i*28]) / 100
                self.height[i] = self.process_data(byte_data[90+i*28:92+i*28],10)
                # self.height[i] = self.bytes2dec(byte_data[90+i*28:92+i*28]) / 10
            self.cumulative_r_x.extend(list(self.r_x.values()))
            self.cumulative_r_y.extend(list(self.r_y.values()))
            self.cumulative_speed.extend(list(self.speed.values()))
            self.cumulative_SNR.extend(list(self.SNR.values()))
            self.cumulative_height.extend(list(self.height.values()))
            

            if y % 10 == 0:
                data_sum = [
                    [self.cumulative_r_x[i], self.cumulative_r_y[i], self.cumulative_speed[i], self.cumulative_SNR[i], self.cumulative_height[i]]
                    for i in range(len(self.cumulative_r_x))
                ]
                
                data_sum = [row for row in data_sum if -20 < row[0] < 20 and 0 < row[1] and 0.3< abs(row[2]) < 10 and 10 < row[3] < 30]
                data_sum.sort(key=lambda x: x[1])
                # classified_data= {"duanmianID": i, "width": w,"v": col_sums[2],"h": col_sums[4]}
                classified_data = self.function_classification(data_sum)
                for data in classified_data:
                    print(str(data["duanmianID"])+" "+str(data["width"])+" "+str(data["v"])+" "+str(data["h"]))
                cluster.append(classified_data)

                for data in classified_data:
                    data_return += struct.pack('<f', round(data["v"], 2))
                    data_return += struct.pack('<f', round(data["h"], 2))
                    data_return += int(data["width"]).to_bytes(2,byteorder="little")
                    

                if ifexport:
                    try:
                        self.export(cluster)
                    except Exception as e:
                        print(e)

                # 仅处理10帧数据，返回128个点
                return data_return
            
            byte_data = byte_data[self.Length[y]:]
            if byte_data:
                k += 1
            else:
                break
            y += 1


        # 截断的位置不确定，直接截断，会导致数据异常
        # if len(data_return) > 128 * 10:
        #     return data_return[:128 * 10]
        # else:
        #     return data_return + bytes(128 * 10 - len(data_return))

    # data转换为json
    def export(self, data):
        FILE = datetime.now().strftime('%Y%m%dT%H%M%S')
        os.makedirs("./FloodResult/", exist_ok=True)
        with open("./FloodResult/" + FILE + ".json", "w") as output:
            json.dump(data, output)


if __name__ == "__main__":
    inputfile = r"C:\Users\Administrator\spu_20241009_1124.log"
    algorithm = Algorithm(inputfile=inputfile)
    byte_data = algorithm.read_data()
    data = algorithm.bytetoinfo(byte_data,ifexport=False)