
const fs = require('fs');
const express = require('express');
const cors = require('cors');

const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true })); // I think this means we support that too
app.use(cors());

const port = 9000;

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
	//console.log(' flushed current scape ');
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
	//console.log(' flushed current sink ');
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

// res is there to throw errors
function background_put(bg, res) {

	if (bg.hasOwnProperty('volume') == false) {
		res.status(400);
		res.send('background must have a volume');
		return(false);
	}
	if ((bg.volume < 0) || (bg.volume > 100)) {
		res.status(400);
		res.send('volume '+bg.volume+' out of range');
		return(false);
	}
	// If volume is zero, ignore name
	if (bg.volume == 0) {
		bg.name = ''
	}
	else {
		if (bg.hasOwnProperty('name') == false) {
			res.status(400);
			res.send('background must have a name');
			return(false);
		}
		if (config.backgrounds.names.includes(bg.name) == false) {
			res.status(400);
			res.send(bg.name + ' is not a supported name');
			return(false);
		}
	}

	g_scape.background.name = bg.name;
	g_scape.background.volume = bg.volume;

	return(true);
}

app.put('/audio/background', (req, res) => {
	console.log('got a PUT audio background request');
	console.log(req.body);

	// side effect: sends error responses if failure
	if (false == background_put(req.body, res)) 
		return;

	// TODO: notify all players of new background

	scape_flush(g_scape);

	res.send(g_scape.background);
})

// Returns the current list of sounscapes as an array
app.get('/audio/soundscapes', (req, res) => {
	var files = fs.readdirSync(config.soundscapeDir);
	// This isn't preciese because I only want the trailing .json
	var ret = {}
	ret.names = files.map(str => str.replace('.json',''))
	res.send(ret);
})

// create a new soundscale with id 'id' using the current senttings
//if no object is passed in, or with the passed in object
app.post('/audio/soundscapes/:id', (req, res) => {
	var id = req.params.id;
	// if exists fail
	var fn = config.soundscapeDir+id+'.json';
	if (fs.existsSync(fn)) {
		res.status(400);
		res.send(id + ' already exists')
		return;
	}

	// if empty body use default
	if (Object.keys(req.body).length==0) {
		fs.writeFileSync(fn, JSON.stringify(g_scape));
		res.send(g_scape)
	}
	else {
		// TODO: should validate this
		fs.writeFileSync(fn, JSON.stringify(req.body));
		res.send(req.body)
	}

})

// Get any named one... and make it current or not?
app.get('/audio/soundscapes/:id', (req, res) => {
    const fileName = config.soundscapeDir + req.params.id + '.json';

    return fs.readFile(fileName, function(err, data) {
        if (err) {
            if (err.code === 'ENOENT') {
                res.status(404);
            } else {
                res.status(500);
            }

            return res.send(`Error reading soundscape file ${fileName}: ${err.message}`);
        }

        return res.send(data);
    });
})

// Delete the soundscape /audio/soundscapes/NAME
app.delete('/audio/soundscapes/:id', (req, res) => {
	var id = req.params.id;
	var fn = config.soundscapeDir+id+'.json';
	if (fs.existsSync(fn) == false) {
		res.status(400);
		res.send(id + ' does not exist')
		return;
	}
	fs.unlinkSync(fn)
	res.send('OK')
})

// Returns the current sounscape as a JSON object
app.get('/audio/soundscape', (req, res) => {
	// Maybe we want all the names in here. I don't think so though.
	//var ret = Object.assign({},g_scape)
	//ret.zones.names = config.zones.names
	//ret.background.names = config.backgrounds.names
	//ret.effects.names = config.effects.names
	//res.send(ret)
	res.send(g_scape);
})

