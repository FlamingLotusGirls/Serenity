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
                    <input type="range" name="masterVolume" id="masterVolume" />
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
                    <IndividualSoundControl v-for="(effectSettings, effectName) in soundscape.effects" v-bind:effectName="effectName" v-bind:volume="effectSettings.volume" v-bind:intensity="effectSettings.intensity" />
                </div>
            </div>
        </div>
        <div class="save-soundscape-section row">
            <input type="text" placeholder="Enter a unique name">
            <button class="btn btn-primary" type="submit">Save This Soundscape</button>
        </div>
    </div>
</template>

<script>
import { getAudioZones, getCurrentSoundscape, setCurrentSoundscapeSettings, getAudioBackgroundNames } from '../requests';
import BalanceControl from './BalanceControl';
import SoundSnippetButton from './SoundSnippetButton';
import SoundscapeSelector from './SoundscapeSelector';
import VolumeSlider from './VolumeSlider';
import IndividualSoundControl from './IndividualSoundControl';

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
                zones: {}
            }
        };
    },
    beforeMount() {
        getCurrentSoundscape()
            .then(soundscape => {
                    console.log('retrieved soundscape', soundscape);
                    this.soundscape = soundscape;
                }, error => {
                    alert(error);
                });

        getAudioBackgroundNames()
            .then(names => {
                    console.log('retrieved backrounds', names);
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
        IndividualSoundControl
    },
    methods: {
        loadSoundscape(soundscape) {
            console.log('Loaded soundscape: ', soundscape);
            this.soundscape = soundscape;
        }
    },
    watch: {
        soundscape: {
            handler() {
                // save soundscape to the server
                setCurrentSoundscapeSettings(this.soundscape)
                    .then(zones => {
                        console.log('Saved soundscape successfully: ' + JSON.stringify(this.soundscape));
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
.sound-page .individual-sound-control {
    width: 48%;
    margin: 1%;
    display: inline-block;
}
</style>

