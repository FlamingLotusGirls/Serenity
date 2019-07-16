<template>
  <form v-on:submit.prevent="onSubmit" class="big-bugs-page-section fire-program-section">
    <h2>or</h2>
    <p>Select a fire program</p>
    <select class="custom-select" name="fireProgramName" id="fireProgramName" v-model="fireProgramName">
      <option value="0" selected>Ambiguous Name 23</option>
      <option value="1">Charlie's Cool Fire Program</option>
      <option value="2">Chase</option>
      <option value="3">Burn Baby Burn</option>
    </select>
    <button class="btn btn-primary">Start Fire Show</button>
  </form>
</template>

<script>
export default {
    name: 'FireProgramSection',
    methods: {
        onSubmit: function() {
            alert(`Starting fire program with ID ${this.fireProgramName}`);
        }
    }
};
</script>

<style>
.fire-program-section button {
  margin-top: 10px;
}
</style>

