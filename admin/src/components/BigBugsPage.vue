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
.big-bugs-page-section {
  display: inline-block;
  vertical-align: top;
  margin: 20px;
}
</style>

