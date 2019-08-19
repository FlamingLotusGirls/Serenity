<template>
    <div class="soundscape-selector col-12">
        <label for="soundscapeName">Choose a Saved Soundscape to Load:</label>
        <select class="custom-select" name="soundscapeName" id="soundscapeName" v-model="selectedSoundscapeName">
            <option v-for="soundscapeName in soundscapeNames" v-bind:value="soundscapeName">{{soundscapeName}}</option>
        </select>
        <button type="submit" class="btn btn-primary" v-on:click="loadSoundscape">Load</button>
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
            getSoundscape(this.selectedSoundscapeName)
                .then(soundscape => {
                    this.$emit('load-soundscape', soundscape);
                    this.selectedSoundscapeName = null;
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
    margin-right: 16px;
}
</style>

