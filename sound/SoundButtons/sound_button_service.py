from gpiozero import Button
import requests

from time import sleep
SOUND_SERVER = "localhost:9000"

buttons = [
    # Bank 1
    {'id':1, 'type': 'cricket', 'function':'on', 'button': Button("BOARD11", bounce_time=0.1)},
    {'id':2, 'type': 'cricket', 'function':'off', 'button': Button("BOARD12", bounce_time=0.1)},
    {'id':3, 'type': 'cicada', 'function': 'on', 'button': Button("BOARD13", bounce_time=0.1)},
    {'id':4, 'type': 'cicada', 'function': 'off', 'button': Button("BOARD15", bounce_time=0.1)},
    {'id':5, 'type': 'katydid', 'function': 'on', 'button': Button("BOARD16", bounce_time=0.1)},
    {'id':6, 'type': 'kaytdid', 'function': 'off', 'button': Button("BOARD18", bounce_time=0.1)},
    # Bank 2
    {'id':7, 'type': 'peeper', 'function': 'on', 'button': Button("BOARD33", bounce_time=0.1)},
    {'id':8, 'type': 'peeper', 'function': 'off', 'button': Button("BOARD35", bounce_time=0.1)},
    {'id':9, 'type': 'whippoorwill', 'function': 'on', 'button': Button("BOARD36", bounce_time=0.1)},
    {'id':10, 'type': 'whippoorwill', 'function': 'off', 'button': Button("BOARD37", bounce_time=0.1)},
    {'id':11, 'type': 'owl', 'function': 'on', 'button': Button("BOARD38", bounce_time=0.1)},
    {'id':12, 'type': 'owl', 'function': 'off', 'button': Button("BOARD40", bounce_time=0.1)},
]

# board_button is the object that triggered

def send_sound_action(board_button):
    
    r = requests.post(
        "http://" + SOUND_SERVER + "/effects/" + board_button['type'],
        data={"onOff": board_button['function']}
    )
    print(r.status_code, r.reason)
   
def init():
    for b in buttons:
        b['button'].when_pressed = send_sound_action
 
def main():
    while(True):
        sleep(0.1) # is there every any reason to wake up?

if __name__ == "__main__":
    init()
    main()

