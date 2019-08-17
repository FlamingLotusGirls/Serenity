<template>
  <div class="pattern-toggle-set">
    <PatternToggleButton v-for="(patternValue, index) in value" v-model="value[index]" v-on:input="patternChanged"></PatternToggleButton>
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
    patternChanged: function() {
      this.$emit('input', this.value);
    }
  }
};
</script>

<style>
</style>
