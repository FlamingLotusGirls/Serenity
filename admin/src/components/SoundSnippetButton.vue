<template>
    <button class="sound-snippet-button btn" v-on:click="onClick" v-bind:class="classObject"><slot/></button>
</template>

<script>
export default {
    props: ['soundName'],
    data: function() { 
        return {
            activationLevel: 0
        };
    },
    computed: {
        classObject: function() {
            switch (this.activationLevel) {
                case 0:
                    return {};
                case 1:
                    return { 'half-activated': true };
                case 2:
                    return { 'full-activated': true };

                default:
                    return {};
            }
        }
    },
    methods: {
        onClick: function() {
            if (this.activationLevel >= 2) {
                this.activationLevel = 0;
            } else {
                this.activationLevel++;
            }

            this.$emit('activationLevelChanged');
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
    .sound-snippet-button.half-activated {
        background: #d8f0fe;
    }
    .sound-snippet-button.full-activated {
        background: #79d5ff;
    }
</style>
