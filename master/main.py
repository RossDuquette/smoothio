######################
# Main functionality #
######################

import module.scheduler as scdr

def intro():
    print """
    --------------------
      RUNNING SCHEDULER
    ---------------------
    """

def main():
    intro() 
    scheduler = scdr.Scheduler()
    scheduler.add_cup(0)
    carousel_spinning = False

    # Run scheduler until the smoothie has been made
    while not scheduler.empty():
        scheduler.update()
        # CHECK FOR NEW CUP ADDS
        if carousel_spinning and not scheduler.check_carousel_idle():
            carousel_spinning = False
            scheduler.shift_cups()
        if scheduler.check_all_stations_go():
            scheduler.start_carousel_spin()
            carousel_spinning = True

if __name__ == "__main__":
    main()
