<template>
  <div class="big-bugs-page-section">
    <p>Tap a button on a bug to make a poof. Tap the bug's name to poof all poofers on that bug.</p>
    <PoofableBug bugName="Metric" bugNumber="1" />
    <PoofableBug class="middle-bug" bugName="John" bugNumber="2" />
    <PoofableBug bugName="Brazen" bugNumber="3" />
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
@media (max-device-width: 863px) {
  .middle-bug {
    top: 0;
  }
}
</style>

