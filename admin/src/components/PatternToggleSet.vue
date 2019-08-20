<template>
  <div class="pattern-toggle-set">
    <PatternToggleButton v-for="(patternValue, index) in value" v-bind:value="value[index]" v-bind:patternIndex="index" v-on:input="patternToggleChanged"></PatternToggleButton>
  </div>
</template>

<script>
import PatternToggleButton from './PatternToggleButton';

export default {
  props: ['value'],
  components: {
    PatternToggleButton
  },
  methods: {
    patternToggleChanged: function(updateObj) {
      let newPattern = this.value.slice();
      newPattern[updateObj.patternIndex] = updateObj.newValue;
      this.$emit('input', newPattern);
    }
  }
};
</script>

<style>
</style>