// Update the current soundscape
app.post('/audio/soundscape', (req, res) => {
	if (req.body.background) {
		if (!background_put(req.body.background, res))
			return;
	}
	if (req.body.effects) {
		if (!effects_put(req.body.effects, res))
			return;
	}
	if (req.body.zones) {
		if (!zones_put(req.body.zones, res))
			return;
	}
	if (req.body.master) {
		var master = req.body.master;
		if ((master.volume > 100) || (master.volume < 0)) {
			res.status(400);
			res.send(key + ' volume out of range');
			return;
		}
		g_scape.master = master;
	}

	scape_flush(g_scape);

	res.send(g_scape)
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

function effects_put(eff, res) {

	// Validate input
	for (const [key, value] of Object.entries(eff)) {
		//console.log(' validating %s object %s ',key,JSON.stringify(value));
		// is the name valid
		if (config.effects.names.includes(key) == false) {
			res.status(400);
			res.send(key + ' is not a supported name')
			return(false);
		}
		if (value.hasOwnProperty('intensity') == false) {
			res.status(400);
			res.send(key + ' must have intensity')
			return(false);
		}
		if ((value.intensity > 3) || (value.intensity < 0)) {
			res.status(400);
			res.send(key + ' intensity out of range');
			return(false);
		}
		if (value.hasOwnProperty('volume') == false) {
			res.status(400);
			res.send(key + ' must have volume')
			return(false);
		}
		if ((value.volume > 100) || (value.volume < 0)) {
			res.status(400);
			res.send(key + ' volume out of range');
			return(false);
		}
	}

	// valid, replace
	for (const [key, value] of Object.entries(eff)) {
		if ((value.volume == 0) || (value.intensity == 0)) {
			value.volume = 0;
			value.intensity = 0;
		}
		g_scape.effects[key] = value;
	}
	return(true);

}

app.put('/audio/effects', (req,res) => {
	//console.log('got a PUT effects request');
	//console.log(req.body);

	if (effects_put(req.body, res) == false)
		return;

	scape_flush(g_scape);

	// Todo: update sound players

	res.send(g_scape.effects)
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

function zones_put(z, res) {

	// Validate input
	for (const [key, value] of Object.entries(z)) {
		//console.log(' validating %s object %s ',key,JSON.stringify(value));
		// is the name valid
		if (config.zones.names.includes(key) == false) {
			res.status(400);
			res.send(key + ' is not a supported name')
			return(false);
		}
		if (value.hasOwnProperty('volume') == false) {
			res.status(400);
			res.send(key + ' must have volume')
			return(false);
		}
		if ((value.volume > 100) || (value.volume < 0)) {
			res.status(400);
			res.send(key + ' volume out of range');
			return(false);
		}
	}

	// valid, replace
	for (const [key, value] of Object.entries(z)) {
		g_scape.zones[key] = value;
	}
	return(true);

}

app.get('/audio/master', (req,res) => {
	res.send( g_scape.master )
})

app.put('/audio/master', (req, res) => {
	//console.log(' setting the master object ')
	//console.log(req.body)
	var master = req.body

	if ((master.volume > 100) || (master.volume < 0)) {
		res.status(400);
		res.send(key + ' volume out of range');
		return(false);
	}

	g_scape.master = master;

	scape_flush(g_scape);

	res.send('OK')

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
	//console.log('got a PUT sinks request');
	//console.log(req.body);

	// Validate input
	for (const [key, value] of Object.entries(req.body)) {
		//console.log(' Sinks: validating %s object %s ',key,JSON.stringify(value));
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

	sink_flush(g_sink);

	// Todo: update sound players

	res.send(g_sink)
})


// This is maybe a little weird.
// There are 6 pairs of buttons on the sculpture
// 1/1 and 1/2 ( which is a pair )
// through
// 6/1 and 6/2
// When one gets hit, call this endpoint
//
// You don't have to put any data, putting to this means that button was hit
//

app.put('/audio/buttons/:bgroup/:button', (req, res) => {
	var bgroup = req.params.bgroup;
	var button = req.params.button;

	// look up in Config

	if (config.buttons.hasOwnProperty(bgroup) == false) {
		res.status(400);
		res.send('button group '+bgroup+' not defined in config');
		return(false);
	}
	if ((button != "0") && (button != "1")) {
		res.status(400);
		res.send('button '+button+' not defined, only 0 and 1 allowed');
		return(false);
	}

	var effect = config.buttons[bgroup].effect;

	// off button
	if (button == "0") {
		g_scape.effects[effect].intensity = 0
	}
	else if ( button == "1") {
		g_scape.effects[effect].intensity++ ;
		if (g_scape.effects[effect].intensity > 3)
			g_scape.effects[effect].intensity = 3;
		if (g_scape.effects[effect].volume == 0) {
			g_scape.effects[effect].volume = config.buttons[bgroup].volume;
		}
	}

	//console.log(g_scape.effects[effect])

	scape_flush(g_scape);

	res.send('OK')
})

app.get('/', (req, res) => res.send('Hello World from SoundAdmin!'))

app.listen(port, () => {
	console.log(`SoundAdmin listening on port ${port}`)
})
