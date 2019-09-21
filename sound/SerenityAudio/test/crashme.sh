host=localhost

echo "set frogs 1entensity 0"
curl -s -d '{"effects":{"crickets":{"intensity":0,"volume":50},"birds":{"intensity":2,"volume":45},"cicadas":{"intensity":0,"volume":0},"frogs":{"intensity":0,"volume":50},"frogsAlt":{"intensity":0,"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://$host:8000/soundscape > /dev/null
sleep 1
echo "set frogs 1entensity 1"
curl -s -d '{"effects":{"crickets":{"intensity":0,"volume":50},"birds":{"intensity":2,"volume":45},"cicadas":{"intensity":0,"volume":0},"frogs":{"intensity":1,"volume":50},"frogsAlt":{"intensity":0,"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://$host:8000/soundscape > /dev/null
sleep 1
echo "set frogs 1entensity 2"
curl -s -d '{"effects":{"crickets":{"intensity":0,"volume":50},"birds":{"intensity":2,"volume":45},"cicadas":{"intensity":0,"volume":0},"frogs":{"intensity":2,"volume":50},"frogsAlt":{"intensity":0,"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://$host:8000/soundscape > /dev/null
sleep 1
echo "set frogs 1entensity 3"
curl -s -d '{"effects":{"crickets":{"intensity":0,"volume":50},"birds":{"intensity":2,"volume":45},"cicadas":{"intensity":0,"volume":0},"frogs":{"intensity":3,"volume":50},"frogsAlt":{"intensity":0,"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://$host:8000/soundscape > /dev/null
sleep 1
echo "set frogs 1entensity 0"
curl -s -d '{"effects":{"crickets":{"intensity":0,"volume":50},"birds":{"intensity":2,"volume":45},"cicadas":{"intensity":0,"volume":0},"frogs":{"intensity":0,"volume":50},"frogsAlt":{"intensity":0,"volume":0}}}' -H 'Content-Type: application/json' -X PUT http://$host:8000/soundscape > /dev/null
