var express = require('express');
var router = express.Router();
let fireController = require('../controllers/api/fire');
let lightController = require('../controllers/api/light');
let soundController = require('../controllers/api/sound');

/* API Methods */

// FIRE methods

router.put('/flames/sequences/:sequenceName', fireController.playFireSequence);

router.put('/flames/stop', fireController.stopFire);

router.put('/flames/poofers/:pooferId', fireController.poof);

router.post('/flames/sequences/:sequenceName', fireController.createFireSequence);

router.get('/flames/sequences', fireController.getFireSequences);

router.get('/flames/poofers', fireController.getPooferStates);

router.delete('/flames/sequences/:sequenceName', fireController.deleteFireSequence);

// Jar Light routes

router.put('/light/jar/patterns/:patternId', lightController.playJarLightPattern);

router.get('/light/jar/patterns', lightController.getJarLightPatterns);

router.get('/light/jar/images', lightController.getJarLightImages);

router.post('/light/jar/images', lightController.addJarLightImage);

router.delete('/light/jar/images/:imageId', lightController.deleteJarLightImage);

// Firefly Light Routes

router.put('/light/fireflies/interval/:interval', lightController.setFireflyBlinkInterval);

router.put('/light/fireflies/color/:color', lightController.setFireflyLightColor);

// SOUND routes

router.put('/sound/effects/:soundEffectId', soundController.playSoundEffect);

router.get('/sound/effects', soundController.getSoundEffects);

router.put('/sound/:controllerId/volume', soundController.setVolumeLevel);

router.post('/sound/presets/:presetId', soundController.saveNewVolumePreset);

router.delete('/sound/presets/:presetId', soundController.deleteVolumePreset);

router.get('/sound/presets', soundController.getVolumePresetList);

router.get('/sound/presets/:presetId', soundController.getVolumePreset);

router.put('/sound/presets/:presetId', soundController.applyVolumePreset);

module.exports = router;
