<template>
    <button class="sound-snippet-button btn" v-on:click="onClick" v-bind:class="classObject"><slot/></button>
</template>

<script>
export default {
    props: ['value'],
    computed: {
        classObject: function() {
            if (this.value === 0) {
                return {};
            } else {
                return `intensity-${this.value}`;
            }
        }
    },
    methods: {
        onClick: function() {
            let newValue;
            if (this.value >= 3) {
                newValue = 0;
            } else {
                newValue = this.value + 1;
            }

            this.$emit('input', newValue);
        }
    }
};
</script>

<style>
    .sound-snippet-button {
        border: 1px solid #bbb;
        color: #777;
        padding: 10px 25px;
        margin: 20px 12px 0 0;
    }
    .sound-snippet-button.intensity-1 {
        background: #d8f0fe;
    }
    .sound-snippet-button.intensity-2 {
        background: #a7dfff;
    }
    .sound-snippet-button.intensity-3 {
        background: #79d5ff;
    }
</style>
