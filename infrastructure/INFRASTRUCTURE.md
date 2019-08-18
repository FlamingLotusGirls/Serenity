#Some notes on Serenity Infrastructure:

##Naming conventions
Serenity's software is mostly run by a network of 9 raspberry pis. The names (and locations, and functionality) of these computers is as follows:
- **Master**. In a box in the fuel depot. Runs the main UI, the core webservers, the small firefly LEDs  and the flame controller.  Note that the box with the master pi also contains the main router for the piece.
- **Fuel Depot** In a box in the fuel depot. Responsible for the fuel depot audio sources
- **Jar 1 (Metric)** In the first jar fragment (on left as seen from the fuel depot). Runs jar LEDs and audio.
- **Jar 2 (John)** In the second jar fragment (center as seen from the fuel depot). Runs jar LEDs and audio.
- **Jar 3 (Brazen)** In the third jar fragment (right as seen from the fuel depot). Runs jar LEDs and audio.
- **Pergola Right Near** In the closer of the two boxes on the right pergola, as seen from the fuel depot. Runs two audio sources, responsible for reading one set of audio buttons.
- **Pergola Right Far** In the farther of the two boxes on the right pergola, as seen from the fuel depot. Runs three audio sources.
- **Pergola Left Near** In the closer of the two boxes on the left pergola, as seen from the fuel depot. Runs two audio sources, responsible for reading one set of audio buttons.
- **Pergola Left Far** In the farther of the two boxes on the left pergola, as seen from the fuel depot. Runs three audio sources.

##IP Addresses
The network is at 192.168.1.xxx. All of the pis have static ip addresses (although the individual small fireflies are dynamically addressed by the dhcp server.)

Addresses are as follows:
- **Master** 192.168.1.10
- **Jar 1**  192.168.1.11
- **Jar 2**  192.168.1.12
- **Jar 3**  192.168.1.13
- **Pergola Right Near** 192.168.1.14
- **Pergola Right Far**  192.168.1.15 
- **Pergola Left Near** 192.168.1.16
- **Pergola Left Far**  192.168.1.17
- **Fuel Depot**        192.168.1.1

##Communications Protocol
- Flame control is over an RS485 bus, using FLG's standard '!' protocol. See source code or FLG wiki for more information
- Firefly LED control is over multicast UDP packets. Multicast group is 224.3.29.71
- Web services run at well defined ports, as follows:
  - **Admin webservice:** 3000
  - **Flames webservice:** 5000
  - **Firefly LED webservice:** 7000
  - **Jar LED webservice:** 8000
  - **Audio webservice:** 9000
All services use REST protocols. 
