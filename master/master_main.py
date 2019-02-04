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
        mod = input("Enter a module. (1: Blender 2: Carousel 3: Dispense)\n")
        if mod == 1:
            add = mods.Blender.BLENDER_ADD
        elif mod == 2:
            add = mods.Carousel.CAROUSEL_ADD
        elif mod == 3:
            add = mods.Dispense.DISPENSE_ADD
        else:
            print "Wrong input. Please try again"
            continue
        command = input("Enter the command (ie Blender, 1: move elevator +ve 2: rotate pivot +ve)\n")

        # Write out I2C command: address, reg_write_dac, msg[0], msg[1]
        print "Sending command to node..."
        bus.write_i2c_block_data(0x04, mod, [command])

if __name__ == '__main__':
    main()