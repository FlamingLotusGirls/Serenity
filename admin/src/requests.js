import { fireControllerURL, smallFireflyLEDControllerURL } from './appConfig';

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

export {
    getFireProgramNameList,
    runFireProgram,
    setFireflyLEDs
};