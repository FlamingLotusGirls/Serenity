# SoundAdmin

This runs on the master node.

It supports all the REST calls in the above specification.

It stores the list of raspberry pis.

It acts as a router for the coming ( single ) admin call, and fans out to the raspberry pis ( SerenityAudio ).

It persists all the Effects and whatnot.

Written in python.

Choosing Aiohttp, because we need to fan out. I'd rather have all the requests go out in parallel,
in case one is flakey, all the other ones have a chance to immediately take effect, and
the other one can catch up. Hopefully it's not too crazy to program.

# Requires python3

```
sudo apt install python3-pip
python3 -m pip install aiohttp
python3 -m pip install cchardet
python3 -m pip install aiodns

python3 SoundAdmin.py
```
