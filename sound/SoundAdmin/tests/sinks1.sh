echo "vol 10"
curl -s -d '{"PergolaLeft1":{"volume":10},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 20"
curl -s -d '{"PergolaLeft1":{"volume":20},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 90"
curl -s -d '{"PergolaLeft1":{"volume":90},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 30"
curl -s -d '{"PergolaLeft1":{"volume":30},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 20"
curl -s -d '{"PergolaLeft1":{"volume":20},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 50"
curl -s -d '{"PergolaLeft1":{"volume":50},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 20"
curl -s -d '{"PergolaLeft1":{"volume":20},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 80"
curl -s -d '{"PergolaLeft1":{"volume":80},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 70"
curl -s -d '{"PergolaLeft1":{"volume":70},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1
echo "vol 20"
curl -s -d '{"PergolaLeft1":{"volume":20},"PergolaRight1":{"volume":10}}' -H 'Content-Type: application/json' -X PUT http://pi:9000/audio/sinks > /dev/null
sleep 1


# {"PergolaRight1":{"volume":67},"PergolaRight2":{"volume":50},"PergolaRight3":{"volume":50},"PergolaRight4":{"volume":50},"PergolaRight5":{"volume":50},"PergolaLeft1":{"volume":50},"PergolaLeft2":{"volume":50},"PergolaLeft3":{"volume":50},"PergolaLeft4":{"volume":50},"PergolaLeft5":{"volume":50},"JarBrazen":{"volume":50},"JarM3tric":{"volume":50},"JarJohn":{"volume":50},"Depot":{"volume":50}}
