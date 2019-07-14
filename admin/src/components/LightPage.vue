<template>
    <div class="lights-page container">
        <div class="jars-section">
            <div class="jars-header row">
                <h2>⚱️Jars</h2>
                <a href="#">view color palette sources</a>
                <button>Synchronize Jars</button>
            </div>
            <div class="jars-content row">
                <JarLightingWidget bugName="Metric" class="col-4" />
                <JarLightingWidget bugName="Brazen" class="col-4" />
                <JarLightingWidget bugName="John" class="col-4" />
            </div>
        </div>
        <div class="small-bugs-section">
            <div class="small-bugs-header row">
                <h2>Small Bugs</h2>
                <button>Synchronize Jars</button>
            </div>
            <div class="small-bugs-content row">
                <div class="swarm-lighting-widget">
                    <h5>Swarm One</h5>
                    <label>Off/On</label>
                    <input type="checkbox"/>
                    <label for="swarmBrightness">Set Swarm Brightness:</label>
                    <input type="range" name="swarmBrightness" id="swarmBrightness" />
                    <label>Select a Light Program:</label>
                    <select class="custom-select">a
                        <option value="0" selected>matingDance2</option>
                    </select>
                </div>
            </div>
        </div>
        <div class="extra-lights-section">
            <label>👀Big Bug Eyes</label>
            <input type="checkbox"/>
            <label for="benchBrightness">💺 Set Bench Brightness:</label>
            <input type="range" name="benchBrightness" id="benchBrightness" />
        </div>
        <button>💡Apply Changes</button>
    </div>
</template>

<script>
import JarLightingWidget from './JarLightingWidget.vue';

export default {
    components: {
        JarLightingWidget
    }
};
</script>

<style>
</style>

