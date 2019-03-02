import sys
from time import sleep
import smbus
import module.module_def as mods

# I2C channel 1 is connected to the GPIO pins
channel = 1

# Initialize I2C (SMBus)
bus = smbus.SMBus(channel)

def main():
    blender = mods.Blender()
    blender.read_data(bus)
    try:
        for i in range(2):
            blender.send_command(bus,3,1) # Pivot CW
            sleep(1)
            blender.send_command(bus,3,0) # Pivot IDLE
            blender.read_data(bus)
            sleep(3)
            blender.send_command(bus,3,2) # Pivot CCW
            sleep(1)
            blender.send_command(bus,3,0) # Pivot IDLE
            blender.read_data(bus)
            sleep(3)
        blender.send_command(bus,3,0) # Pivot IDLE
    except KeyboardInterrupt:
        blender.send_command(bus,3,0) # Pivot IDLE
        sys.exit()


if __name__ == '__main__':
    main()