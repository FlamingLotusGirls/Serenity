<template>
    <div class="poofable-bug">
        <img src="/images/firefly.png" />
        <h3><a href="#" v-on:click="poof">{{bugName}}</a></h3>
        <Poofer v-for="pooferId in pooferIds" v-bind:pooferId="pooferId" v-bind:bugNumber="bugNumber" />
    </div>
</template>

<script>
import Poofer from './Poofer.vue'
import { runFireProgram } from '../requests';

export default {
    props: ['bugName', 'bugNumber'],
    name: 'PoofableBug',
    components: {
        Poofer
    },
    computed: {
        pooferIds() {
            return [
                'T1',
                'T2',
                'T3',
                'T4',
                'T5',
                'T6',
                'A1',
                'A2'
            ]
        }
    },
    methods: {
        poof: function() {
            let formData = new FormData();
            formData.append('active', true);

            return runFireProgram(`__${this.bugNumber}_ALL`)
                .then(() => {
                    console.log(`Fired all poofers on ${this.bugName}`);
                }, error => {
                    alert(error);
                });
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
    .poofable-bug .poof-button.poofer-id-A1 {
        top: -31px;
        left: -27px;
    }
    .poofable-bug .poof-button.poofer-id-A2 {
        top: -31px;
        left: 104px;
    }
    .poofable-bug .poof-button.poofer-id-T1 {
        top: 140px;
        left: 3px;
    }
    .poofable-bug .poof-button.poofer-id-T2 {
        top: 140px;
        left: 80px;
    }
    .poofable-bug .poof-button.poofer-id-T3 {
        top: 185px;
        left: -8px;
    }
    .poofable-bug .poof-button.poofer-id-T4 {
        top: 185px;
        left: 90px;
    }
    .poofable-bug .poof-button.poofer-id-T5 {
        top: 240px;
        left: 20px;
    }
    .poofable-bug .poof-button.poofer-id-T6 {
        top: 240px;
        left: 63px;
    }
</style>

