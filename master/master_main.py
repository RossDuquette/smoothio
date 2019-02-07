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
    Blender = mods.Blender()
    Carousel = mods.Carousel()
    Dispense = mods.Dispense()
    while True:
        print_modules()
        # Receives the data from the User
        uin = input("Select Module: ")
        if uin == 1:
            add = Blender.BLENDER_ADD
            Blender.print_selectors()
            selector = input("Selector number: ")
            if selector == 0:
                Blender.read_data(bus)
            else:
                Blender.print_actions(selector)
        elif uin == 2:
            add = mods.Carousel.CAROUSEL_ADD
        elif uin == 3:
            add = mods.Dispense.DISPENSE_ADD
        else:
            print "Wrong input. Please try again"
            continue

        action = input("Action: ")
        # Write out I2C command: address, msg[0], msg[1]
        print """
        ###########################
        # Sending command to node #
        ###########################
        """
        bus.write_i2c_block_data(add, selector, [action])

if __name__ == '__main__':
    main()
