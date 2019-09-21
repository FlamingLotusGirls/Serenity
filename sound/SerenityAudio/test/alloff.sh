host=localhost

echo "set all effects off "
curl -s -d '{"effects":{"crickets":{"intensity":0},"birds":{"intensity":0},"cicadas":{"intensity":0},"frogs":{"intensity":0},"frogsAlt":{"intensity":0}}}' -H 'Content-Type: application/json' -X PUT http://$host:8000/soundscape > /dev/null
