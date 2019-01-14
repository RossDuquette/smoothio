"""Eqivalent to Arduino 'Blink'
"""

from gpiozero import LED
from time import sleep
from flask import Flask

app = Flask(__name__)

@app.route('/')
def blink_red():
    led = LED(16)

    while True:
        led.on()
        sleep(1)
        led.off()
        sleep(1)

if __name__ == '__main__':

    app.run(host= '0.0.0.0')
    led = LED(19)

    while True:
        led.on()
        sleep(1)
        led.off()
        sleep(1)
