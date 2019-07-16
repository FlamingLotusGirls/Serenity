<template>
    <div class="poofable-bug">
        <img src="/images/firefly.png" />
        <h3><a href="#" v-on:click="poof">{{bugName}}</a></h3>
        <Poofer pooferId="antenna1" v-bind:bugName="bugName"/>
        <Poofer pooferId="antenna2" v-bind:bugName="bugName"/>
        <Poofer pooferId="butt1" v-bind:bugName="bugName"/>
        <Poofer pooferId="butt2" v-bind:bugName="bugName"/>
        <Poofer pooferId="butt3" v-bind:bugName="bugName"/>
        <Poofer pooferId="butt4" v-bind:bugName="bugName"/>
        <Poofer pooferId="butt5" v-bind:bugName="bugName"/>
        <Poofer pooferId="butt6" v-bind:bugName="bugName"/>
    </div>
</template>

<script>
import Poofer from './Poofer.vue'

export default {
    props: ['bugName'],
    name: 'PoofableBug',
    components: {
        Poofer
    },
    methods: {
        poof: function() {
            alert(`Poofing all poofers on ${this.bugName}`);
        }
    }
};
</script>

<style>
    .poofable-bug {
        display: inline-block;
        position: relative;
        margin: 48px;
    }
    .poofable-bug h3 {
        text-align: center;
        margin-top: 24px;
    }
    .poofable-bug img {
        width: 145px;
        height: 278px;
    }

    .poofable-bug .poof-button {
        position: absolute;
    }
    .poofable-bug .poof-button.poofer-id-antenna1 {
        top: -31px;
        left: -27px;
    }
    .poofable-bug .poof-button.poofer-id-antenna2 {
        top: -31px;
        left: 104px;
    }
    .poofable-bug .poof-button.poofer-id-butt1 {
        top: 140px;
        left: 3px;
    }
    .poofable-bug .poof-button.poofer-id-butt2 {
        top: 140px;
        left: 80px;
    }
    .poofable-bug .poof-button.poofer-id-butt3 {
        top: 185px;
        left: -8px;
    }
    .poofable-bug .poof-button.poofer-id-butt4 {
        top: 185px;
        left: 90px;
    }
    .poofable-bug .poof-button.poofer-id-butt5 {
        top: 240px;
        left: 20px;
    }
    .poofable-bug .poof-button.poofer-id-butt6 {
        top: 240px;
        left: 63px;
    }
</style>

