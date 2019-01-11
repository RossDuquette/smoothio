from gpiozero import LED
from time import sleep

def main():
    led = LED(19)
    while True:
        led.on()

if __name__ == '__main__':
    main()
