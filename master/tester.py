######################
# Main functionality #
######################

import smbus
import module.module_def as mods

# I2C channel 1 is connected to the GPIO pins
channel = 1

# Initialize I2C (SMBus)
bus = smbus.SMBus(channel)

def print_modules():
    print """
        Module Numbers
        --------------
        Smoothie: 0
        Blender:  1
        Carousel: 2
        Dispense: 3
    """

def main():
    blender = mods.Blender()
    carousel = mods.Carousel()
    dispense = mods.Dispense()

    while True:
        print_modules()
        # Receives the data from the User
        uin = input("Select Module: ")
        if uin == 1:
            blender.handle(bus)
        elif uin == 2:
            carousel.handle(bus)
        elif uin == 3:
            dispense.handle(bus)
        else:
            print "Wrong input. Please try again"
            continue

if __name__ == '__main__':
    main()
