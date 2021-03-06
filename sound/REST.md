# Audio subsystem

There is a design document a few levels up. Please pay attention to what is happening here instead
of there.

In terms of interfaces, there will be a number of SINKS. We will adopt the PulseAudio definition,
where a SOURCE is INPUT ( like a microphone ), and a SINK is a thing that plays audio ( like a speaker ).

**NOTE: All PUTs and POSTs expect JSON object format.**

## Sinks

Sinks are speakers, but they are also zones of speakers.

There is a call to list all SINKS. ( Should there be an internal "idname" as well as a "humanname" ? )

There is a call to retrieve info about a given SINK.

Each parameter in the SINK ( just volume? ) can be changed ( 0 to 100 where 0 is "off" )

## Zones

A zone is an output area where the volume is set as a group. This is broken out so that 
the individual sculpture can know its zones, and can be manipulated in a human friendly fashion,
without the UI having to know the mapping of individual sink names to zones.


## Effects

An Effect is an insect or something ( not an owl or thunder! )

There is a call to list all EFFECTs.

There is a call to list info about a given EFFECT.

Effects have volume, but they also have "intensity", which is 1, 2, or 3 ( currently ). Both of which can be changed.

Effects play continuously in a loop ( but... might have things like "spacers?" )

## Background

A background is a track without intensity. There may only one "background".

Question: is a "background" so similar to an effect that we should only have one object type?

A 'background' might also have multiple files, for longer loops? Is that interesting?

A background also has a name and volume.

## Soundscape

A soundscape is a collection of:
- A number of effects ( at a given volume and intensity )
- A single Background
- A set of Zone volumes
- A set of Sink volumes
- A master volume

## Master

There may be parameters that effect everything, currently we only have a master volume.
Thus the master object ( in the scape and normally ) just has the one parameter.

## Buttons

There are a number of interactive buttons scattered around the peice.

They send PUT requests to the endpoint /audio/buttons/button-group/button

In that we expect there to be a pair of buttons, and the one on the left will take the 
value of a given effect to 0, and the one on the right will increase it

because it's always hard to tell how people will interact with a peice, changing this
logic should be easy, so it's in one spot.... in this code. So just PUT to the
endpoint in question, and SoundAdmin will have a configuration file that will 
figure out what to do.

# REST interface

## Sinks

GET /audio/sinks - JSON object with the set of SINKs, with the key being the name, and objects being the values

This is a way to get all the sinks.

```
{
	"LeftPergola1": {
		"volume": 50
	},
	"LeftPergola2": {
		"volume": 45
	},
	"LeftPergola3": {
		"volume": 75
	}
}
```

PUT /audio/sinks - Similar JSON object with values. If a SINK is omitted, its OLD value is retained

example:
```
{
	"LeftPergola1": {
		"volume": 50
	},
	"LeftPergola2": {
		"volume": 45
	}
}
```

## Zones

GET /audio/zones - JSON object with the set of ZONEs, with the key being the name, and objects being the values

This is also the way to get the list of zones, if you want to double check

Zones values are always set through the scape, so use that.

```
{
	"names": ["LeftPergola", "RightPergola", "Jars"],
	"LeftPergola": {
		"volume": 50
	},
	"RightPergola": {
		"volume": 45
	},
	"Jars": {
		"volume": 75
	}
}
```



## Effects

If either INTENSITY or VOLUME is zero, then the Effect is not active.

( there is not a separate "active" keyword yet )

GET /audio/effects - JSON object with the list of all available Effects, and the volume and intensity of each.

The name field will have the list of all possible things.

```
{
	"names": ["Crickets", "Crickets2", "Crickets3"],
	"Crickets1": {
		"intensity": 3,
		"volume": 50
	},
	"Crickets2": {
		"intensity": 2,
		"volume": 45
	},
	"Katydids": {
		"intensity": 0,
		"volume": 0
	}
}
```

PUT /audio/effects

Sets the attributes of a set of effects. If you leave out an effect, it goes to 0 intensity
and zero volume.

```
{
	"Crickets1": {
		"intensity": 3,
		"volume": 50
	},
	"Crickets2": {
		"intensity": 0,
		"volume": 
	}
}
```

## Background

Since there can only be one background at a time, the interface isn't exactly the same.

GET /audio/backgrounds

Returns a list of the background names that have been loaded.

```
{
	"names": [ "Ambient1", "Ambient2", "Texas", "Bulgaria" ]

}
```

GET /audio/background

Returns the background that is currently playing.

```
{
	"name": "Ambient",
	"volume": 50
}
```

PUT /audio/background

Set the background that is currently playing, or the attribute.

Note: for style, we could use these as URL parameters intead of a JSON object. I just picked this.

```
{
	"name": "Ambient",
	"volume": 75
}
```

## Buttons

PUT /audio/buttons/:button-group/:button

This signals that a button has been hit. We expect there to be two buttons in a group,
and 0 is the right button and 1 is the left button.

There is a configuration which maps a given button-group to an effect in the config file,
and we'll do the "business logic" ( like figuring the "clamping" ) in SoundAdmin.

## Master

GET /audio/master

Returns a single JSON object which has a Volume field.

PUT /audio/master

Send a single JSON object which has a volume field.

## Soundscapes

Soundscapes are more complex, as we need to create and delete new ones. 

For simplicity the ID field will be the user visible name, instead of using a unique ID that is generated
by the server ( and then having a name internally )

GET /audio/soundscapes - returns a JSON array of soundscapes. For simplicity, we will use the

```
{
	"names": ["MyScape", "BrianScape", "Muzzy"]
}
```

GET /audio/soundscapes/<name>

Retrieves that object. Currently does NOT change the current object, although it could.


DELETE /audio/soundscapes/<name>

Removes this Soundscape from the server system.

POST /audio/soundscapes/<name> - Create a new soundscape with that Name, using the format below.

NOTE: If you pass in a soundscape object, the system will save it with that name and not change the current.
If you don't pass a soundscape, then the current soundscape settings will be used. This is the preferred means.

NOTE: for safety reasons, we are not supporting overwriting a soundscape. The server will inforce
that if a soundscape already exists, an attempt to write the values a second time will return an error.
If you wish to set the values to something new, you must DELETE then POST.

### Soundscape

Soundscape is the current playing one. This persists over server restarts and everything. 

GET /audio/soundscape - returns the JSON object of the current settings

PUT /audio/soundscape - takes as param a JSON blob containing some or all settings. Server updates current settings with whatever properties are set in the update object, leaving other properties untouched, and returns the current soundscape after updates are applied.

### Default Soundscapes

Brian is not so sure about the idea of resetting the default. The default should be baked in. Let's talk.

GET /soundscapes/default - returns the ID of the default soundscape

POST /soundscapes/default - takes a soundscape ID as a parameter, sets that to be the default soundscape

### Soundscape JSON object

When defining a soundscape, if Effects are left out, they are NOT ACTIVE.

If a ZONE is left out, it is.... ? off ?

SINKs are NOT a part of a soundscape. Set them individually.

```
{
	"background": {
		"name": "Ambient",
		"volume": 50
	},
	"effects": {
		"Crickets1": {
			"intensity": 3,
			"volume": 50
		},
		"Crickets2": {
			"intensity": 2,
			"volume": 45
		}
	},
	"zones": {
		"LeftPergola": {
			"volume": 50
		},
		"RightPergola": {
			"volume": 45
		},
		"Jars": {
			"volume": 75
		}
	}
}
```






