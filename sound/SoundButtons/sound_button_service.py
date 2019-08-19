from gpiozero import Button
import requests

from time import sleep
SOUND_SERVER = "localhost:9000"

buttons = [
    {'id':7, 'group': 1, 'function': 1, 'button': Button("BOARD33", bounce_time=0.1)},
    {'id':8, 'group': 1, 'function': 0, 'button': Button("BOARD35", bounce_time=0.1)},
    {'id':9, 'group': 2, 'function': 1, 'button': Button("BOARD36", bounce_time=0.1)},
    {'id':10, 'group': 2, 'function': 0, 'button': Button("BOARD37", bounce_time=0.1)},
    {'id':11, 'group': 3, 'function': 1, 'button': Button("BOARD38", bounce_time=0.1)},
    {'id':12, 'group': 3, 'function': 0, 'button': Button("BOARD40", bounce_time=0.1)},
]

# board_button is the object that triggered

def send_sound_action(board_button):
    button = [button in buttons if button['button'] == board_button]
    print(f"button is {button}")
    print(f"board_button is {board_button}")
    r = requests.put(
        "http://" + SOUND_SERVER + "/audio/buttons/" +
        board_button['group'] + "/" + board_button['function']
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

