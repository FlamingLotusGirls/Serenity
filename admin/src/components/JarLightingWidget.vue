<template>
    <div class="jar-lighting-widget">
        <!-- inner div is just a hack so our borders can go inside of the Bootstrap grid gutters -->
        <div class="jar-lighting-widget-inner">
            <label>Set {{bugName}}'s Color Palette</label>
            <ColorPicker class="color-slider" v-model="selectedColor" />
            <label>Select a Jar Pattern for {{bugName}}</label>
            <select class="custom-select">
                <option value="0" selected>soothingFlow</option>
            </select>
        </div>
    </div>
</template>

<script>
import ColorPicker from './ColorPicker';

export default {
    props: ['bugName'],
    data () {
        return {
            selectedColor: '#ff0000'
        };
    },
    components: {
        ColorPicker
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
</style>