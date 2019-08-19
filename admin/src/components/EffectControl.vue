<template>
    <div class="effect-control">
        <SoundSnippetButton v-bind:value="intensity" v-on:input="updateIntensity">{{effectName}}</SoundSnippetButton>
        <div class="slider-container">
            <VolumeSlider v-bind:value="volume" v-on:input="updateVolume" />
        </div>
    </div>
</template>

<script>
    import SoundSnippetButton from './SoundSnippetButton';
    import VolumeSlider from './VolumeSlider'

    export default {
        props: ['intensity', 'volume', 'effectName'],
        data() {
            return {
            }
        },
        components: {
            VolumeSlider,
            SoundSnippetButton
        },
        methods: {
            updateVolume(newVolume) {
                this.$emit('update-volume', { effectName: this.effectName, newValue: newVolume });
            },
            updateIntensity(newIntensity) {
                this.$emit('update-intensity', { effectName: this.effectName, newValue: newIntensity });
            }
        }
    };
</script>

<style>
.effect-control .slider-container {
    width: 50%;
    display: inline-block;
}
</style>
