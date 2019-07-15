<template>
    <div class="lights-page container">
        <div class="jars-section">
            <div class="jars-header row">
                <div class="col-8 align-self-left">
                    <h2><img class="jars-icon" src="/images/firefly-jar.png"></img> Jars</h2>
                    <a class="view-color-sources-link" href="#">view color palette sources</a>
                </div>
                <div class="col-4 align-self-right synchronize-column">
                    <button class="btn btn-primary">Synchronize Jars</button>
                </div>
            </div>
            <div class="jars-content row">
                <JarLightingWidget bugName="Metric" class="col-4" />
                <JarLightingWidget bugName="Brazen" class="col-4" />
                <JarLightingWidget bugName="John" class="col-4" />
            </div>
        </div>
        <div class="small-bugs-section">
            <div class="small-bugs-header row">
                <div class="col-8 align-self-left">
                    <h2><img class="small-bugs-icon" src="/images/firefly.png"></img> Small Bugs</h2>
                </div>
                <div class="col-4 align-self-right synchronize-column">
                    <button class="btn btn-primary">Synchronize Jars</button>
                </div>
            </div>
            <div class="small-bugs-content row">
                <SwarmLightingWidget swarmNumberString="One" class="col-3" />
                <SwarmLightingWidget swarmNumberString="Two" class="col-3" />
                <SwarmLightingWidget swarmNumberString="Three" class="col-3" />
                <SwarmLightingWidget swarmNumberString="Four" class="col-3" />
            </div>
        </div>
        <div class="extra-lights-section row">
            <div class="col-6">
                <label><span class="icon">👀</span> Big Bug Eyes</label>
                <ToggleButton labels="true" />
            </div>
            <div class="col-6">
                <label for="benchBrightness"><span class="icon">💺</span> Set Bench Brightness:</label>
                <input type="range" name="benchBrightness" id="benchBrightness" />
            </div>
        </div>
        <button class="btn btn-primary apply-changes-button">💡Apply Changes</button>
    </div>
</template>

<script>
import JarLightingWidget from './JarLightingWidget.vue';
import SwarmLightingWidget from './SwarmLightingWidget.vue';
import { ToggleButton } from 'vue-js-toggle-button';

export default {
    components: {
        JarLightingWidget,
        SwarmLightingWidget,
        ToggleButton
    }
};
</script>

<style>
.jars-icon {
    width: 43px;
    height: 63px;
}
.small-bugs-icon {
    width: 26px;
    height: 50px;
}
.view-color-sources-link {
    margin-top: 20px;
    margin-left: 40px;
}
.synchronize-column {
    text-align: right;
}
.lights-page h2 {
    display: inline-block;
}
.jars-section {
    margin-bottom: 40px;
}
.extra-lights-section {
    margin-top: 24px;
}
.extra-lights-section label {
    font-size: 16px;
    margin-right: 8px;
}
.extra-lights-section .icon {
    font-size: 28px;
}
.apply-changes-button {
    float: right;
}
</style>

