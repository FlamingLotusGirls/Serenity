echo "set entensity 2"
curl -s -d '{"effects": {"birds": {"volume":75,"intensity": 2}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 2
echo "set intensity 0"
curl -s -d '{"effects": {"birds": {"volume":75,"intensity": 0}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null 
sleep 2
echo "set intenisty 3"
curl -s -d '{"effects": {"birds": {"volume":75,"intensity": 3}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 2
echo "set intensity 0"
curl -s -d '{"effects": {"birds": {"volume":75,"intensity": 0}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 2
echo "set intensity 1"
curl -s -d '{"effects": {"birds": {"volume":75,"intensity": 1}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 2
echo "set intensity 0"
curl -s -d '{"effects": {"birds": {"volume":75,"intensity": 0}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 2
