echo "MINIMAL: set crickets 1entensity 0"
curl -s -d '{"effects":{"frogs":{"intensity":0,"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://192.168.1.8:8000/soundscape > /dev/null
sleep 1
echo "set crickets 1entensity 1"
curl -s -d '{"effects":{"frogs":{"intensity":1,"volume":50}}}' -H 'Content-Type: application/json' -X PUT http://192.168.1.8:8000/soundscape > /dev/null
sleep 1
echo "set crickets 1entensity 2"
curl -s -d '{"effects":{"frogs":{"intensity":2,"volume":50}}}' -H 'Content-Type: application/json' -X PUT http://192.168.1.8:8000/soundscape > /dev/null
sleep 1
echo "set crickets 1entensity 3"
curl -s -d '{"effects":{"frogs":{"intensity":3,"volume":50}}}' -H 'Content-Type: application/json' -X PUT http://192.168.1.8:8000/soundscape > /dev/null
sleep 1
echo "set crickets 1entensity 0"
curl -s -d '{"effects":{"frogs":{"intensity":0,"volume":50}}}' -H 'Content-Type: application/json' -X PUT http://192.168.1.8:8000/soundscape > /dev/null
