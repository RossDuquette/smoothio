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
    running = True

    # Run scheduler until the smoothie has been made
    while not scheduler.empty():
        scheduler.update()
        if scheduler.check_all_stations_go():
            schedu




if __name__ == "__main__":
    main()
