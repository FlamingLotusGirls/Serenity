<template>
  <button class="poof-pattern-button" v-on:click="toggleState"></button>
</template>

<script>
export default {
  props: ['initiallyLit'],
  mounted: function() {
      if (this.initiallyLit) {
          this.$el.classList.add('lit');
      }
  },
  methods: {
    toggleState: function() {
        this.$el.classList.toggle('lit');
    }
  }
};
</script>

<style>
.poof-pattern-button {
    -webkit-appearance: none;
    border: 1px solid #000;
    width: 20px;
    height: 20px;
    border-radius: 10px;
    background: #000;
}
.poof-pattern-button.lit {
    background: #efb64c;
}
</style>
