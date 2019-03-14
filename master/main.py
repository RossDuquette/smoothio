######################
# Main functionality #
######################

import module.scheduler as scdr
from flask import Flask  
import thread
import time

app = Flask(__name__)
scheduler = scdr.Scheduler()
smoothie_queue = []

def intro():
    print("""
    --------------------
      RUNNING SCHEDULER
    ---------------------
    """)

@app.route("/enqueue")
def enqueue():
    print("Enqueuing Smoothie")
    smoothie_queue.append(0)
    return "Request Satisfied"

def flask_server():
    app.run(host= '0.0.0.0')

def main():
    intro() 
    print "Homing Everything"
    scheduler.home_everything()
    resp = input("Send any command when done homing: ")

    carousel_spinning = False
    # while scheduler.empty():
    #     pass
    smoothie_queue.append(0)
    # smoothie_queue.append(0)
    # smoothie_queue.append(0)

    # Run scheduler until the smoothie has been made
    while True:
        print scheduler.cup_posns
        if scheduler.empty() and len(smoothie_queue) > 0:
            smoothie_queue.pop()
            scheduler.cup_posns.append(0)
            scheduler.add_cup(0) 
        if carousel_spinning:
            if scheduler.check_carousel_idle():
                carousel_spinning = False
                scheduler.shift_cups()
                if len(smoothie_queue) > 0:
                    smoothie_queue.pop()
                    scheduler.cup_posns.append(0)
                    scheduler.add_cup(0)
        else:
            scheduler.update()
            if scheduler.check_all_stations_go():
                if not scheduler.empty():
                    scheduler.start_carousel_spin()
                    carousel_spinning = True
        
        time.sleep(0.1)

if __name__ == "__main__":
    thread.start_new_thread(flask_server,())
    print("Waiting For Flask Server")
    time.sleep(2)
    main()
