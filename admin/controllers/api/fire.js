exports.playFireSequence = function(req, res) {
    // req.body.bugId = ID of the bug to play the sequence on
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.stopFire = function(req, res) {
    // req.body.bugId = ID of the bug to stop fire on
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.poof = function(req, res) {
    // options: req.body.on, req.body.duration, req.body.bugId
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.createFireSequence = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.getFireSequences = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.playFireSequence = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.getPooferStates = function(req, res) {
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};

exports.deleteFireSequence = function(req, res) {
    // don't allow deletion of built in all-poof and chase!
    return res.json({ status: 'error', message: 'Method not yet implemented' });
};
