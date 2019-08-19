<template>
    <div class="balance-control">
        <div class="slider-container">
            <vue-slider :marks="marks" v-bind:value="value" v-on:change="onValueChanged" />
        </div>
        &nbsp; <slot/>
    </div>
</template>

<script>
    import VueSlider from 'vue-slider-component'
    import 'vue-slider-component/theme/default.css'

    export default {
        props: ['value', 'controllerId'],
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
                console.log('emitted input ', newValue);
            }
        }
    };
</script>

<style>
.balance-control {
    margin-top: 6px;
    margin-bottom: 32px;
}
.balance-control .slider-container {
    width: 60%;
    display: inline-block;
}

.vue-slider-marks {
    width: 92%;
}
.vue-slider-ltr .vue-slider-mark, .vue-slider-rtl .vue-slider-mark {
    top: 0;
}
</style>
