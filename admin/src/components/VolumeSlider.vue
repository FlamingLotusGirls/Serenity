<template>
    <vue-slider :marks="marks" v-bind:value="value" v-on:change="onValueChanged" />
</template>

<script>
    import VueSlider from 'vue-slider-component'
    import 'vue-slider-component/theme/default.css'

    export default {
        props: ['value'],
        data() {
            return {
                marks: [0, 100]
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
