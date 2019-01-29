"""Blender Driver
"""
import smbus
import time


class Commands:
    RUN_BLENDER_MOTOR = 1
    MOVE_ELEVATOR = 2
    ROTATE_PIVOT = 3


class Blender(object):
    def __init__(self):
        self.add = 0x01

    def run_blender_motor(self, speed):
        return
