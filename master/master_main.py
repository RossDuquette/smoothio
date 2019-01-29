# from gpiozero import LED
import argparse
import subsys_test as tst
from time import sleep
from enum import Enum


class Subsystems:
    BLENDER_MOTOR = 'blender_motor'
    BLENDER_ELEVATOR = 'blender_elevator'
    BLENDER_PIVOT = 'blender_pivot'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-t', help='name of the mudule to be tested', default=None)
    parser.add_argument(
        '-f', help='name of the function to be tested', default=None)
    args = parser.parse_args()
    run(args)


def test(ss, function):
    test_selector = "{}_{}".format(ss, function)
    print test_selector, Subsystems.BLENDER_MOTOR
    if test_selector == Subsystems.BLENDER_MOTOR:
        tst.blender_motor()
    if test_selector == Subsystems.BLENDER_ELEVATOR:
        tst.blender_elevator()
    if test_selector == Subsystems.BLENDER_PIVOT:
        tst.blender_pivot()


def run(args):
    print args
    if args.t != None:
        test(args.t, args.f)
    # led = LED(19)
    # while True:
    #     led.on()


if __name__ == '__main__':
    main()
