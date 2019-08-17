
const fs = require('fs')

const express = require('express')
const app = express()
app.use(express.json())

const port = 3000

// https://codeburst.io/node-js-best-practices-smarter-ways-to-manage-config-files-and-variables-893eef56cbef
const config = require('./config.json')

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
	res.json(config.zones)
})

app.put('/audio/zones', (req, res) => {
	res.send('Got a PUT audio zones request')
})

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

