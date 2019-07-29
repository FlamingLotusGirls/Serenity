# Serenity
Things related to our flaming fireflies

## Audio and Sound

Please see the sound directory.

Generally, there is a daemon running on Raspberry pis, about 7 of them ( 2 per pergola and 1 per jar ).
These are written in C, have a simple HTTP server to receive REST calls, and are written to 
PulseAudio, an audio interface which allows easy manipulation of multiple outputs, and volumes and modules
which are manipulated via code. While there are interfaces in higher level languages, the "zero copy"
nature of PulseAudio is best done through C, and C is a language I like.

Due to the fact that it's hard to store audio files, we've created a google drive folder which has the
actual files. That directory should be copied directly into a SerenityAudio server.

There is then another server on the "master" which is driven by the admin UI, and buttons, and knows about the
PIs throughout the sculpture.

 
