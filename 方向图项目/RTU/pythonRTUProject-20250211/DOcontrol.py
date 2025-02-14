from periphery import GPIO


#板子有4路DO
DO1=15
DO2=16
DO3=17
DO4=18

gpio1 = GPIO("/dev/gpiochip3", DO1, "out")
gpio2 = GPIO("/dev/gpiochip3", DO2, "out")
gpio3 = GPIO("/dev/gpiochip3", DO3, "out")
gpio4 = GPIO("/dev/gpiochip3", DO4, "out")


def DO1set(bool):
    gpio1.write(bool)


def DO2set(bool):
    gpio2.write(bool)


def DO3set(bool):
    gpio3.write(bool)


def DO4set(bool):
    gpio4.write(bool)
    
    
if __name__ == "__main__":
    DO1set(True)
    DO2set(True)
    DO3set(True)
    DO4set(True)