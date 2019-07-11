<template>
  <form>
    <h2>or</h2>
    <p>Select a fire program</p>
    <select class="custom-select" name="fireProgramName" id="fireProgramName">
      <option value="0" selected>Ambiguous Name 23</option>
      <option value="1">Charlie's Cool Fire Program</option>
      <option value="2">Chase</option>
      <option value="3">Burn Baby Burn</option>
    </select>
    <StartFireButton />
  </form>
</template>

<script>
import StartFireButton from './StartFireButton.vue'

export default {
  name: 'FireProgramSection',
  components: {
    StartFireButton
  }
};
</script>

<style>
</style>

