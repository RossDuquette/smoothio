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
            add = blender.ADD
            blender.print_selectors()
            selector = input("Selector number: ")
            if selector == 0:
                blender.read_data(bus)
                continue
            else:
                blender.print_actions(selector)
        elif uin == 2:
            add = carousel.ADD
        elif uin == 3:
            add = dispense.ADD
        else:
            print "Wrong input. Please try again"
            continue

        if selector == 255:
            action = 0
        else:
            action = input("Action: ")

        print """
        ###########################
        # Sending command to node #
        ###########################
        """
        bus.write_i2c_block_data(add, selector, [action])

if __name__ == '__main__':
    main()
