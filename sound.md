# Sound API design

## High-level description

(from cswales)

> In terms of what the sound system can do - each controller has two different
> output channels that can be controlled independently. In each channel we can
> mix together a set of sound effects, and those effects can be layered on top
> of one another. The button box that Ted showed me had a button for 'add more
> sound effect X' and another for 'stop all sound effect X'. Thinking at a
> design level, I'd like to have a certain background noise/drone/effects that
> the participants *cant* change, and then ideally some set of buttons like
> Ted's that the participants can use to add additional sounds on top of the
> ambient. That says that our admin app definitely needs to be able to modify,
> save, and restore the ambient, and probably needs to be able to have some
> control over the effect overlays. Which of course also says that the APIs I
> tossed out there are hopelessly inadequate. Anyone else want to take a crack
> at this functionality?

## API proposals

### sound effects

- `GET /sound/controllers/`
- `GET /sound/controller/<soundControllerId>/channels`
- `GET /sound/controller/<soundControllerId>/channel/<channelId>/ambients/`
- `GET /sound/controller/<soundControllerId>/channel/<channelId>/effects/`
    - these are both /sound/effects, and this _could_ be handled by returning them with metadata as to if they're loaded into the "effects" layer or the "ambients" layer
- `GET /sound/effects/`
- `PUT /sound/controller/<soundControllerId>/channel/<channelId>/{ambients,effects}/<soundEffectId> volume=<0-100>`
    - volume 0 deletes the sound from playing on the controller, could be a DELETE instead?
- `POST /sound/controller/<soundControllerId>/[channel/<channelId>/]{ambients,effects} relativeVolume=<-100-100>
    - adjust the volume, capped at 0/100, of all sound effects on a given controller's (and optionally channel's) {ambients,effects} track by the given amount
- `PUT /sound/controllers/{ambients,effects}/<soundEffectId> relativeVolume=<-100-100>`
    - adjust the volume, capped at 0/100, of a given sound effect on all controllers {ambients,effects} tracks by the given amount
    - unclear to me if this should be a backend doing "query every controller+channel, find all the controllers with this effect, send a manually updated volume to each" or "send every controller an 'adjust volume appropriately' message"
- `DELETE /sound/controllers/{ambients,effects}/<soundEffectId>`
    - remove `soundEffectId` from all {ambient,effect} controller tracks
- `DELETE /sound/controller/<soundControllerId>/channel/<channelId>/{ambients,effects}`
    - remove all sound effects from a given controller/channel/track combo
    - useful to also have "from controller X / *all* channels / track Y"?
- 

### presets

- `GET /sound/presets/`
- `GET /sound/presets/<presetId>`
- `PUT /sound/controller/<soundControllerId>/preset/<presetId>`
    - this applies a single-controller preset OR applies only that controller's preset from a multi-controller preset
- `PUT /sound/controllers/preset/<presetId>`
- `PUT /sound/presets [controllerId=X] [track={ambients,effects}] name=presetName`
- `DELETE /sound/preset/<presetId>`
