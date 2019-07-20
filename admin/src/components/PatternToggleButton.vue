<template>
  <!-- for accessibility etc, would be better if this was actually a checkbox input underneath -->
  <button class="pattern-toggle-button" v-on:click="toggleState" v-bind:class="{ lit: lit }"></button>
</template>

<script>
export default {
  props: ['initiallyLit', 'patternIndex'],
  data() {
    return {
      lit: this.initiallyLit
    }
  },
  methods: {
    toggleState: function() {
        this.lit = !this.lit;
        this.$emit('toggle-state-changed', this.lit, this.patternIndex);
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
