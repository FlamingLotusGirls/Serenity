<template>
  <div class="simple-poofer-section">
    <p>Tap a button on a bug to make a poof. Tap the bug's name to poof all poofers on that bug.</p>
    <PoofableBug />
    <PoofableBug />
    <PoofableBug />
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
</style>

