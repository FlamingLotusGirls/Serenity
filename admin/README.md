# serenity-admin
Admin dashboard for Serenity, FLG's 2019 fire sculpture

## Background

This dashboard is intended to help FLGs running Serenity them control the installation including fire, light, sound.

It's intended to be accessed via the local WiFi network broadcasted by Serenity, using FLG devices including smartphones, tablets, or laptops.

It's written mostly in JavaScript, with a Node.js server and a JS client using Vue and Bootstrap.

## Setting Up

```
# Prerequisites: node 10.15.0 and yarn 0.18.1
git clone git://github.com/cstigler/serenity-admin.git
cd serenity-admin
yarn install
```

## Running for Development

`yarn serve`

Access at http://localhost:8080

## Building for Production

```
# Compiles static files for production and puts them in /dist
yarn build

# Runs the node server, which is currently just a static file server serving out of /dist
yarn start
```

## Developing

Where to start:
- `app.js` is the main entry point for the Node.js server (technically `bin/www` but we don't touch that)
- `public/` is all of our static files and will be served from the root `/`
- `public/js/main.js` is the main entry point for the JavaScript client
- `routes/` contains the entire route table for the admin web server
- `controllers/` contains all server-side controller logic for our API methods

## License

serenity-admin is &copy; 2019 Flaming Lotus Girls, and released under the MIT License.