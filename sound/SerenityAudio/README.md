# SerenityAudio
A quick and simple audio configuration that allows playing samples of crickets and frogs and an ambiant wash for the FLG's burning man project.

Basic idea

A headless raspberry pi system, with USB audio, drives many small speakers

Each RPI has some kind of network ( rest ) interface, and can have "more" of each animal
Volume for different things - speakers, animals - should be individually controllable
Adding "more" of a bug will add more-per-second

First attempt is using PulseAudio. Rasperrian is at 'stretch', which is before 'buster'.
The version of PulseAudio is 10.0, and so far works out of the box.

# Dependancies
jansson - sudo apt install libjansson-dev
pulseaudio - sudo apt install libpulse-dev
soundfile - sudo apt install libsndfile1-dev
microhttpd - sudo apt install libmicrohttpd-dev

# Also need libcurl, package seems bad?

Have to install from scratch

```
wget https://curl.haxx.se/download/curl-7.65.3.tar.gz
tar xzf curl-7.65.3.tar.gz
cd curl-7.65.3
./configure --without-ssl --prefix=/usr
make
sudo make install
```

# use
Type 'make' to get the executable.

Put the serenityaudio.service file in /etc/systemd/system
sudo systemctl enable serenityaudio

# testing

Since the program has a rest interface, the easiest way to create tests
is to have CURL bang against the port. The port by default is 8000.
You will see a test directory, and that has a few .sh which form some nice tests.

Feel free to add to those.

# startup

It's really nice to have the system come up with a nice sound by default, even if
it can't find its master server. The sytem has a 'recentScapes' json file which
is the initial JSON file. This file will be overwritten each time it receives a new
value, which isn't great because it might have a terrible value... a better system might
be to always startup with 'nice', and not overwrite that file, but you also 
are happy to have a system where a given pi can restart and get back to it's old place.

feel free to change the behavior!

# where are my sounds?

Go up a directory to the readme there, you'll see a link to a google driver folder
hiwhc has gets a zip file with the sounds we created. Pull that down an uncompress
it in the parent directory.

# debugging

You'll see two things about debugging. There is a verbose flag, which allows turning
on a lot of things, then poor man's unit debuggers because there's a variable called
'ldebug' ( localdebug ) in some files which will allow you to turn to true, recompile,
and see other things.

Remember that when you're running as a service, it's pretty easy to just journalctl -f -u servicename , which gives you a "follow" of the log.

# pulseaudio

Two comments. First, the pulseaudio people suggested that I try 'gstreamer' next time I try
a project like this. Gstreamer is a little higher level, so doesn't require things
like the reference counted system. OTOH, I don't really see how you do the level of 
volume control this code gets out of pulseaudio.

There is a secret regarding how to create a pulseaudio and serenityaudio which can both
be run as a service. There's a magic parameter to the pulseaudio module which disables
security checkes. This is explained in the readme in the above directory, which has
the service, and also describes the one parameter you have to add to the pulseaudio
configuration file.

# todo

The "master volume" didn't get working, and neither, really, is the zone volume
working. The right thing is to multiply those three things gether and divide.
Which means tracking the master and the zone values, which just doesn't have a data structure. Sorry about that. 
