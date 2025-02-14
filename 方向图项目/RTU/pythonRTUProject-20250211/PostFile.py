import os
import requests
import json


# file_path = "D:/test/20241017144000.jpg"
# url = "http://222.74.26.58:9998/jeecg-boot/sys/common/cameraFileUpload"
# client_id = "00000000"


def post_file(host, clientid, filepath, taskID = None, isFailremove = False):
    if taskID is None:
        info = {'clientId': clientid}
    else:
        info = {'clientId': clientid, 'taskId': taskID}
    response = requests.post(host, files={'file': open(filepath, 'rb')}, data=info)
    dic_d = json.loads(response.text)

    if dic_d['message'] == "成功":
        os.remove(filepath)
        return True
    else: # 失败，各种原因
        print(dic_d['message'])
        if isFailremove:
            os.remove(filepath)
        return False

if __name__ == '__main__':
    file_path = "D:/test/20241017144000.jpg"
    url = "http://222.74.26.58:9998/jeecg-boot/sys/common/cameraFileUpload"
    client_id = "00000000"
    post_file(url, client_id, file_path)