import periphery  # python_periphery


LED2='led2'
LED3='led3'
LED4='led4'
LED5='led5'

led2=periphery.LED(LED2, 0)
led3=periphery.LED(LED3, 0)
led4=periphery.LED(LED4, 0)
led5=periphery.LED(LED5, 0)


def LED2set(bool):
    led2.write(bool)


def LED3set(bool):
    led3.write(bool)


def LED4set(bool):
    led4.write(bool)


def LED5set(bool):
    led5.write(bool)