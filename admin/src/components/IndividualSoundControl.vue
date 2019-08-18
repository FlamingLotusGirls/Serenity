<template>
    <div class="individual-sound-control">
        <SoundSnippetButton>{{effectName}}</SoundSnippetButton>
        <div class="slider-container">
            <VolumeSlider v-bind:value="volume" v-on:input="volumeUpdated" />
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
            volumeUpdated(newVolume) {
                console.log('new vol: ', newVolume);
                this.$emit('volume-updated', newVolume);
            }
        }
    };
</script>

<style>
.individual-sound-control .slider-container {
    width: 60%;
    display: inline-block;
}
</style>
