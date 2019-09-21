# Where are the actual sounds?

There is a Google Drive folder that holds the sounds, because the compressed version is about 145MB.

This folder is here: https://drive.google.com/drive/folders/1bvNyOcxjq2taruH4BgBfTN2E9kwuVKE0?usp=sharing

There should be a file 'flg_sound.zip'. Please decompress it in this directory.

# Names of speakers

Your best reference is the SoundAdmin default JSON file, one of which is in SoundAdmin/data.

The names of the speakers are replicated here:
"PergolaRight1", "PergolaRight2",  "PergolaRight3", "PergolaRight4", "PergolaRight5", 
"PergolaLeft1", "PergolaLeft2", "PergolaLeft3", "PergolaLeft4", "PergolaLeft5", 
"JarBrazen", "JarM3tric", "JarJohn", "Depot" 

The current names of the sounds are:
"birds", "cicadas", "crickets", "frogs", "frogsAlt"

# a few elements to remember for sound

## Add User

Add the user you want to use ( in this case `flaming` to audio and pulse ).

sudo usermod -a -G pulse flaming
sudo usermod -a -G audio flaming

## alsamixer

A fresh install of raspberrypi gives an initial alsa audio setting of 50 which is really low.

Good news is this is sticky. While there is a command line incantation, I went into
`alsamixer` and simply set the volume to 95.

# auto-restart of serenityaudio

What an unholy pain! Here are the basic steps.

Put the 'pulseaudio.service' in, in the usual way, and enable it and make sure it starts. Put it
in `\etc\systemd\system`

For your memory aid, here are the commands of greatest use
- sudo systemctl status pulseaudio.service
- sudo systemctl start pulseaudio.service
- sudo systemctl enable pulseaudio.service
- sudo systemctl daemon-reload # for each enabled service copies version in etc to its startup stash
- sudo journalctl -u pulseaudio.service # also excepts the very useful -f follow option

## change an element of PulseAudio config

By default, PulseAudio works hard to protect your privacy by not allow random processes to attach,
notably to listen to your microphone ( maybe playing sound isn't so bad ). The authors greatly
worry about this, and discourage the use of --system mode, and then have a system of privs
called a 'cookie' file which is randomly generated on system startup ( and placed probably in  `/var/run/pulse/.config/cookie` or something).

You will see this in action if you attempt to start the `serenityaudio.service`, and you get
a `connection refused` error.

While it would be properly secure to use this mechanism, it's a great mechanism, a variety of
problems plagued me from actually using it. Those included: the generated file had rw only for
the pulse user not the pulse group, so my 'flaming' user couldn't access it (easily). The
--system daemon resisted efforts to use an environment variable to load a different cookie file.

It turns out you can disable this security check as follows:
In the file `/etc/pulse/system.pa` there are a number of load-module, just find the one that's
already in there and add `auth-anonymous=1`.

As such:

```
load-module module-native-protocol-unix auth-anonymous=1
```

## ordering

I notice that serenityaudio still can't connect the first time, but it does connect properly
if you do the work manually. Therefore, I've put in a 12 second auto-restart, which is a good
idea for a crashed process anyway, and you'll see in the log that the first time, it doesn't
connect. Don't be alarmed.

You'll notice that serenityaudio is listed to be after pulseaudio is loaded and up, so I don't know
what the deal is. This is a reasonable enough hack for my purpose.


