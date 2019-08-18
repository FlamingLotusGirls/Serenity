import { fireControllerURL, smallFireflyLEDControllerURL, soundControllerURL } from './appConfig';

/* FIRE Requests */

const getFireProgramNameList = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${fireControllerURL}/flame/patterns`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch list of fire programs. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                let filteredProgramNames = result.map(fireProgram => fireProgram.name).filter(name => {
                    // the single poofers are set up as fire sequences with names starting with __
                    // so we should filter all of those out so as not to confuse the user
                    return !name.startsWith('__');
                });
                return resolve(filteredProgramNames);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch fire program list with error ${error}`);
            });
    });
};

const runFireProgram = function(programName) {
    let formData = new FormData();
    formData.append('active', 'true');

    return new Promise(function(resolve, reject) {
        fetch(`${fireControllerURL}/flame/patterns/${programName}`, {
            method: 'POST',
            body: formData
        })
            .then(res => {
                // handle non-success responses
                if (!res.ok) {
                    return reject(`Unable to start fire. Request failed with status ${res.status} ${res.statusText}`);
                }
                return res;
            })
            .then(res => {
                return resolve();
            }, error => {
                return reject(`Failed to start fire with error ${error}`);
            });
    });
};

/* FIREFLY LED requests */

const getFireflyLEDs = function() {
    return new Promise(function(resolve, reject) {
        return fetch(`${smallFireflyLEDControllerURL}/firefly_leds`, {
            method: 'GET'
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to save swarm LED settings. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => res.json())
        .then(result => {
            return resolve(result);
        }, error => {
            return reject(`Failed to save swarm LED settings with error ${error}`);
        });
    });
};

const setFireflyLEDs = function(swarmNumber, sequence, colorObject) {
    let formData = new FormData();
    formData.append('swarm', swarmNumber);
    formData.append('sequence', sequence);
    formData.append('color', `${colorObject.r / 255.0},${colorObject.g / 255.0},${colorObject.b / 255.0}`);

    return new Promise(function(resolve, reject) {
        return fetch(`${smallFireflyLEDControllerURL}/firefly_leds`, {
            method: 'POST',
            body: formData
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to save swarm LED settings. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to save swarm LED settings with error ${error}`);
        });
    });
};

/* AUDIO requests */

const getAudioSinks = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/sinks`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch audio sinks. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch audio sinks with error ${error}`);
            });
    });
};

const setAudioSinkVolumes = function(newSinkVolumes) {
    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/sinks`, {
            method: 'PUT',
            body: JSON.stringify(newSinkVolumes)
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to save audio sink volumes. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to save audio sink volumes with error ${error}`);
        });
    });
};

const getAudioZones = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/zones`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch audio zones. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch audio zones with error ${error}`);
            });
    });
};

const getAudioEffects = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/effects`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch audio effects. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch audio effects with error ${error}`);
            });
    });
};

const setAudioEffectSettings = function(newEffectSettings) {
    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/effects`, {
            method: 'PUT',
            body: JSON.stringify(newEffectSettings)
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to save audio effect settings. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to save audio effect settings with error ${error}`);
        });
    });
};

const getAudioBackgrounds = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/backgrounds`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch audio backgrounds. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch audio backgrounds with error ${error}`);
            });
    });
};

const getCurrentAudioBackground = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/background`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch current audio background. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch current audio background with error ${error}`);
            });
    });
};

const setAudioBackground = function(newBackgroundObj) {
    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/background`, {
            method: 'PUT',
            body: JSON.stringify(newBackgroundObj)
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to save audio background. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to save audio background with error ${error}`);
        });
    });
};

const getSoundscapesList = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/soundscapes`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch list of soundscapes. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch list of soundscapes with error ${error}`);
            });
    });
};

const getSoundscape = function(name) {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/soundscapes/${name}`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch soundscape ${name}. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch soundscape ${name} with error ${error}`);
            });
    });
};

const deleteAudioSoundscape = function(name) {
    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/soundscapes/${name}`, {
            method: 'DELETE'
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to delete soundscape ${name}. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to delete soundscape ${name} with error ${error}`);
        });
    });
};

const saveNewAudioSoundscape = function(soundscapeObject) {
    const name = soundscapeObject.name;

    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/soundscapes/${name}`, {
            method: 'POST',
            body: JSON.stringify(soundscapeObject)
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to create soundscape ${name}. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to create soundscape ${name} with error ${error}`);
        });
    });
};

const getCurrentSoundscape = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/soundscape`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch current soundscape. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch current soundscape with error ${error}`);
            });
    });
};

const setCurrentSoundscapeSettings = function(newSettings) {
    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/soundscape`, {
            method: 'PUT',
            body: JSON.stringify(newSettings)
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to update current soundscape settings. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to update current soundscape settings with error ${error}`);
        });
    });
};

const getDefaultSoundscapeId = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/soundscapes/default`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch default soundscape. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch default soundscape with error ${error}`);
            });
    });
};

// "Brian is not so sure about the idea of resetting the default. The default should be baked in. Let's talk."
const setDefaultSoundscapeId = function(newDefaultId) {
    return new Promise(function(resolve, reject) {
        fetch(`${soundControllerURL}/audio/soundscapes/default`, {
                method: 'POST',
                body: {
                    default: newDefaultId
                }
            })
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to update default soundscape. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to update default soundscape with error ${error}`);
            });
    });
};

export {
    getFireProgramNameList,
    runFireProgram,

    getFireflyLEDs,
    setFireflyLEDs,
    
    getAudioSinks,
    setAudioSinkVolumes,
    getAudioZones,
    getAudioEffects,
    setAudioEffectSettings,
    getAudioBackgrounds,
    getCurrentAudioBackground,
    setAudioBackground,
    getSoundscapesList,
    getSoundscape,
    deleteAudioSoundscape,
    saveNewAudioSoundscape,
    getCurrentSoundscape,
    setCurrentSoundscapeSettings,
    getDefaultSoundscapeId,
    setDefaultSoundscapeId
};