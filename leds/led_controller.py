import socket
import time

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

if __name__ == '__main__':

    senderSocket = createBroadcastSender()
    led_command = b'hi there'
    for i in range(300):
        senderSocket.sendto(led_command, (multicast_group, LED_CMD_PORT))
    time.sleep(0.1) 
#    senderSocket.sendto(led_command, (, LED_CMD_PORT))
#    senderSocket.sendto(led_command, ('<broadcast>', LED_CMD_PORT))
