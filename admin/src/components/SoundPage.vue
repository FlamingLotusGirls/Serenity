<template>
    <div class="sound-page container">
        <div class="soundscape-row">
            <SoundscapeSelector class="col-12" v-on:load-soundscape="loadSoundscape"/>
        </div>
        <hr>
        <div class="main-section row">
            <div class="left-column col-4">
                <div class="master-volume-control">
                    <label for="masterVolume">Set Master Volume:</label>
                    <input type="range" name="masterVolume" id="masterVolume" v-model="soundscape.master.volume" />
                </div>
                <div class="zone-volume-controls">
                    <label>Set Zone Volume Controls:</label>
                    <BalanceControl v-for="(zone, zoneName) in soundscape.zones" v-model="zone.volume">{{zoneName}}</BalanceControl>
                </div>
            </div>
            <div class="right-columns col-8">
                <div class="ambient-tracks row">
                    <div class="col-6">
                        <label for="ambientTrackName">Select An Ambient Track:</label>
                        <select class="custom-select" name="ambientTrackName" id="ambientTrackName" v-model="soundscape.background.name">
                            <option v-for="backgroundName in backgroundNames" v-bind:value="backgroundName">{{backgroundName}}</option>
                        </select>
                    </div>
                    <div class="col-6">
                        <label for="ambientTrackName">Set Ambient Track Volume:</label>
                        <VolumeSlider v-model="soundscape.background.volume" />
                    </div>
                </div>
                <p>
                    Tap a button to activate a sound. Double tap to play moderate density of sound.  Triple tap to get maximum density of sound.  Volume can be set for each sound.
                </p>
                <p>
                    When activated, tap once to turn off. 
                </p>
                <label>Set individual sound densities and volumes</label>
                <div class="single-sound-buttons">
                    <EffectControl
                        v-for="(effectSettings, effectName) in soundscape.effects"
                        v-bind:effectName="effectName"
                        v-bind:volume="effectSettings.volume"
                        v-bind:intensity="effectSettings.intensity"
                        v-on:update-volume="updateVolume"
                        v-on:update-intensity="updateIntensity"
                        />
                </div>
            </div>
        </div>
        <hr>
        <div class="save-soundscape-section row">
            <input type="text" class="form-control" placeholder="Enter a unique name" v-model="newSoundscapeName" >
            <button class="btn btn-primary" type="submit" v-on:click="saveNewSoundscape">Save This Soundscape</button>
        </div>
    </div>
</template>

<script>
import { getAudioZones, getCurrentSoundscape, setCurrentSoundscapeSettings, getAudioBackgroundNames, saveNewAudioSoundscape } from '../requests';
import BalanceControl from './BalanceControl';
import SoundSnippetButton from './SoundSnippetButton';
import SoundscapeSelector from './SoundscapeSelector';
import VolumeSlider from './VolumeSlider';
import EffectControl from './EffectControl';

export default {
    data: function() {
        return {
            backgroundNames: [],
            soundscape: {
                background: {
                    name: 'Ambient2',
                    volume: 100
                },
                effects: {},
                zones: {},
                master: {
                    volume: 100
                }
            },
            newSoundscapeName: ''
        };
    },
    beforeMount() {
        getCurrentSoundscape()
            .then(soundscape => {
                    this.soundscape = soundscape;
                }, error => {
                    alert(error);
                });

        getAudioBackgroundNames()
            .then(names => {
                    this.backgroundNames = names;
                }, error => {
                    alert(error);
                });
    },
    components: {
        BalanceControl,
        SoundSnippetButton,
        SoundscapeSelector,
        VolumeSlider,
        EffectControl
    },
    methods: {
        loadSoundscape(soundscape) {
            console.log('Loaded soundscape: ', soundscape);
            this.soundscape = soundscape;
        },
        saveNewSoundscape() {
            saveNewAudioSoundscape(this.newSoundscapeName, this.soundscape)
                .then(zones => {
                        alert(`Saved new soundscape ${this.newSoundscapeName} successfully`);
                    }, error => {
                        alert(error);
                    });
        },
        updateVolume(updateObj) {
            this.soundscape.effects[updateObj.effectName].volume = updateObj.newValue;
        },
        updateIntensity(updateObj) {
            this.soundscape.effects[updateObj.effectName].intensity = updateObj.newValue;
        }
    },
    watch: {
        soundscape: {
            handler() {
                // save current soundscape changes to the server
                setCurrentSoundscapeSettings(this.soundscape)
                    .then(zones => {
                        console.log('Updated current soundscape successfully: ' + JSON.stringify(this.soundscape));
                    }, error => {
                        alert(error);
                    });
            },
            deep: true
        }
    }
};
</script>

<style>
.bottom-section {
    margin: 40px 0;
}
.bottom-section p {
    font-weight: bold;
}
.sound-page .effect-control {
    width: 48%;
    margin: 1%;
    display: inline-block;
}
.save-soundscape-section {
    margin-top: 40px;
}
.save-soundscape-section input {
    margin-right: 20px;
}
</style>

