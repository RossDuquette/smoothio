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

def main():
    while True:
        # Receives the data from the User
        uin = input("Enter a module. (1: Blender 2: Carousel 3: Dispense)\n")
        if uin == 1:
            add = mods.Blender.BLENDER_ADD
        elif uin == 2:
            add = mods.Carousel.CAROUSEL_ADD
        elif uin == 3:
            add = mods.Dispense.DISPENSE_ADD
        else:
            print "Wrong input. Please try again"
            continue
        head = input("Enter a header (ie 1: dispense one, 2: dispense two, ..., 6: cup dispense\n")
        data = input("Enter data to be sent (ie 1: idle, 2: dispense fruit, 3: dispense cup\n")
        # Write out I2C command: address, reg_write_dac, msg[0], msg[1]
        print "Sending command to node..."
        bus.write_i2c_block_data(add, head, [data])

if __name__ == '__main__':
    main()