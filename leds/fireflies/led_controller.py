import socket
import time
import sys
from threading import Thread
from threading import Lock

NUM_SWARMS = 4
MULTICAST_GROUP = '224.3.29.71'
#multicast_group = '224.0.0.1'
#BROADCAST_ADDR = "255.255.255.255"
LED_CMD_PORT   = 5010

class FireflyLedController():
    class FireflyPattern():
        def __init__(self, board_id, red, green, blue, speed, pattern_str):
            self.board_id = board_id
            self.red = red
            self.blue = blue
            self.green = green
            self.speed = speed
            self.pattern_str = pattern_str
            self.packet = create_firefly_packet(
                self.board_id,
                [red, green, blue],
                self.speed,
                self.pattern_str
            )

        def get_minimal(self):
            return {
                'board_id':self.board_id,
                'color': [self.red, self.green, self.blue],
                'speed': self.speed,
                'pattern': self.pattern_str
            }       
    def __init__(self):
        self.sender_socket = self.createBroadcastSender()
        self.patterns = {
            board_id : FireflyLedController.FireflyPattern(board_id, 0.5, 0.7, 0.5, 2, "01100110000") \
            for board_id in range(NUM_SWARMS)}

        self.patternLock = Lock()
        self.is_running = True
        self.broadcastThread = Thread(target=self.broadcastFireflyPatterns)
        self.broadcastThread.start()


    def set_led_pattern(self, board_id, red, green, blue, speed, pattern_str):
        self.patternLock.acquire()
        self.patterns[board_id] = FireflyLedController.FireflyPattern(
            board_id,
            red,
            green,
            blue,
            speed,
            pattern_str
        )
        self.patternLock.release()


    def get_led_patterns(self):
        self.patternLock.acquire()
        pattern_copy = [pattern.get_minimal() for pattern in set(self.patterns.values())]
        self.patternLock.release()
        return pattern_copy


    def broadcastFireflyPatterns(self):
        while self.is_running:
            self.patternLock.acquire()
            try:
                for pattern in self.patterns:
                    self.sender_socket.sendto(packet, (MULTICAST_GROUP, LED_CMD_PORT))
                    time.sleep(0.01)
            except Exception:
                pass # I'm concerned about transient networking issues...
                     # and I really don't know what to do if they happen
            self.patternLock.release()
            time.sleep(1.0)
                
    def createBroadcastSender(self, ttl=4):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#       self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)





def simple_checksum(data):
    print(f"Data len is {len(data)}")
    checksum = 0
    for item in data:
        checksum += int(item)
        print(f"{item}, {checksum %256}")    
    return checksum % 256
        
def create_firefly_packet(board_id, color, speed, pattern): 
    cmd_str = 'BL' + ':' + ','.join([str(element) for element in color]) + ':' + str(speed) + ':' + str(pattern)
    print(cmd_str)
    cmd_str_len = len(cmd_str) % 256
    cmd_bytes = cmd_str.encode('utf-8')
    print(f"checksum bytes are {cmd_bytes}")
    checksum = simple_checksum(cmd_bytes)
    print(f"checksum is {checksum}")
    header_id = 'FLG2'.encode('utf-8')
    header_data = bytearray([board_id%256, cmd_str_len, 0, checksum])
    packet = header_id + header_data + cmd_bytes

    return packet

if __name__ == '__main__':
    args = sys.argv
    
    if len(args) < 7:
        print(f"Need more arguments!\n {args[0]} <board_id> <red> <green> <blue> <speed> <pattern> ")
        exit(1)

    board_id = int(args[1])
    red = int(args[2])
    green = int(args[3])
    blue = int(args[4])
    speed = int(args[5])
    pattern = args[6]
    
    packet = create_firefly_packet(board_id, [red,green,blue], speed, pattern);
    print(packet)

    senderSocket = createBroadcastSender()
    led_command = b'hi there'
    while True:
        senderSocket.sendto(packet, (multicast_group, LED_CMD_PORT))
        time.sleep(0.1)
#        print("sending\n") 
#    senderSocket.sendto(led_command, (, LED_CMD_PORT))
#    senderSocket.sendto(led_command, ('<broadcast>', LED_CMD_PORT))
