###########################
# Definition of Scheduler #
###########################
import module.module_def as mods
import time
import smbus

class Scheduler:
    FROZEN_DISPENSE_TIME = 4
    LIQUID_DISPENSE_TIME = 6
    CAROUSEL_SPIN_TIME = 1
    BLEND_TIME = 4
    CUP_DISPENSE_TIME = 2

    def __init__(self):
        self.clock = 0
        self.blender = mods.Blender()
        self.carousel = mods.Carousel()
        self.dispense = mods.Dispense()
        self.bus = smbus.SMBus(1)

        self.cup_posns = []
        self.cup_states = [True for _ in range(self.carousel.num_slots)]
        self.all_stations_go = True

        self.frozen_time = time.time()
        self.liquid_time = time.time()
        self.spin_time = time.time()
        self.blend_time = time.time()
        self.blend_cycles = 0

    def empty(self):
        """Check if the machine is empty"""
        if len(self.cup_posns) == 0:
            return True
        return False

    def check_all_stations_go(self):
        return self.all_stations_go

    def add_cup(self, posn):
        """Add cup to queue and start action"""
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> Scheduler changes
        if not self.check_carousel_idle:
            posn -= 1
        if posn not in self.cup_posns:
            self.cup_posns.append(posn)

<<<<<<< HEAD
=======
        self.cup_posns[posn] = True
>>>>>>> Scheduler changes
=======
>>>>>>> Scheduler changes
        self.all_stations_go = False
        if posn == 0:
            # Send cup dispense command
            self.dispense.send_command(self.bus, 7, 1)
            self.cup_states[0] = True
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
            # TODO: BLENDER ROUTINE
            self.cup_states[3] = True
            self.blend_cycles = 4
            self.blend_time = time.time + self.BLEND_TIME
            self.dispense.send_command(self.bus, 4, 2)
        elif posn == 4:
            # Wait for cup to be taken 
            self.cup_states[4] = True

    def shift_cups(self):
        """Shift all cups by one slot"""
        for i, cp in enumerate(self.cup_posns):
            if cp >= 4:
                self.cup_posns.pop(i)
            else:
                self.cup_posns[i] += 1
        for posn in self.cup_posns:
            self.add_cup(posn)

    def update(self):
        """Non-blocking update routine"""

        # Check if there is anything to do
        if self.all_stations_go or not self.check_carousel_idle():
            return
        # Check cup dispense
        if not self.cup_states[0] and cup_dispense_done():
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
            if time.time >= self.blend_time:
                self.blend_cycles -= 1
                if self.blend_cycles != 0:
                    self.blend_time = time.time + self.BLEND_TIME
                    self.dispense.send_command(self.bus, 4, 2)
                else:
                    self.cup_states[3] = True
                    self.dispense.send_command(self.bus, 4, 0)
        # Check if cup has been taken
        if not self.cup_states[4] and cup_serve_done():
            self.cup_states[4] = True

        # Check if all states are idle
        self.all_stations_go = True
        for cs in self.cup_states:
            if cs == False:
                self.all_stations_go = False
            
    def start_carousel_spin(self):
        """Spin carousel one spot"""
        print "Spinning Carousel"
        self.spin_time = time.time() + self.CAROUSEL_SPIN_TIME
        self.carousel.send_command(self.bus, 1, 1)

    def check_carousel_idle(self):
        """Check if carousel is done spinning"""
        if time.time() < self.spin_time:
            return False
        return True

    def blender_done(self):
        """Check if the blender is done"""
        self.blender.read_data()
        if self.blender.routine == 0:
            return True
        return False

    def cup_dispense_done(self):
        """Check if the cup dispense is done"""
        self.dispense.read_data()
        if self.dispense.cup == 0:
            return True
        return False

    def cup_serve_done(self):
        """Check if the cup has been taken from serving station"""
        self.carousel.read_data()
        # @ROSS pls check this this is the correct cup-sense
        if self.carousel.cup_sense0 == 0:
            return True
        return False
