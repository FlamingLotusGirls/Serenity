# SoundAdmin

Listens on the main port, keeps track of state of things, directs individual speakers

Charlie says that if I write this in Node, he can maintain it

so, node!

Version from regular raspberrian seems out of date with NPM which is a pity.

```
sudo apt install nodejs
sudo apt install npm
sudo npm install npm@latest -g
```

In the directory, the npm is not most recent? `sudo npm install npm@latest -g`

```
sudo npm install
```

Get the ARM v8 version from nodejs.org
sudo tar xvf node-... -C /opt
sudo mv /opt/node.... /opt/node
Add node to everyone's path: /opt/node/bin to /etc/profile

# test code

curl -v  http://localhost:9000/audio/background

curl -v -d '{"name": "Ambient1","volume": 60}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/background

curl -v -d '{"name": "Austinss","volume": 60}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/background  

curl -v  http://localhost:9000/audio/effects  

curl -v -d '{"crickets": {"volume":50,"intensity": 1}, "birds": {"volume":75,"intensity": 2}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v -d '{"crickets": {"volume":50,"intensity": 4}, "birds": {"volume":75,"intensity": 2}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v -d '{"frogs": {"volume":50,"intensity": 1}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v -d '{"frogs": {"volume":0,"intensity": 1}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v -d '{"XXXX": {"volume":0,"intensity": 3}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v -d '{"XXXX": {"volume":50,"intensity": 4}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v -d '{"frogs": {"volume":50,"intensity": 4}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/effects 

curl -v  http://localhost:9000/audio/zones 

curl -v  http://localhost:9000/audio/sinks 

curl -v -d '{"PergolaLeft5": {"volume":20}, "PergolaRight1": {"volume":10}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/sinks 

curl -v  http://localhost:9000/audio/soundscapes

curl -v -X POST http://localhost:9000/audio/soundscapes/test1

curl -v -X DELETE http://localhost:9000/audio/soundscapes/test1

curl -v  http://localhost:9000/audio/soundscape

curl -v -d '{"effects": { "crickets": {"volume":50,"intensity": 0}}}' -H 'Content-Type: application/json' -X PUT http://localhost:9000/audio/soundscape  

curl -v  http://localhost:9000/audio/soundscapes  

