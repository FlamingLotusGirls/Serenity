<template>
    <div class="swarm-light-admin-widget container">
        <h5>Swarm {{swarmNumberString}}</h5>
        <div class="row">
            <div class="col-6">
                <label>Pick a Color:</label>
                <ColorPicker class="color-slider" color="#f00" v-model="selectedColor" />
            </div>
            <div class="col-4">
                <label for="blinkPattern">Set a Blink Pattern (tap to turn on or off)</label>
                <PatternToggleButton v-for="n in 12"></PatternToggleButton>
            </div>
        </div>
    </div>
</template>

<script>
import ColorPicker from './ColorPicker';
import PatternToggleButton from './PatternToggleButton';

export default {
    props: ['swarmNumberString'],
    data() {
        return {
            selectedColor: {}
        };
    },
    components: {
        ColorPicker,
        PatternToggleButton
    }
};
</script>

<style>
.swarm-light-admin-widget {
    padding-top: 16px;
    padding-bottom: 10px;
    border-bottom: 1px solid rgb(228, 228, 230);
}
.swarm-light-admin-widget label {
    display: block;
}
</style>
