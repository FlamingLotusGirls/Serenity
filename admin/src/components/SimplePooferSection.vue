<template>
  <div class="big-bugs-page-section">
    <p>Tap a button on a bug to make a poof. Tap the bug's name to poof all poofers on that bug.</p>
    <PoofableBug bugName="Metric"/>
    <PoofableBug class="middle-bug" bugName="Brazen"/>
    <PoofableBug bugName="John"/>
  </div>
</template>

<script>
import PoofableBug from './PoofableBug.vue'

export default {
  name: 'SimplePooferSection',
  components: {
    PoofableBug
  }
};
</script>

<style>
.middle-bug {
  top: 80px;
}
</style>

