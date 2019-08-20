<template>
    <vue-slider :marks="sliderMarks" v-bind:min="sliderMin" v-bind:max="sliderMax" :duration="0.25" v-bind:value="value" v-on:change="onValueChanged" />
</template>

<script>
    import VueSlider from 'vue-slider-component'
    import 'vue-slider-component/theme/default.css'

    export default {
        props: ['value', 'marks', 'min', 'max'],
        data() {
            return {
                sliderMarks: this.marks || [0, 100],
                sliderMin: this.min || 0,
                sliderMax: this.max || 100
            }
        },
        components: {
            VueSlider
        },
        methods: {
            onValueChanged(newValue) {
                this.$emit('input', newValue)
            }
        }
    };
</script>

<style>
.vue-slider-marks {
    width: 92%;
}
.vue-slider-ltr .vue-slider-mark, .vue-slider-rtl .vue-slider-mark {
    top: 0;
}
</style>
