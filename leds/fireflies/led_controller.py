import socket
import time
import sys

ttl = 8
def createBroadcastSender(ttl=4):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

    return sock

multicast_group = '224.3.29.71'
#multicast_group = '224.0.0.1'
#BROADCAST_ADDR = "255.255.255.255"
LED_CMD_PORT   = 5010

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
