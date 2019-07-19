<template>
  <form v-on:submit.prevent="onSubmit" class="big-bugs-page-section fire-program-section">
    <h2>or</h2>
    <p>Select a fire program</p>
    <select class="custom-select" name="patternName" id="fireProgramName" v-model="selectedFireProgram">
      <option v-for="programName in fireProgramNames" v-bind:value="programName">{{programName}}</option>
    </select>
    <button class="btn btn-primary">Start Fire Show</button>
    <input type="hidden" name="active" value="true" />
  </form>
</template>

<script>
import { fireControllerURL } from '../appConfig';

export default {
    name: 'FireProgramSection',
    data() {
      return {
        fireProgramNames: [],
        selectedFireProgram: null
      };
    },
    beforeMount() {
      fetch(`${fireControllerURL}/flame/patterns`)
        .then(res => {
          // handle non-success responses
          if (!res.ok) {
            alert(`Unable to fetch list of fire programs. Request failed with status ${res.status} ${res.statusText}`);
          }
          return res;
        })
        .then(res => res.json())
        .then(result => {
          this.fireProgramNames = result.map(fireProgram => fireProgram.name).filter(name => {
            // the single poofers are set up as fire sequences with names starting with __
            // so we should filter all of those out so as not to confuse the user
            return !name.startsWith('__');
          });
        }, error => {
            // triggers only on network errors, not unsuccessful responses
            alert(`Failed to fetch fire program list with error ${error}`);
        });
    },
    methods: {
        onSubmit: function() {
          fetch(`${fireControllerURL}/flame/patterns/${this.selectedFireProgram}`, {
            method: 'POST',
            body: new FormData(this.$el)
          })
            .then(res => {
              // handle non-success responses
              if (!res.ok) {
                alert(`Unable to start fire program. Request failed with status ${res.status} ${res.statusText}`);
              }
              return res;
            })
            .then(res => {
              alert(`Started fire program ${this.selectedFireProgram}`);
            }, error => {
              alert(`Failed to start fire program with error ${error}`);
            });
        }
    }
};
</script>

<style>
.fire-program-section button {
  margin-top: 10px;
}
</style>

