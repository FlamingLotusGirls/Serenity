<template>
  <div class="pattern-toggle-set">
    <PatternToggleButton v-for="(patternValue, index) in currentPattern" v-bind:initiallyLit="patternValue" v-bind:patternIndex="index" v-on:toggle-state-changed="patternChanged"></PatternToggleButton>
  </div>
</template>

<script>
import PatternToggleButton from './PatternToggleButton';

export default {
  props: ['patternLength', 'value'],
  data() {
    return {
      currentPattern: this.value || new Array(this.patternLength).fill(false)
    }
  },
  beforeMount() {
    if (this.currentPattern && this.currentPattern.length !== this.patternLength) {
        console.log(`WTF? Pattern length should be ${this.patternLength} but our pattern is ${this.currentPattern}. Discarding!`);
        this.currentPattern = new Array(this.patternLength).fill(false);
    }
  },
  methods: {
    patternChanged: function(newVal, index) {
        this.$set(this.currentPattern, index, newVal);
        this.$emit('input', this.currentPattern);
    }
  },
  components: {
      PatternToggleButton
  }
};
</script>

<style>
</style>
