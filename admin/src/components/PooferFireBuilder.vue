<template>
    <div class="custom-fire-big-bug-section">
        <h3>{{ pooferName }}</h3>
        <div class="custom-poof-pattern">
            <label>Set A Pattern</label>
            <PatternToggleSet v-bind:value="value" v-on:input="updatePattern"></PatternToggleSet>
        </div>
  </div>
</template>

<script>
import PatternToggleSet from './PatternToggleSet';

export default {
    name: 'PooferFireBuilder',
    props: ['value', 'pooferName', 'bugName'],
    components: {
        PatternToggleSet
    },
    methods: {
        updatePattern(newPattern) {
            this.$emit('input', newPattern);
        }
    }
};
</script>

<style></style>
