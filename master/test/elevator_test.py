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
    sleep_time = 0.5
    while 1:
        try:
            blender.send_command(bus,4,1) # Elev ascend
            blender.read_data(bus)
            sleep(sleep_time)
            blender.send_command(bus,4,0) # Elev stop
            blender.read_data(bus)
            sleep(sleep_time)
            blender.send_command(bus,4,2) # Elev descend
            blender.read_data(bus)
            sleep(sleep_time)
            blender.send_command(bus,4,0) # Elev stop
            blender.read_data(bus)
            sleep(sleep_time)
        except KeyboardInterrupt:
            blender.send_command(bus,4,0) # Elev stop
            sys.exit()

if __name__ == '__main__':
    main()
