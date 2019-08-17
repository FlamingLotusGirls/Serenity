
const fs = require('fs');

const express = require('express');
const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true })); // I think this means we support that too

const port = 3000;

// https://codeburst.io/node-js-best-practices-smarter-ways-to-manage-config-files-and-variables-893eef56cbef
const config = require('./data/config.json')

var g_scape = null;
var g_sinks = null;

// SCAPE FILES

// Read from the default file
function scape_init() {
	// first try to load the last thing that was set
	//console.log('scape init');
	var rawdata = null;
	if (fs.existsSync(config.currentScape)) {
		rawdata = fs.readFileSync(config.currentScape);
	}
	else {
		rawdata = fs.readFileSync(config.soundscapeDir + 'default.json');
	}
	var s = JSON.parse(rawdata);
	//console.log(s);
	return(s);
}

// write the newly changed scape to the last file 
function scape_flush(s) {
	fs.writeFileSync(config.currentScape, JSON.stringify(s));
	console.log(' flushed current scape ');
}

// Init function, load the scape
g_scape = scape_init();

// SINK FILE

// Read from the default file
function sink_init() {
	// first try to load the last thing that was set
	//console.log('scape init');
	var rawdata = null;
	if (fs.existsSync(config.currentSink)) {
		rawdata = fs.readFileSync(config.currentSink);
	}
	else {
		rawdata = fs.readFileSync(config.defaultSink);
	}
	var s = JSON.parse(rawdata);
	//console.log(s);
	return(s);
}

// write the newly changed scape to the last file 
function sink_flush(s) {
	fs.writeFileSync(config.currentSink, JSON.stringify(g_sink));
	console.log(' flushed current sink ');
}

// Init function, load the scape
g_sink = sink_init();



//
// Endpoints
//


// Get the list of all possible backgrounds
app.get('/audio/backgrounds', (req, res) => {
	res.json(config.backgrounds)
})

// the current playing
app.get('/audio/background', (req,res) => {
	res.json(g_scape.background);
})

app.put('/audio/background', (req, res) => {
	console.log('got a PUT audio background request');
	console.log(req.body);

	if (req.body.hasOwnProperty('name') == false) {
		res.status(400);
		res.send('background must have a name');
		return;
	}
	if (req.body.hasOwnProperty('volume') == false) {
		res.status(400);
		res.send('background must have a volume');
		return;
	}
	if (config.backgrounds.names.includes(req.body.name) == false) {
		res.status(400);
		res.send(req.body.name + ' is not a supported name');
		return;
	}

	g_scape.background.name = req.body.name;
	g_scape.background.volume = req.body.volume;

	scape_flush(g_scape);

	// TODO: notify all players of new background

	res.send('OK')
})

// Returns the current list of sounscapes as an array
app.get('/audio/soundscapes', (req, res) => {
	res.send('Got a GET audio backgrounds request')
})

// create a new soundscale with id 'id' using the current senttings
app.post('/audio/soundscapes', (req, res) => {
	res.send('Got a PUT audio backgrounds request')
})

app.get('/audio/effects', (req,res) => {
	//console.log(" getting effects ");
	var effects = {};
	effects.names = config.effects.names;
	for (const [key, value] of Object.entries(g_scape.effects)) {
		if ( (value.volume > 0) &&
			 (value.intensity > 0) ) {
			effects[key] = value;
		}
	}
	//console.log(effects);
	res.send(effects)
})

app.put('/audio/effects', (req,res) => {
	//console.log('got a PUT effects request');
	//console.log(req.body);

	// Validate input
	for (const [key, value] of Object.entries(req.body)) {
		console.log(' validating %s object %s ',key,JSON.stringify(value));
		// is the name valid
		if (config.effects.names.includes(key) == false) {
			res.status(400);
			res.send(key + ' is not a supported name')
			return;
		}
		if (value.hasOwnProperty('intensity') == false) {
			res.status(400);
			res.send(key + ' must have intensity')
			return;
		}
		if ((value.intensity > 3) || (value.intensity < 0)) {
			res.status(400);
			res.send(key + ' intensity out of range');
			return;
		}
		if (value.hasOwnProperty('volume') == false) {
			res.status(400);
			res.send(key + ' must have volume')
			return;
		}
		if ((value.volume > 100) || (value.volume < 0)) {
			res.status(400);
			res.send(key + ' volume out of range');
			return;
		}
	}

	// valid, replace
	g_scape.effects = {};
	for (const [key, value] of Object.entries(req.body)) {
		if ( (value.volume > 0) &&
			 (value.intensity > 0) ) {
			g_scape.effects[key] = value;
		}
	}

	scape_flush(g_scape);

	// Todo: update sound players

	res.send("OK")
})

app.get('/audio/zones', (req, res) => {
	//console.log(' endpoint audio zones called ');
	var ret = {}
	ret.names = config.zones.names;
	for (const [key, value] of Object.entries(g_scape.zones)) {
		if (value.volume > 0) {
			ret[key] = value;
		}
	}

	res.send(ret);
})

// set zones only through the put scape stuff

app.get('/audio/sinks', (req, res) => {
	var ret = {}
	ret.names = config.sinks.names;
	for (const [key, value] of Object.entries(g_sink)) {
		if (value.volume > 0) {
			ret[key] = value;
		}
	}
	res.send(ret);
})

app.put('/audio/sinks', (req, res) => {
	console.log('got a PUT sinks request');
	console.log(req.body);

	// Validate input
	for (const [key, value] of Object.entries(req.body)) {
		console.log(' validating %s object %s ',key,JSON.stringify(value));
		// is the name valid
		if (config.sinks.names.includes(key) == false) {
			res.status(400);
			res.send(key + ' is not a supported name')
			return;
		}
		if (value.hasOwnProperty('volume') == false) {
			res.status(400);
			res.send(key + ' must have volume')
			return;
		}
		if ((value.volume > 100) || (value.volume < 0)) {
			res.status(400);
			res.send(key + ' volume out of range');
			return;
		}
	}

	// valid, replace
	for (const [key, value] of Object.entries(req.body)) {
		g_sink[key] = value;
	}

	sink_flush(g_scape);

	// Todo: update sound players

	res.send("OK")
})

app.get('/', (req, res) => res.send('Hello World from SoundAdmin!'))

app.listen(port, () => {
	console.log(`SoundAdmin listening on port ${port}`)
})

