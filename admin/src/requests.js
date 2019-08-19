import { fireControllerURL, smallFireflyLEDControllerURL, jarLEDControllerURL, soundControllerURL } from './appConfig';

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

/* JAR LED requests */

const getJarLEDPatternLists = function() {
    return new Promise(function(resolve, reject) {
        fetch(`${jarLEDControllerURL}/jar_leds/patterns`)
            .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to fetch list of jar LED patterns. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
            })
            .then(res => res.json())
            .then(result => {
                return resolve(result);
            }, error => {
                // triggers only on network errors, not unsuccessful responses
                return reject(`Failed to fetch jar LED pattern list with error ${error}`);
            });
    });
};

const getJarLEDs = function() {
    return new Promise(function(resolve, reject) {
        return fetch(`${jarLEDControllerURL}/jar_leds`, {
            method: 'GET'
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to retrieve jar LED settings. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => res.json())
        .then(result => {
            return resolve(result);
        }, error => {
            return reject(`Failed to retrieve jar LED settings with error ${error}`);
        });
    });
};

const setJarLEDs = function(foregroundPatternName, backgroundPatternName, intensity) {
    let formData = new FormData();
    formData.append('foreground', foregroundPatternName);
    formData.append('background', backgroundPatternName);
    formData.append('intensity', intensity);

    return new Promise(function(resolve, reject) {
        return fetch(`${jarLEDControllerURL}/jar_leds`, {
            method: 'POST',
            body: formData
        })
        .then(res => {
            // handle non-success responses
            if (!res.ok) {
                return reject(`Unable to save jar LED settings. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
        })
        .then(res => {
            return resolve();
        }, error => {
            return reject(`Failed to save jar LED settings with error ${error}`);
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
            .then(sinks => {
                // if a soundscape has a sink with volume 0, it won't show up as an object prop
                // but will show up in the names array, so pull from there
                sinks.names.forEach(function(sinkName) {
                    if (typeof sinks[sinkName] === 'undefined') {
                        sinks[sinkName] = { volume: 0 };
                    }
                });

                // then get rid of sinks.names so we don't read it as an extra sink itself
                delete sinks['names'];

                return resolve(sinks);
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
            body: JSON.stringify(newSinkVolumes),
            headers: {
                'Content-Type': 'application/json'
            }
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
            .then(zones => {
                // if a soundscape has a zone with volume 0, it won't show up as an object prop
                // but will show up in the names array, so pull from there
                zones.names.forEach(function(zoneName) {
                    if (typeof zones[zoneName] === 'undefined') {
                        zones[zoneName] = { volume: 0 };
                    }
                });

                // then get rid of zones.names so we don't read it as an extra zone itself
                delete zones['names'];

                return resolve(zones);
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
            body: JSON.stringify(newEffectSettings),
            headers: {
                'Content-Type': 'application/json'
            }
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

const getAudioBackgroundNames = function() {
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
                return resolve(result.names);
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
            body: JSON.stringify(newBackgroundObj),
            headers: {
                'Content-Type': 'application/json'
            }
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
                return resolve(result.names);
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

const saveNewAudioSoundscape = function(name, soundscapeObject) {
    return new Promise(function(resolve, reject) {
        return fetch(`${soundControllerURL}/audio/soundscapes/${name}`, {
            method: 'POST',
            body: JSON.stringify(soundscapeObject),
            headers: {
                'Content-Type': 'application/json'
            }
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
            method: 'POST',
            headers: {
              'Content-Type': 'application/json'
            },
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
                },
                headers: {
                    'Content-Type': 'application/json'
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

    getJarLEDPatternLists,
    getJarLEDs,
    setJarLEDs,
    
    getAudioSinks,
    setAudioSinkVolumes,
    getAudioZones,
    getAudioEffects,
    setAudioEffectSettings,
    getAudioBackgroundNames,
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