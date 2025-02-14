import subprocess
import time
from core.logger_utils import logger


def monitor_and_restart(python_program_path, args=[]):
    while True:
        process = subprocess.Popen(['python3', python_program_path] + args)
        process.wait()  # 等待程序结束

        if process.returncode != 0:  # 如果程序非正常退出

            logger.error(f"{python_program_path} 已退出，正在重新启动...")

        else:
            logger.info(f"{python_program_path} 正常结束")
            break  # 如果程序正常结束则退出监控循环

        time.sleep(5)  # 等待 5 秒后重新启动


# 指定要监控的 Python 程序路径和可能的参数
python_program_to_monitor = '/usr/main.py'
monitor_and_restart(python_program_to_monitor)