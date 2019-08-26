<template>
    <div class="lights-page container">
        <div class="jars-section">
            <div class="jars-header row">
                <div class="col-12 align-self-left">
                    <h2><img class="jars-icon" src="/images/firefly-jar.png"></img> Jars</h2>
                </div>
            </div>
            <div class="jars-content row">
                <JarLightingWidget bugName="Metric" jarIndex="0" class="col-4" />
                <JarLightingWidget bugName="John" jarIndex="1" class="col-4" />
                <JarLightingWidget bugName="Brazen" jarIndex="2" class="col-4" />
            </div>
        </div>
        <div class="small-bugs-section">
            <div class="small-bugs-header row">
                <div class="col-12 align-self-left">
                    <h2><img class="small-bugs-icon" src="/images/firefly.png"></img> Small Bugs</h2>
                </div>
            </div>
            <div class="small-bugs-content row">
                <SwarmLightingWidget v-bind:swarmNumber="1" swarmNumberString="One" class="col-4" />
                <SwarmLightingWidget v-bind:swarmNumber="2" swarmNumberString="Two" class="col-4" />
                <SwarmLightingWidget v-bind:swarmNumber="3" swarmNumberString="Three" class="col-4" />
            </div>
        </div>
    </div>
</template>

<script>
import JarLightingWidget from './JarLightingWidget.vue';
import SwarmLightingWidget from './SwarmLightingWidget.vue';
import { ToggleButton } from 'vue-js-toggle-button';

export default {
    beforeMount() {
      // REST request to get current LED status goes here
    },
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

