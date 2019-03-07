###########################
# Definition of Scheduler #
###########################
import module.module_def as mods
import time
import smbus

class Scheduler:
    FROZEN_DISPENSE_TIME = 5
    LIQUID_DISPENSE_TIME = 5

    def __init__(self):
        self.blender = mods.Blender()
        self.carousel = mods.Carousel()
        self.dispense = mods.Dispense()
        self.bus = smbus.SMBus(1)

        self.cup_posns = []
        self.cup_states = [True for _ in range(carousel.num_slots)]

        self.frozen_time = time.time()
        self.liquid_time = time.time()

    def enqueue_smoothie(self): 
        self.cup_posns.append(0)
        self.cup_states[0] = False

    def empty(self):
        if len(self.cup_posns) == 0:
            return True
        return False

    def update(self):
        for cp in self.cup_posns:
            if cp == 0:
                # Send cup dispense command
                self.dispense.send_command(7, 1)
                self.cup_states[0] = False
            elif cp == 1:
                # Send frozen dispense commands
                self.dispense.send_command(1, 1)
                self.dispense.send_command(2, 1)
                self.dispense.send_command(3, 1)
                self.frozen_time = time.time() + FROZEN_DISPENSE_TIME
                self.cup_states[1] = False
            elif cp == 2:
                # Send liquid dispense commands
                self.dispense.send_command(4, 1)
                self.dispense.send_command(5, 1)
                self.dispense.send_command(6, 1)
                self.liquid_time = time.time() + LIQUID_DISPENSE_TIME
                self.cup_states[2] = False
            elif cp == 3:
                # TODO: BLENDER ROUTINE
                self.cup_states[3] = True
            elif cp == 4:
                # Wait for cup to be taken 
                self.cup_states[4] = False

        while not all_stations_go:
            # Check cup dispense
            if not self.cup_states[0] and cup_dispense_done():
                self.cup_states[0] = True
            # Check frozen dispense
            if not self.cup_states[1] and time.time() >= self.frozen_time:
                self.dispense.send_command(1, 0)
                self.dispense.send_command(2, 0)
                self.dispense.send_command(3, 0) 
                self.cup_states[1] = True
            # Check liquid dispense
            if not self.cup_states[2] and time.time() >= self.liquid_time:
                self.dispense.send_command(4, 0)
                self.dispense.send_command(5, 0)
                self.dispense.send_command(6, 0) 
                self.cup_states[2] = True
            # Check blender
            if not self.cup_states[3] and blender_done():
                self.cup_states[3] = True
            # Check if cup has been taken
            if not self.cup_states[4] and cup_serve_done():
                self.cup_states[4] = True

            # Check if all states are idle
            all_stations_go = True
            for cs in self.cup_states:
                if cs == False:
                    all_stations_go = False

        # Rotate carousel one spot, adjust states
        self.carousel.send_command(bus, 1, 1)
        for i, cp in enumerate(self.cup_posns):
            if cp >= 4:
                self.cup_posns.pop(i)
            else:
                self.cup_posns[i] += 1

    def blender_done(self):
        self.blender.read_data()
        if self.blender.routine == 0:
            return True
        return False

    def cup_dispense_done(self):
        self.dispense.read_data()
        if self.dispense.cup == 0:
            return True
        return False

    def cup_serve_done(self):
        self.carousel.read_data()
        # @ROSS pls check this this is the correct cup-sense
        if self.carousel.cup_sense0 == 0:
            return True
        return False
