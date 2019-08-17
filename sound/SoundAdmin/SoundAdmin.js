
const fs = require('fs');

const express = require('express');
const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true })); // I think this means we support that too

const port = 3000;

// https://codeburst.io/node-js-best-practices-smarter-ways-to-manage-config-files-and-variables-893eef56cbef
const config = require('./data/config.json')

var g_scape = null;



// Read from the default file
function scape_init() {
	// first try to load the last thing that was set
	console.log('scape init');
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
	fs.writeFileSync(config.currentScape, JSON.stringify(g_scape));
	console.log(' flushed current scape ');
}

// have a JSON block with new effect volume level
//function scape_effects_apply(s, e) {
//}

// Receive a new scape, find the diffs, leave the old


// Init function, load the scape
g_scape = scape_init();


//
// Endpoints
//


// 
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
		res.send('name is not a supported value');
		return;
	}

	g_scape.background.name = req.body.name;
	g_scape.background.volume = req.body.volume;

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


app.get('/audio/zones', (req, res) => {
	console.log(' endpoint audio zones called ');
	res.json(g_scape.zones);
})

// set zones only through the put scape stuff

app.get('/audio/sinks', (req, res) => {
	res.json(config.sinks)
})

app.put('/audio/sinks', (req, res) => {
	res.send('Got a PUT audio sinks request')
})

app.get('/', (req, res) => res.send('Hello World from SoundAdmin!'))

app.listen(port, () => {
	console.log(`SoundAdmin listening on port ${port}`)
})

