import socket
import struct

multicast_group = '224.3.29.71'
#multicast_group = '224.0.0.1'
#BROADCAST_ADDR = "255.255.255.255"
LED_CMD_PORT   = 5010


def createBroadcastListener(port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Set some options to make it multicast-friendly
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    except AttributeError:
        pass # Some systems don't support SO_REUSEPORT

    group = socket.inet_aton(multicast_group)
    mreq = struct.pack('4sL', group, socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    #sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    sock.bind(('', port))

    return sock

if __name__ == '__main__':
    listener = createBroadcastListener(LED_CMD_PORT)
    while True:
        data = listener.recv(1024) 
        print(data)
    
        
