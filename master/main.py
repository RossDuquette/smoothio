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
    scheduler.enqueue_smoothie()

    # Run scheduler until the smoothie has been made
    while not scheduler.empty():
        scheduler.update()

if __name__ == "__main__":
    main()
