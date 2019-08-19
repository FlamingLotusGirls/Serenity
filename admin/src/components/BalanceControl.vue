<template>
    <div class="balance-control">
        <div class="slider-container">
            <VolumeSlider v-bind:value="value" v-on:input="onInput" />
        </div>
        &nbsp; <slot/>
    </div>
</template>

<script>
    import VolumeSlider from './VolumeSlider';

    export default {
        props: ['value'],
        components: {
            VolumeSlider
        },
        methods: {
            onInput(newValue) {
                this.$emit('input', newValue)
            }
        }
    };
</script>

<style>
.balance-control {
    margin-top: 6px;
    margin-bottom: 32px;
    max-width: 800px
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
