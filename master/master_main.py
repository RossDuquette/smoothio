from gpiozero import LED
from time import sleep
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--test', help='foo help')
    args = parser.parse_args()


def main():
    led = LED(19)
    while True:
        led.on()


if __name__ == '__main__':
    main()
