from gpiozero import Button
import requests

from time import sleep
SOUND_SERVER = "192.168.1.10:9000"

buttons = [
    {'id':7, 'group': 1, 'function': 1, 'button': Button("BOARD33", bounce_time=0.2)},
    {'id':8, 'group': 1, 'function': 0, 'button': Button("BOARD35", bounce_time=0.2)},
    {'id':9, 'group': 2, 'function': 1, 'button': Button("BOARD36", bounce_time=0.2)},
    {'id':10, 'group': 2, 'function': 0, 'button': Button("BOARD37", bounce_time=0.2)},
    {'id':11, 'group': 3, 'function': 1, 'button': Button("BOARD38", bounce_time=0.2)},
    {'id':12, 'group': 3, 'function': 0, 'button': Button("BOARD40", bounce_time=0.2)},
]

# board_button is the object that triggered

def send_sound_action(board_button):
    button = [button for button in buttons if button['button'] == board_button][0]
    url = "http://" + SOUND_SERVER + "/audio/buttons/" + \
          str(button['group']) + "/" + str(button['function'])
    print(f"url is {url}")


    r = requests.put(url)
 
#	r = requests.put(
#        "http://" + SOUND_SERVER + "/audio/buttons/" +
#        board['group'] + "/" + board['function']
#    )
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

