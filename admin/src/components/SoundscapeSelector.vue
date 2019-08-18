<template>
    <div class="soundscape-selector col-12">
        <label for="soundscapeName">Choose a Soundscape to Play:</label>
        <select class="custom-select" name="soundscapeName" id="soundscapeName" v-model="selectedSoundscapeName">
            <option v-for="soundscapeName in soundscapeNames" v-bind:value="soundscapeName">{{soundscapeName}}</option>
        </select>
    </div>
</template>

<script>
import { getSoundscapesList, getSoundscape } from '../requests';

export default {
    data: function() {
        return {
            soundscapeNames: [],
            selectedSoundscapeName: null
        };
    },
    beforeMount() {
      getSoundscapesList()
        .then(list => {
          this.soundscapeNames = list;
        }, error => {
          alert(error);
        });
    },
    components: {
    },
    methods: {
        loadSoundscape() {
            getSoundscape()
                .then(soundscape => {
                    console.log('loading soundscape ', soundscape);
                    this.$emit('load-soundscape', soundscape);
                }, error => {
                    alert(error);
                });
        }
    }
};
</script>

<style>
.soundscape-selector label {
    margin-right: 16px;
}
.soundscape-selector select {
    width: auto;
}
</style>

