<template>
  <button class="poof-button" v-on:click="poof">🔥</button>
</template>

<script>
export default {
  props: ['pooferId', 'bugName'],
  mounted: function() {
    this.$el.classList.add(`poofer-id-${this.pooferId}`);
  },
  methods: {
    poof: function() {
      alert(`Would send poof for poofer ${this.pooferId} in big bug ${this.bugName}`);
    }
  }
};
</script>

<style>
  .poof-button {
    font-size: 48px;
    -webkit-appearance: none;
    border: none;
    background: transparent;
    cursor: pointer;
  }
</style>
