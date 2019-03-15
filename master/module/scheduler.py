###########################
# Definition of Scheduler #
###########################
import module.module_def as mods
import time
import smbus

class BlenderStates:
    IDLE = 0
    SEAL_BLENDER = 1
    STARTING_BLENDER = 2
    DESCENDING_BLENDER = 3
    RECOILING_BLENDER = 4
    STOPPING_BLENDER = 5
    HOMING_BLENDER = 6

class Scheduler:
    FROZEN_DISPENSE_TIME = 7
    LIQUID_DISPENSE_TIME = 7
    CAROUSEL_SPIN_TIME = 2
    BLEND_TIME = 20
    CUP_DISPENSE_TIME = 2
    BLENDER_RECOIL_TIME = 0.5
    BLENDER_START_TIME = 1
    BLENDER_DESCEND_TIME = 1

    def __init__(self):
        self.clock = 0
        self.blender = mods.Blender()
        self.carousel = mods.Carousel()
        self.dispense = mods.Dispense()
        self.bus = smbus.SMBus(1)

        self.cup_posns = []
        self.cup_states = [True for _ in range(self.carousel.num_slots)]
        self.all_stations_go = True

        self.cup_time = time.time()
        self.frozen_time = time.time()
        self.liquid_time = time.time()
        self.spin_time = time.time()
        self.blend_time = time.time()
        self.blend_recoil_time = time.time()
        self.blend_cycles = 0
        self.blender_state = BlenderStates.IDLE
        self.blend_state_timer = time.time()
        self.pivot_location = 0

    def home_everything(self):
        # Home elevator
        self.blender.send_command(self.bus, 4, 3)
        while not self.elevator_idle():
            time.sleep(0.1)

        # Disable carousel
        self.carousel.send_command(self.bus, 4, 0)

        # Home pivot
        self.blender.send_command(self.bus, 3, 1)
        while not self.pivot_idle():
            time.sleep(0.1)
        self.blender.send_command(self.bus, 3, 3)
        while not self.pivot_idle():
            time.sleep(0.1)

    def empty(self):
        """Check if the machine is empty"""
        if len(self.cup_posns) == 0:
            return True
        return False

    def check_all_stations_go(self):
        return self.all_stations_go

    def add_cup(self, posn):
        """Add cup to queue and start action"""
        # if posn not in self.cup_posns:
        #     self.cup_posns.append(posn)

        self.all_stations_go = False
        if posn == 0:
            # Send cup dispense command
            self.dispense.send_command(self.bus, 7, 1)
            self.cup_time = time.time() + self.CUP_DISPENSE_TIME
            self.cup_states[0] = False
        elif posn == 1:
            # Send frozen dispense commands
            self.dispense.send_command(self.bus, 1, 1)
            self.dispense.send_command(self.bus, 2, 1)
            self.dispense.send_command(self.bus, 3, 1)
            self.frozen_time = time.time() + self.FROZEN_DISPENSE_TIME
            self.cup_states[1] = False
        elif posn == 2:
            # Send liquid dispense commands
            self.dispense.send_command(self.bus, 4, 1)
            self.dispense.send_command(self.bus, 5, 1)
            self.dispense.send_command(self.bus, 6, 1)
            self.liquid_time = time.time() + self.LIQUID_DISPENSE_TIME
            self.cup_states[2] = False
        elif posn == 3:
            # Send blender commands
            self.cup_states[3] = False
            self.blend_time = time.time() + self.BLEND_TIME
            self.blender.send_command(self.bus, 4, 4)
            self.blender_state = BlenderStates.SEAL_BLENDER
            self.carousel.send_command(self.bus, 4, 0)
        elif posn == 4:
            # Wait for cup to be taken
            self.cup_states[4] = False

    def shift_cups(self):
        """Shift all cups by one slot"""
        for i, cp in enumerate(self.cup_posns):
            self.cup_posns[i] += 1
            self.add_cup(self.cup_posns[i])

    def update(self):
        """Non-blocking update routine"""

        # Check if there is anything to do
        if self.all_stations_go or not self.check_carousel_idle():
            return
        # Check cup dispense
        if not self.cup_states[0] and time.time() >= self.cup_time:
            self.dispense.send_command(self.bus, 7, 0) 
            self.cup_states[0] = True
        # Check frozen dispense
        if not self.cup_states[1] and time.time() >= self.frozen_time:
            self.dispense.send_command(self.bus, 1, 0)
            self.dispense.send_command(self.bus, 2, 0)
            self.dispense.send_command(self.bus, 3, 0) 
            self.cup_states[1] = True
        # Check liquid dispense
        if not self.cup_states[2] and time.time() >= self.liquid_time:
            self.dispense.send_command(self.bus, 4, 0)
            self.dispense.send_command(self.bus, 5, 0)
            self.dispense.send_command(self.bus, 6, 0) 
            self.cup_states[2] = True
        # Check blender
        if not self.cup_states[3]:
            if time.time() >= self.blend_time and self.blender_state != BlenderStates.HOMING_BLENDER:
                self.blender.send_command(self.bus, 1, 0)
                time.sleep(0.5)
                self.blender.send_command(self.bus, 4, 3)
                self.blender_state = BlenderStates.HOMING_BLENDER
            
            # State Machine
            if self.blender_state == BlenderStates.IDLE:
                pass
            elif self.blender_state == BlenderStates.SEAL_BLENDER:
                if self.elevator_idle() == True:
                    self.blender_state = BlenderStates.STARTING_BLENDER
                    self.blender.send_command(self.bus, 1, 1)
                    self.blend_state_timer = time.time() + self.BLENDER_START_TIME
            elif self.blender_state == BlenderStates.STARTING_BLENDER:
                if time.time() >= self.blend_state_timer:
                    self.blender_state = BlenderStates.DESCENDING_BLENDER
                    self.blender.send_command(self.bus, 4, 2)
                    self.blend_state_timer = time.time() + self.BLENDER_DESCEND_TIME
            elif self.blender_state == BlenderStates.DESCENDING_BLENDER:
                if time.time() >= self.blend_state_timer or self.elevator_idle():
                    self.blender_state = BlenderStates.RECOILING_BLENDER
                    self.blender.send_command(self.bus, 4, 0)
                    self.blend_state_timer = time.time() + self.BLENDER_RECOIL_TIME
            elif self.blender_state == BlenderStates.RECOILING_BLENDER:
                if time.time() >= self.blend_state_timer:
                    self.blender_state = BlenderStates.DESCENDING_BLENDER
                    self.blender.send_command(self.bus, 4, 2)
                    self.blend_state_timer = time.time() + self.BLENDER_DESCEND_TIME
            elif self.blender_state == BlenderStates.HOMING_BLENDER:
                if self.elevator_idle():
                    self.rotate_pivot()
                    self.cup_states[3] = True
                    self.blender_state = BlenderStates.IDLE
                    
        # Check if cup has been taken
        if not self.cup_states[4] and self.cup_serve_done():
            self.cup_states[4] = True
            for i, cp in enumerate(self.cup_posns):
                if cp == 4:
                    self.cup_posns.pop(i)

        # Check if all states are idle
        self.all_stations_go = True
        for cs in self.cup_states:
            if cs == False:
                self.all_stations_go = False
            
    def start_carousel_spin(self):
        """Spin carousel one spot"""
        print("Spinning Carousel")
        self.spin_time = time.time() + self.CAROUSEL_SPIN_TIME
        self.carousel.send_command(self.bus, 1, 1)

    def check_carousel_idle(self):
        """Check if carousel is done spinning"""
        if time.time() >= self.spin_time:
            return True
        return False

    def elevator_idle(self):
        """Check if the blender is done"""
        self.blender.read_data(self.bus, print_data=False)
        if self.blender.elevator == 0:
            return True
        return False
    
    def pivot_idle(self):
        """Check if the blender is done"""
        self.blender.read_data(self.bus, print_data=False)
        if self.blender.pivot == 0:
            return True
        return False

    def cup_dispense_done(self):
        """Check if the cup dispense is done"""
        self.dispense.read_data(self.bus, print_data=False)
        if self.dispense.cup == 0:
            return True
        return False

    def cup_serve_done(self):
        """Check if the cup has been taken from serving station"""
        self.carousel.read_data(self.bus, print_data=False)
        if self.carousel.cup_sense1 == 0:
            return True
        return False

    def rotate_pivot(self):
        if self.pivot_location == 0:
            self.blender.send_command(self.bus, 3, 4)
            self.pivot_location = 180
        else:
            self.blender.send_command(self.bus, 3, 3)
            self.pivot_location = 0
