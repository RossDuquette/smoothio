from time import sleep
import smbus
import module.module_def as mods

# I2C channel 1 is connected to the GPIO pins
channel = 1

# Initialize I2C (SMBus)
bus = smbus.SMBus(channel)

def main():
    blender = mods.Blender()
    while 1:
        blender.send_command(bus,4,2) # Elev descend
        sleep(1)
        blender.send_command(bus,4,0) # Elev stop
        sleep(1)
        blender.send_command(bus,4,1) # Elev ascend
        sleep(1)
        blender.send_command(bus,4,0) # Elev stop
        sleep(1)


if __name__ == '__main__':
    main()