######################
# Main functionality #
######################

import module.scheduler as scdr
from flask import Flask  
import thread
import time

app = Flask(__name__)
scheduler = scdr.Scheduler()

def intro():
    print("""
    --------------------
      RUNNING SCHEDULER
    ---------------------
    """)

@app.route("/enqueue")
def enqueue():
    print("Enqueuing Smoothie")
    scheduler.add_cup(0)
    return "Request Satisfied"

def flask_server():
    app.run(host= '0.0.0.0')

def main():
    intro() 
    scheduler.add_cup(0)
    carousel_spinning = False
    # Run scheduler until the smoothie has been made
    while not scheduler.empty():
        scheduler.update()
        if carousel_spinning and not scheduler.check_carousel_idle():
            carousel_spinning = False
            scheduler.shift_cups()
        if scheduler.check_all_stations_go():
            
            scheduler.start_carousel_spin()
            carousel_spinning = True

if __name__ == "__main__":
    # thread.start_new_thread(flask_server,())
    # print("Waiting For Flask Server")
    # time.sleep(2)
    main()
