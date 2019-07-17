<template>
    <div class="custom-fire-big-bug-section">
        <h3>{{ bugName }}</h3>
        <div class="custom-poof-duration">
            <label>Set Poof Duration</label>
            <input type="range" min="0.25" max="3"/>
        </div>
        <div class="custom-poof-pattern">
            <label>Set A Pattern</label>
            <PatternToggleButton v-for="n in 20"></PatternToggleButton>
        </div>
  </div>
</template>

<script>
import PatternToggleButton from './PatternToggleButton';

export default {
    name: 'BigBugFireBuilder',
    props: ['bugName'],
    components: {
        PatternToggleButton
    }
};
</script>

<style></style>
