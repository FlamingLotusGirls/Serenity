<template>
  <!-- for accessibility etc, would be better if this was actually a checkbox input underneath -->
  <button class="pattern-toggle-button" v-on:click="toggleState" v-bind:class="{ lit: value }"></button>
</template>

<script>
export default {
  props: ['value', 'patternIndex'],
  methods: {
    toggleState: function() {
      this.$emit('input', !this.value);
    }
  }
};
</script>

<style>
.pattern-toggle-button {
    -webkit-appearance: none;
    border: 1px solid #000;
    width: 20px;
    height: 20px;
    border-radius: 10px;
    background: #000;
}
.pattern-toggle-button.lit {
    background: #efb64c;
}
</style>
