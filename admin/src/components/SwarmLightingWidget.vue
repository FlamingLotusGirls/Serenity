<template>
    <div class="swarm-lighting-widget">
        <!-- inner div is just a hack so our borders can go inside of the Bootstrap grid gutters -->
        <div class="swarm-lighting-widget-inner">
            <h5>Swarm {{swarmNumberString}}</h5>
            <div class="master-switch">
                <label>Master Lighting Switch</label>
                <ToggleButton labels />
            </div>
            <label for="swarmBrightness">Set Swarm Brightness:</label>
            <input type="range" name="swarmBrightness" class="swarm-brightness-control" id="swarmBrightness" />
            <label>Select a Light Program:</label>
            <select class="custom-select">a
                <option value="0" selected>matingDance2</option>
            </select>
        </div>
    </div>
</template>

<script>
import { ToggleButton } from 'vue-js-toggle-button';

export default {
    props: ['swarmNumberString'],
    components: {
        ToggleButton
    }
};
</script>

<style>
.swarm-lighting-widget-inner {
    border: 1px solid #979797;
    border-radius: 3px;
    padding: 15px;
}
.swarm-lighting-widget .master-switch label {
    margin-right: 8px;
}
.swarm-lighting-widget .custom-select, .swarm-lighting-widget input, .swarm-lighting-widget .master-switch {
    margin-bottom: 12px;
}
.swarm-lighting-widget label[for="swarmBrightness"] {
    display: block;
}
.swarm-brightness-control {
    width: 100%;
}
</style>
