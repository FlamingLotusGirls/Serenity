// Sound effects + volume

exports.playSoundEffect = function(req, res) {
    // req.body.controllerId = sound controller ID, req.body.newState = 0 or 1 (on or off)
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
 
exports.getSoundEffects = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
 
exports.setVolumeLevel = function(req, res) {  
    // req.body.volumeLevel = new volume level, between 0 and 1  
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
 
// Volume presets (saved settings)
 
// save current settings for volume as a preset under a tag
exports.saveNewVolumePreset = function(req, res) {
    // req.body.swarmId = swarm type to modify 
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
 
exports.deleteVolumePreset = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.getVolumePresetList = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.getVolumePreset = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

// apply specified volume settings
exports.applyVolumePreset = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
