#################################################
# Class definition and signal codes for modules #
#################################################

class Blender:
    blender_states = {
        0 : "IDLE",
        1 : "ON"
    }
    elevator_states = {
        0 : "IDLE",
        1 : "ASCEND",
        2 : "DESCEND",
        3 : "HOME"
    }
    pivot_states = {
        0 : "IDLE",
        1 : "CW",
        2 : "CCW",
        3 : "HOME"
        4 : "ROTATE_180"
    }
    routine_states = {
        0 : "IDLE",
        1 : "BLEND AND CLEAN",
        2 : "CLEAN",
        3 : "BLEND"
    }

    # Init variables
    def __init__(self):
        self.ADD = 0x04
        self.blender0 = 0
        self.blender1 = 0
        self.elevator = 0
        self.pivot = 0
        self.routine = 0
        self.p_homed = 0
        self.e_homed = 0
        self.pivot_deg = 0
        self.elevator_height = 0
        self.limit1 = 0
        self.limit2 = 0

    # Read data from blend module
    def read_data(self, i2cbus):
        data = i2cbus.read_i2c_block_data(self.ADD, 0, 11)
        #print ''.join(map(chr, data)) # Print raw incomming data
        self.blender0 = data[0]
        self.blender1 = data[1]
        self.elevator = data[2]
        self.pivot = data[3]
        self.routine = data[4]
        self.p_homed = data[5]
        self.e_homed = data[6]
        self.pivot_deg = data[7]
        self.elevator_height = data[8]
        self.limit1 = data[9]
        self.limit2 = data[10]
        print "Blender 0: {}".format(self.blender_states[self.blender0])
        print "Blender 1: {}".format(self.blender_states[self.blender1])
        print "Elevator: {}".format(self.elevator_states[self.elevator])
        print "Pivot: {}".format(self.pivot_states[self.pivot])
        print "Routine: {}".format(self.routine_states[self.routine])
        print "Pivot Homed: {}".format(self.p_homed)
        print "Elevator Homed: {}".format(self.e_homed)
        print "Pivot Angle: {} degrees".format(self.pivot_deg)
        print "Elevator Height: {}cm".format(self.elevator_height)
        print "Limit Switch 1: {}".format(self.limit1)
        print "Limit Switch 2: {}".format(self.limit2)


    # Selectors
    def print_selectors(self):
        print """
        Selector Numbers
        --------------
        0: Read state
        1: Blender 0
        2: Blender 1
        3: Pivot
        4: Elevator
        5: Routine
        255: Reset
        """

    # Actions
    def print_actions(self, selector):
        print """
        Actions
        --------------
        """
        if selector == 1 or selector == 2:
            for key,val in self.blender_states.items():
                print "        {} : {}".format(key,val)
        elif selector == 3:
            for key,val in self.pivot_states.items():
                print "        {} : {}".format(key,val)
        elif selector == 4:
            for key,val in self.elevator_states.items():
                print "        {} : {}".format(key,val)
        elif selector == 5:
            for key,val in self.routine_states.items():
                print "        {} : {}".format(key,val)
        print ""

class Carousel:
    def __init__(self):
        self.ADD = 0x05

class Dispense:
    def __init__(self):
        self.ADD = 0x06
