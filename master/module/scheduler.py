###########################
# Definition of Scheduler #
###########################
import module.module_def as mods

class Scheduler:
    def __init__(self):
        blender = mods.Blender()
        carousel = mods.Carousel()
        dispense = mods.Dispense()
        cup_posns = []
        cup_states = [False for _ in range(carousel.num_slots)]

    def enqueue_smoothie(): 
        cup_posns.append(0)

    def empty():
        if len(cup_posns) == 0:
            return True
        return False

    def update():
        return


