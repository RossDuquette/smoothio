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
        3 : "HOME",
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
        self.block_size = 13
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
        self.curr_sense0 = 0
        self.curr_sense1 = 0

    # Read data from blend module
    def read_data(self, i2cbus):
        data = i2cbus.read_i2c_block_data(self.ADD, 0, self.block_size)
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
        self.curr_sense0 = data[11]
        self.curr_sense1 = data[12]
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
        print "Current Sense 0: {}".format(self.curr_sense0)
        print "Current Sense 1: {}".format(self.curr_sense1)


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

    # Main handle
    def handle(self, i2cbus):
        self.print_selectors()
        selector = input("Selector number: ")
        if selector == 0:
            self.read_data(i2cbus)
            return
        if selector != 255:
            self.print_actions(selector)
            action = input("Action: ")
        else:
            action = 0
        i2cbus.write_i2c_block_data(self.ADD, selector, [action])
        print "\nCommand sent\n\n"

    def send_command(self, i2cbus, selector, action):
        i2cbus.write_i2c_block_data(self.ADD, selector, [action])


########################
#       Carousel       #
########################
class Carousel:
    carousel_selectors = {
        0 : "Read State",
        1 : "CW",
        2 : "CCW",
        3 : "HOME",
        4 : "DISABLE STEPPER",
        255 : "RESET"
    }

    # Init variables
    def __init__(self):
        self.ADD = 0x05
        self.block_size = 8
        self.c_state = 0
        self.num_cups = 0
        self.cup_sense0 = 0
        self.cup_sense1 = 0
        self.cup_mass0 = 0
        self.cup_mass1 = 0
        self.carousel_pos = 0
        self.homed = 0

    # Read data from blend module
    def read_data(self, i2cbus):
        data = i2cbus.read_i2c_block_data(self.ADD, 0, self.block_size)
        self.c_state = data[0]
        self.num_cups = data[1]
        self.cup_sense0 = data[2]
        self.cup_sense1 = data[3]
        self.cup_mass0 = data[4]
        self.cup_mass1 = data[5]
        self.carousel_pos = data[6]
        print "Carousel State: {}".format(self.carousel_selectors[self.c_state])
        print "Slots to Rotate: {}".format(self.num_cups)
        print "Cup Sense 0: {}".format(self.cup_sense0)
        print "Cup Sense 1: {}".format(self.cup_sense1)
        print "Cup Mass 0: {}".format(self.cup_mass0)
        print "Cup Mass 1: {}".format(self.cup_mass1)
        print "Homing Sensor: {}".format(self.carousel_pos)
        print "Homed: {}".format(self.homed)

    # Selectors
    def print_selectors(self):
        print """
        Selectors
        --------------
        """
        for key,val in self.carousel_selectors.items():
            print "        {} : {}".format(key,val)

    # Main handle
    def handle(self, i2cbus):
        self.print_selectors()
        selector = input("Selector number: ")
        if selector == 0:
            self.read_data(i2cbus)
            return
        if selector == 1 or selector == 2:
            action = input("Number of slots to rotate: ")
        else:
            action = 0
        i2cbus.write_i2c_block_data(self.ADD, selector, [action])
        print "\nCommand sent\n\n"


#######################
#      Dispense       #
#######################
class Dispense:
    dispense_selectors = {
        0 : "Read State",
        1 : "Frozen 1",
        2 : "Frozen 2",
        3 : "Frozen 3",
        4 : "Liquid 1",
        5 : "Liquid 2",
        6 : "Liquid 3",
        7 : "Cup",
        255 : "Reset"
    }
    dispense_actions = {
        0 : "IDLE",
        1 : "DISPENSE"
    }
    # Init variables
    def __init__(self):
        self.ADD = 0x06
        self.block_size = 7
        self.F1 = 0
        self.F2 = 0
        self.F3 = 0
        self.L1 = 0
        self.L2 = 0
        self.L3 = 0
        self.cup = 0

    # Read state
    def read_data(self, i2cbus):
        data = i2cbus.read_i2c_block_data(self.ADD, 0, self.block_size)
        self.F1 = data[0]
        self.F2 = data[1]
        self.F3 = data[2]
        self.L1 = data[3]
        self.L2 = data[4]
        self.L3 = data[5]
        self.cup = data[6]
        print "Frozen 1: {}".format(self.dispense_actions[self.F1])
        print "Frozen 2: {}".format(self.dispense_actions[self.F2])
        print "Frozen 3: {}".format(self.dispense_actions[self.F3])
        print "Liquid 1: {}".format(self.dispense_actions[self.L1])
        print "Liquid 2: {}".format(self.dispense_actions[self.L2])
        print "Liquid 3: {}".format(self.dispense_actions[self.L3])
        print "Cup Dispense: {}".format(self.dispense_actions[self.cup])

    # Print selectors
    def print_selectors(self):
        print """
        Selectors
        --------------
        """
        for key,val in self.dispense_selectors.items():
            print "        {} : {}".format(key,val)

    # Print actions
    def print_actions(self):
        print """
        Actions
        --------------
        """
        for key,val in self.dispense_actions.items():
            print "        {} : {}".format(key,val)
    
    # Main handle
    def handle(self, i2cbus):
        self.print_selectors()
        selector = input("Selector number: ")
        if selector == 0:
            self.read_data(i2cbus)
            return
        if selector != 255:
            self.print_actions()
            action = input("Action: ")
        else:
            action = 0
        i2cbus.write_i2c_block_data(self.ADD, selector, [action])
        print "\nCommand sent\n\n"
