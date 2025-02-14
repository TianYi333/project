import ntplib
import subprocess
import time


# 同步时间
def sync_time_with_ntp(ntp_server='pool.ntp.org'):
    """
    同步本地时间到指定的 NTP 服务器。

    :param ntp_server: NTP 服务器地址，默认为 'pool.ntp.org'
    """
    client = ntplib.NTPClient()
    try:
        response = client.request(ntp_server)
        timestamp = response.tx_time
        new_time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp))

        # 执行命令来设置系统时间
        subprocess.run(['date', '-s', new_time], check=True)

        print(f"Time synchronized with NTP server {ntp_server}: {new_time}")
        return True
    except Exception as e:
        print(f"Failed to synchronize time with NTP server {ntp_server}: {e}")
        return False
