<template>
    <div class="jar-lighting-widget">
        <!-- inner div is just a hack so our borders can go inside of the Bootstrap grid gutters -->
        <div class="jar-lighting-widget-inner">
            <label>Set {{bugName}}'s Color Palette</label>
            <ColorSlider />
            <label>Select a Jar Pattern for {{bugName}}</label>
            <select class="custom-select">
                <option value="0" selected>soothingFlow</option>
            </select>
            <label for="jarBrightness">Set {{bugName}}'s Jar Brightness:</label>
            <input type="range" name="jarBrightness" class="jar-brightness-selector" id="jarBrightness" />
        </div>
    </div>
</template>

<script>
import { Slider } from 'vue-color';

export default {
    props: ['bugName'],
    components: {
        ColorSlider: Slider
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
.jar-brightness-selector {
    width: 100%;
}
</style>