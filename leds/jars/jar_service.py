#!/usr/bin/env python3
from jar_led_driver import JarLedDriver
import jar_webserver

import queue

def main():
    # get id of this jar.
    try: 
        with open("/etc/jar_id", "r") as id_file:
            jar_id = int(id_file.read())
    except:
        jar_id = 1
        print(f"Cannot read jar id from file, defaulting to {jar_id}")

    # create communication queues for passing messages between the webserver
    # and the lower level code. The webserver will pass a command on the 
    # cmd_queue, and the led_driver will respond back on the resp_queue. 
    cmd_queue = queue.Queue()
    resp_queue = queue.Queue()

    # create jar led driver
    jar_driver = JarLedDriver(cmd_queue, resp_queue, jar_id)

    # create jar webserver
    jar_webserver.init(cmd_queue, resp_queue, jar_id)

    # main thread - run webserver
    try:
        jar_webserver.serve_forever()
    except Exception as e:  # XXX includes KeyboardInterrupt?
        print(f"Exception {e}")
    finally:
        jar_driver.stop() # will do a join
        
        
if __name__ == "__main__":
    main()
