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

# use
Type 'make' to get the executable.

Put the serenityaudio.service file in /etc/systemd/system
sudo systemctl enable serenityaudio
 
