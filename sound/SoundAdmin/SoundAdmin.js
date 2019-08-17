
const fs = require('fs')

const express = require('express')
const app = express()
app.use(express.json())

const port = 3000

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
	console.log(s);
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



app.get('/audio/backgrounds', (req, res) => {
	res.json(config.backgrounds)
})

app.put('/audio/backgrounds', (req, res) => {
	res.send('Got a PUT audio backgrounds request')
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
	console.log(`example app listening on port ${port}!`)
	console.log(config.soundscapeDir)
})

