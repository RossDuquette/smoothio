######################
# Main functionality #
######################

import smbus
import module.scheduler as scdr

def main():
    scheduler = scdr.Scheduler()
    scheduler.enqueue_smoothie()

    # Run scheduler until the smoothie has been made
    while not scheduler.empty():
        scheduler.update()

if __name__ == "__main__":
    main()