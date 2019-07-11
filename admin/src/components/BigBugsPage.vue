<template>
  <div class="big-bugs-page">
    <SimplePooferSection/>
    <FireProgramSection/>
  </div>
</template>

<script>
import SimplePooferSection from './SimplePooferSection.vue'
import FireProgramSection from './FireProgramSection.vue'

export default {
  name: 'BigBugsPage',
  components: {
    SimplePooferSection,
    FireProgramSection
  }
};
</script>

<style>
</style>

