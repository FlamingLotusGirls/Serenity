<template>
  <button class="poof-button" v-on:click="poof">🔥</button>
</template>

<script>
import { runFireProgram } from '../requests';

export default {
  props: ['pooferId', 'bugNumber'],
  mounted: function() {
    this.$el.classList.add(`poofer-id-${this.pooferId}`);
  },
  methods: {
    poof: function() {
      let formData = new FormData();
      formData.append('active', true);

      return runFireProgram(`__${this.bugNumber}_${this.pooferId}`)
        .then(() => {
          alert(`Fired poofer ${this.bugNumber}_${this.pooferId}`);
        }, error => {
          alert(error);
        });
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
