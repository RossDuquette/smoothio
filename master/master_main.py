######################
# Main functionality #
######################

import smbus
import module.module_def as mods

# I2C channel 1 is connected to the GPIO pins
channel = 1

# Register addresses (with "normal mode" power-down bits)
reg_write_dac = 0x40

# Initialize I2C (SMBus)
bus = smbus.SMBus(channel)

def print_lookup_table():
    print """
        Module Numbers
        --------------
        Blender:  1
        Carousel: 2
        Dispense: 3

        Selector Numbers
        --------------
        *NDR = No Data Required
        Blender:
            0: Blender One
            1: Blender Two
            2: Pivot/Elevator
        Carousel:
            0: Rotate CW
            1: Rotate CCW
        Dispense:
            0-4: Dispenses 1-5
            5: Cup Dispense

        Data Segments
        -------------
        Blender:
            1: Idle
            2: Run Blender Motor
        Carousel: 
            N: Number of cups to rotate
        Dispense:
            1: Idle
            2: Dispense
    """

def main():
    print_lookup_table()
    while True:
        # Receives the data from the User
        uin = input("Module number (1: Blender 2: Carousel 3: Dispense): ")
        if uin == 1:
            add = mods.Blender.BLENDER_ADD
        elif uin == 2:
            add = mods.Carousel.CAROUSEL_ADD
        elif uin == 3:
            add = mods.Dispense.DISPENSE_ADD
        else:
            print "Wrong input. Please try again"
            continue
        head = input("Selector number: ")
        data = input("Data segment: ")
        # Write out I2C command: address, reg_write_dac, msg[0], msg[1]
        print """
        ###########################
        # Sending command to node #
        ###########################
        """
        bus.write_i2c_block_data(add, head, [data])

if __name__ == '__main__':
    main()
