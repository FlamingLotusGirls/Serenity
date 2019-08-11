const express = require('express')
const app = express()
const port = 3000
const fs = require('fs')

// https://codeburst.io/node-js-best-practices-smarter-ways-to-manage-config-files-and-variables-893eef56cbef
const config = require('./config.json')

app.get('/', (req, res) => res.send('Hello World from SoundAdmin!'))

app.listen(port, () => console.log(`example app listening on port ${port}!`))

