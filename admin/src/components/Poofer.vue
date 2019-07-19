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
      return fetch(`${fireControllerURL}/flame/patterns/__${this.bugNumber}_${this.pooferId}`, {
            method: 'POST',
            body: JSON.stringify({ active: true })
          })
          .then(res => {
            // handle non-success responses
            if (!res.ok) {
              alert(`Unable to fire poofer. Request failed with status ${res.status} ${res.statusText}`);
            }
            return res;
          })
          .then(res => {
            alert(`Fired poofer ${this.bugNumber}_${this.pooferId}`);
          }, error => {
            alert(`Failed to fire poofer with error ${error}`);
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
