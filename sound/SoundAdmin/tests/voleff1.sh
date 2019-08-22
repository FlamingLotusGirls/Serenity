echo "turn on birds"
curl -s -d '{"effects": {"birds": {"volume":100,"intensity": 2}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
echo "set volume 10"
curl -s -d '{"effects": {"birds": {"volume":10}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 80"
curl -s -d '{"effects": {"birds": {"volume":80}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 20"
curl -s -d '{"effects": {"birds": {"volume":20}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 90"
curl -s -d '{"effects": {"birds": {"volume":90}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 15"
curl -s -d '{"effects": {"birds": {"volume":15}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 70"
curl -s -d '{"effects": {"birds": {"volume":70}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 60"
curl -s -d '{"effects": {"birds": {"volume":60}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 0"
curl -s -d '{"effects": {"birds": {"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1
echo "set volume 50"
curl -s -d '{"effects": {"birds": {"volume":50}}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/soundscape > /dev/null
sleep 1