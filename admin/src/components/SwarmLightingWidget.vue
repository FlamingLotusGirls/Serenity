<template>
    <div class="swarm-lighting-widget">
        <!-- inner div is just a hack so our borders can go inside of the Bootstrap grid gutters -->
        <div class="swarm-lighting-widget-inner">
            <h5>Swarm {{swarmNumberString}}</h5>
            <div>
                <label>Pick a Color:</label>
                <ColorPicker class="color-slider" color="#f00" v-model="selectedColor" v-on:input="saveSwarmSettings" />
            </div>
            <div>
                <label for="swarmBrightness">Set Brightness:</label>
                <input type="range" name="swarmBrightness" class="swarm-brightness-control" id="swarmBrightness" v-model="swarmBrightness" v-on:change="saveSwarmSettings" />
            </div>
            <div>
                <label for="blinkPattern">Set a Blink Pattern (tap to turn on or off)</label>
                <PatternToggleSet v-bind:patternLength="10" v-model="blinkPattern" v-on:input="saveSwarmSettings"></PatternToggleSet>
            </div>
            <!-- light programs don't exist currently
            <label>Select a Light Program:</label>
            <select class="custom-select">a
                <option value="0" selected>matingDance2</option>
            </select>
            -->
        </div>
    </div>
</template>

<script>
import { smallFireflyLEDControllerURL } from '../appConfig';
import ColorPicker from './ColorPicker';
import PatternToggleSet from './PatternToggleSet';

export default {
    props: ['swarmNumber', 'swarmNumberString'],
    data() {
        return {
            selectedColor: {},
            swarmBrightness: 100,
            blinkPattern: new Array(10).fill(false)
        };
    },
    components: {
        ColorPicker,
        PatternToggleSet
    },
    methods: {
        patternChanged(newPattern) {
            this.blinkPattern = newPattern;
            console.log('pattern changed', newPattern);
            this.saveSwarmSettings();
        },
        saveSwarmSettings() {
            console.log('saving swarm settings', this.selectedColor, this.swarmNumber);
            let formData = new FormData();
            formData.append('swarm', this.swarmNumber);
            formData.append('sequence', this.blinkPattern.map(bool => bool ? 1 : 0).join(''));

            return fetch(`${smallFireflyLEDControllerURL}/firefly_leds`, {
                method: 'POST',
                body: formData
            })
            .then(res => {
                // handle non-success responses
                if (!res.ok) {
                    alert(`Unable to save swarm LED settings. Request failed with status ${res.status} ${res.statusText}`);
                    return;
                }
                return res;
            })
            .then(res => {
                console.log(`Saved swarm LED settings`);
            }, error => {
                alert(`Failed to save swarm LED settings with error ${error}`);
            });
        }
    }
};
</script>

<style>
.swarm-lighting-widget-inner {
    border: 1px solid #979797;
    border-radius: 3px;
    padding: 15px;
}
.swarm-lighting-widget .custom-select, .swarm-lighting-widget input, .swarm-lighting-widget .master-switch {
    margin-bottom: 12px;
}
.swarm-lighting-widget label {
    display: block;
    margin-top: 6px;
}
.swarm-brightness-control {
    width: 100%;
}
</style>
