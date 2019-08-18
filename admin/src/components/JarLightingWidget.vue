<template>
    <div class="jar-lighting-widget">
        <!-- inner div is just a hack so our borders can go inside of the Bootstrap grid gutters -->
        <div class="jar-lighting-widget-inner">
            <label>Foreground jar pattern for {{bugName}}</label>
            <select class="custom-select" v-model="selectedForegroundPattern" v-on:change="saveJarSettings">
                <option v-for="patternName in foregroundPatternNames" v-bind:value="patternName">{{patternName}}</option>
            </select>
            <label>Background jar pattern for {{bugName}}</label>
            <select class="custom-select" v-model="selectedBackgroundPattern" v-on:change="saveJarSettings">
                <option v-for="patternName in backgroundPatternNames" v-bind:value="patternName">{{patternName}}</option>
            </select>
            <label for="jarIntensity">Set {{bugName}}'s Jar LED Intensity:</label>
            <input type="range" name="jarIntensity" class="jar-intensity-control" id="jarIntensity" min="0" max="1" step="0.01" v-model="intensity" v-on:change="saveJarSettings" />
        </div>
    </div>
</template>

<script>
import ColorPicker from './ColorPicker';
import { getJarLEDPatternLists, getJarLEDs, setJarLEDs } from '../requests';

export default {
    props: ['bugName'],
    beforeMount() {
        // get lists of jar LED patterns to show in the menus
        getJarLEDPatternLists()
            .then(lists => {
                this.backgroundPatternNames = lists.backgrounds.map(obj => obj.name);
                this.foregroundPatternNames = lists.foregrounds;
                console.log('background: ', this.foregroundPatternNames, 'foreground', this.foregroundPatternNames);
            }, error => {
                alert(error);
            });

        // fetch current jar LED settings
        getJarLEDs()
            .then((settings) => {
                this.selectedForegroundPattern = settings.foreground;
                this.selectedBackgroundPattern = settings.background;
                this.intensity = settings.intensity;
            }, error => {
                alert(error);
            });
    },
    data () {
        return {
            backgroundPatternNames: [],
            foregroundPatternNames: [],
            selectedForegroundPattern: null,
            selectedBackgroundPattern: null,
            intensity: 1
        };
    },
    methods: {
        saveJarSettings() {
            return setJarLEDs(this.selectedForegroundPattern, this.selectedBackgroundPattern, this.intensity)
                .then(() => {
                    console.log(`Saved jar LED settings`);
                }, error => {
                    alert(error);
                });
        }
    }
};
</script>

<style>
.jar-lighting-widget-inner {
    border: 1px solid #979797;
    border-radius: 3px;
    padding: 15px;
}
.jar-lighting-widget .custom-select {
    margin-bottom: 12px;
}
.jar-intensity-control {
    width: 100%;
}
</style>