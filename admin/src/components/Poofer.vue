<template>
  <button class="poof-button" v-on:click="poof">🔥</button>
</template>

<script>
export default {
  props: ['pooferId', 'bugNumber'],
  mounted: function() {
    this.$el.classList.add(`poofer-id-${this.pooferId}`);
  },
  methods: {
    poof: function() {
      // TODO: add fetch() request to poof poofer here
      alert(`Would send poof for poofer ${this.bugNumber}_${this.pooferId}`);
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
