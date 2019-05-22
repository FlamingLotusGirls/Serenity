// Jar stuff

exports.playJarLightPattern = function(req, res) {
   // req.body.bugId = ID of the bug to play the sequence on, req.body.imageId = image to use
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.getJarLightPatterns = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

// if we can use images. Images end up being a good way to grab a 
// color palette, even if we can't project the actual image on the the jar.
// See the Soma code for an example of this
exports.getJarLightImages = function(req, res) {    
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.addJarLightImage = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.deleteJarLightImage = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

// Firefly stuff

// set the blink interval (which probably ends up following a gaussian)
exports.setFireflyBlinkInterval = function(req, res) {
    // req.body.swarmId = swarm type to modify 
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.setFireflyLightColor = function(req, res) {
    // set the firefly light color
    // req.body.swarmId = swarm type to modify 
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
