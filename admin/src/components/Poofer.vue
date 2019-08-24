<template>
  <button class="poof-button" v-on:click="poof" v-bind:class="classObject">🔥</button>
</template>

<script>
import { runFireProgram } from '../requests';
const DEFAULT_POOF_DURATION = 300;
let activeClearingTimer = null;

export default {
  props: ['pooferId', 'bugNumber'],
  data() {
    return {
      active: false
    };
  },
  computed: {
    classObject() {
      let obj = {
        poofing: this.active
      };

      obj[`poofer-id-${this.pooferId}`] = true;

      return obj;
    }
  },
  methods: {
    poof: function() {
      let formData = new FormData();
      formData.append('active', true);

      return runFireProgram(`__${this.bugNumber}_${this.pooferId}`)
        .then(() => {
          console.log(`Fired poofer ${this.bugNumber}_${this.pooferId}`);

          // add the poofing class but only for the length of the poof (0.3 sec currently)
          this.active = true;

          // if we do multiple poofs overlapping, need to make sure we don't end up with weird timer glitches
          if (activeClearingTimer) {
            clearTimeout(activeClearingTimer);
          }

          activeClearingTimer = setTimeout(() => {
            clearTimeout(activeClearingTimer);
            activeClearingTimer = null;

            this.active = false;
          }, 300)
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

  .poof-button.poofing {
    filter: brightness(0.8);
  }
</style>
