#!/bin/bash
#
# This is the startup code for the OPC server for the Serenity jars.
#

echo starting Serenity LED service on `hostname`

JARID=`cat /etc/jar.id`

HOME=/home/flaming
OPC_SERVER=$HOME/fadecandy/bin/fcserver-rpi
OPC_CONFIG=$HOME/Serenity/leds/jars/serenity_fc-config_$JARID.json

#OPCLOG=/var/log/opc_server.log
#
#CYCLELOVGS=$HOME/pulse/pi_startup/cycleLogs.sh
#
#$CYCLELOGS $BPMLOG
#$CYCLELOGS $SOUNDLOG
#$CYCLELOGS $OPCLOG
#$CYCLELOGS $LEDLOG

# start OPC server
$OPC_SERVER $OPC_CONFIG
#stdbuf -oL $OPC_SERVER $OPC_CONFIG  &


