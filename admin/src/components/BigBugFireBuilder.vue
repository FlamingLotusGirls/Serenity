<template>
    <div class="custom-fire-big-bug-section">
        <h3>{{ bugName }}</h3>
        <div class="custom-poof-duration">
            <label>Set Poof Duration</label>
            <input type="range" min="0.25" max="3"/>
        </div>
        <div class="custom-poof-pattern">
            <label>Set A Pattern</label>
            <PoofPatternButton v-for="n in 20"></PoofPatternButton>
        </div>
  </div>
</template>

<script>
import PoofPatternButton from './PoofPatternButton';

export default {
    name: 'BigBugFireBuilder',
    props: ['bugName'],
    components: {
        PoofPatternButton
    }
};
</script>

<style>
    .custom-fire-big-bug-section label {
        text-transform: uppercase;
        color: #999;
        font-size: 12px;
    }
</style>
