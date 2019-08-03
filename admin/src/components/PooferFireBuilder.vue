<template>
    <div class="custom-fire-big-bug-section">
        <h3>{{ pooferName }}</h3>
        <div class="custom-poof-pattern">
            <label>Set A Pattern</label>
            <PatternToggleButton v-for="n in 20"></PatternToggleButton>
        </div>
  </div>
</template>

<script>
import PatternToggleButton from './PatternToggleButton';

export default {
    name: 'PooferFireBuilder',
    props: ['pooferName'],
    components: {
        PatternToggleButton
    }
};
</script>

<style></style>
