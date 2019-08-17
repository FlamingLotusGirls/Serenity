<template>
    <form class="custom-fire-by-poofer container">
        <h4>Select a bug name to view poofer controls and create fire sequence</h4>
        <div class="sequence-builder-section row">
            <div class="col-1 bug-admin-tabs">
                <button class="btn bug-admin-tab" v-bind:class="{ selected: selectedBug === 'Metric' }" v-on:click="selectBug('Metric')">metric</button>
                <button class="btn bug-admin-tab" v-bind:class="{ selected: selectedBug === 'Brazen' }" v-on:click="selectBug('Brazen')">brazen</button>
                <button class="btn bug-admin-tab" v-bind:class="{ selected: selectedBug === 'John' }" v-on:click="selectBug('John')">john</button>
            </div>
            <div class="col-11">
                <PooferFireBuilder pooferName="butt1" />
                <PooferFireBuilder pooferName="butt2" />
                <PooferFireBuilder pooferName="butt3" />
                <PooferFireBuilder pooferName="butt4" />
                <PooferFireBuilder pooferName="butt5" />
                <PooferFireBuilder pooferName="butt6" />
                <PooferFireBuilder pooferName="head1" />
                <PooferFireBuilder pooferName="head2" />
            </div>
        </div>
        <div class="manage-sequences-section row">
            <div class="remove-sequences-section col-8">
                <p>Remove Fire Sequence</p>
                <select class="custom-select" name="patternName" id="fireProgramName" v-model="selectedFireProgram">
                    <option v-for="programName in fireProgramNames" v-bind:value="programName">{{programName}}</option>
                </select>
            </div>
            <div class="save-sequence-section col-4">
                <input type="text" placeholder="Enter a unique name">
                <button type="submit" class="btn btn-primary">Save Fire</button>
            </div>
        </div>
    </form>
</template>

<script>
import PooferFireBuilder from './PooferFireBuilder.vue';
import { getFireProgramNameList } from '../requests';

export default {
    components: {
        PooferFireBuilder
    },
    data() {
        return {
            fireProgramNames: [],
            selectedBug: 'Metric'
        };
    },
    beforeMount() {
        getFireProgramNameList()
            .then(list => {
                this.fireProgramNames = list;
            }, error => {
                alert(error);
            });
    },
    methods: {
        selectBug(bugName) {
            this.selectedBug = bugName
        }
    }
};
</script>

<style>
    .sequence-builder-section {
        border: 1px solid #99`9;
        margin: 10px;
    }
    .manage-sequences-section {
        margin-top: 20px;
    }
    .bug-admin-tabs {
        left: -16px;
    }
    .bug-admin-tab {
        font-weight: bold;
        border-radius: 2px;
        border: solid 1px #979797;
        width: 134%;
    }
    .bug-admin-tab.selected {
        background: #de019e;
    }
</style>
