<template>
    <div class="swarm-lighting-widget">
        <!-- inner div is just a hack so our borders can go inside of the Bootstrap grid gutters -->
        <div class="swarm-lighting-widget-inner">
            <h5>Swarm {{swarmNumberString}}</h5>
            <div>
                <label>Pick a Color:</label>
                <ColorPicker class="color-slider" v-model="selectedColor" v-on:input="saveSwarmSettings" />
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
import { getFireflyLEDs, setFireflyLEDs } from '../requests';
import ColorPicker from './ColorPicker';
import PatternToggleSet from './PatternToggleSet';

// TODO: put this into a util file so it can be shared
const hexToRgb = function(hex) {
  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
};

// TODO: put this into a util file so it can be shared
const zeroPad = function(number, width) {
    var padded = number + '';
    while (padded.length < width) {
        padded = '0' + padded;
    }
    return padded;
};

// TODO: put this into a util file so it can be shared
const rgbToHex = function(r, g, b) {
    console.log('rgb to hex: ', r, g, b);
    const componentToHex = (component) => {
        return zeroPad(Math.round(component).toString(16), 2);
    };

    return '#' + componentToHex(r) + componentToHex(g) + componentToHex(b);
};

export default {
    props: ['swarmNumber', 'swarmNumberString'],
    data() {
        return {
            selectedColor: '#ff0000',
            blinkPattern: new Array(10).fill(false)
        };
    },
    beforeMount() {
        // fetch current settings
        return getFireflyLEDs()
            .then((allSettings) => {
                console.log('got firefly LEDs result', allSettings);
                let swarmSettings = allSettings.find((settings) => {
                    return settings.board_id === this.swarmNumber;
                });

                this.selectedColor = rgbToHex(swarmSettings.color[0] * 255, swarmSettings.color[1] * 255, swarmSettings.color[2] * 255);
                console.log('this swarm settings are ', swarmSettings);
                console.log(`so selected color is ${this.selectedColor}`)
            }, error => {
                alert(error);
            });
    },
    components: {
        ColorPicker,
        PatternToggleSet
    },
    methods: {
        patternChanged(newPattern) {
            this.blinkPattern = newPattern;
            this.saveSwarmSettings();
        },
        saveSwarmSettings() {
            let rgbColor = hexToRgb(this.selectedColor);
            let sequence = this.blinkPattern.map(bool => bool ? 1 : 0).join('');

            return setFireflyLEDs(this.swarmNumber, sequence, rgbColor)
                .then(() => {
                    console.log(`Saved swarm LED settings`);
                }, error => {
                    alert(error);
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
</style>
