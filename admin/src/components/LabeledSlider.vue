<template>
    <div class="labeled-slider">
        <div class="labels">
            <div class="slider-start-aligned slider-label">{{min}}</div>
            <div class="slider-end-aligned slider-label">{{max}}</div>
        </div>
        <CustomSlider v-bind:min="min" v-bind:max="max" v-bind:value="value" v-on:input="$emit('input', $event.target.value)" class="labeled-slider-input" />
    </div>
</template>

<script>
    import CustomSlider from 'vue-custom-range-slider';
    import 'vue-custom-range-slider/dist/vue-custom-range-slider.css';

    export default {
        props: ['value', 'min', 'max'],
        components: {
            CustomSlider
        }
    };
</script>

<style>
.labeled-slider {
    margin-top: 6px;
}
.labels, .labeled-slider-input {
    width: 100%;
    position: relative;
}

.slider-start-aligned {
    float: left;
    left: 4px;
}
.slider-end-aligned {
    float: right;
    right: 4px;
}

.slider-label {
    display: inline-block;
    height: auto;
}
.slider {
    margin: 0;
}
</style>
