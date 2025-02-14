import cv2

# 获得视频的格式
videoCapture = cv2.VideoCapture('D:/test/test10292034.mjpg')

# 获得码率及尺寸
fps = videoCapture.get(cv2.CAP_PROP_FPS)
size = (int(videoCapture.get(cv2.CAP_PROP_FRAME_WIDTH)),
        int(videoCapture.get(cv2.CAP_PROP_FRAME_HEIGHT)))

# 指定写视频的格式, I420-avi, MJPG-mp4
# *mp4v是mp4在openCV里的写法
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
videoWriter = cv2.VideoWriter('D:/test/oto_other.mp4', fourcc, fps, size)

# 读帧
success, frame = videoCapture.read()

while success:
    videoWriter.write(frame)  # 写视频帧
    success, frame = videoCapture.read()  # 获取下一帧

print("转码成功")