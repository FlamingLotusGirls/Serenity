<template>
    <form class="custom-fire-by-poofer container">
        <h4>Select a bug name to view poofer controls and create fire sequence</h4>
        <div class="sequence-builder-section row">
            <div class="col-2 bug-admin-tabs">
                <button class="btn bug-admin-tab" v-bind:class="{ selected: selectedBug === 'Metric' }" v-on:click="selectBug('Metric')">metric</button>
                <button class="btn bug-admin-tab" v-bind:class="{ selected: selectedBug === 'Brazen' }" v-on:click="selectBug('Brazen')">brazen</button>
                <button class="btn bug-admin-tab" v-bind:class="{ selected: selectedBug === 'John' }" v-on:click="selectBug('John')">john</button>
            </div>
            <div class="col-10">
                <div class="speed-row row">
                    <VolumeSlider :min="0.1" :max="1.0" v-bind:marks="[0.1, 1.0]" />
                </div>
                <PooferFireBuilder
                        v-for="(pooferObj, pooferName) in selectedBugObject"
                        v-bind:pooferName="pooferName"
                        v-model="pooferObj.pattern"
                        />
            </div>
        </div>
        <hr>
        <div class="manage-sequences-section row">
            <div class="remove-sequences-section col-6">
                <label>Delete Fire Sequence</label>
                <select class="custom-select" name="patternName" id="fireProgramName" v-model="fireProgramToDelete">
                    <option v-for="programName in fireProgramNames" v-bind:value="programName">{{programName}}</option>
                </select>
                <button type="button" class="btn btn-primary" v-on:click="deleteFireProgram">Delete</button>
            </div>
            <div class="save-sequence-section col-6">
                <input type="text" class="form-control" placeholder="Enter a unique name">
                <button type="submit" class="btn btn-primary">Save Sequence</button>
            </div>
        </div>
    </form>
</template>

<script>
import PooferFireBuilder from './PooferFireBuilder.vue';
import VolumeSlider from './VolumeSlider.vue';
import { getFireProgramNameList, deleteFirePattern } from '../requests';

export default {
    components: {
        PooferFireBuilder,
        VolumeSlider
    },
    data() {
        return {
            myObj: {
                testArr: new Array(20).fill(false)
            },
            fireProgramNames: [],
            fireProgramToDelete: null,
            selectedBug: 'Metric',
            currentProgramPatterns: {},
            poofDuration: 0.25
        };
    },
    computed: {
        selectedBugObject() {
            return this.currentProgramPatterns[this.selectedBug];
        }
    },
    beforeMount() {
        // fill the current program with blanks for all poofer patterns
        let bugNames = ['Metric', 'Brazen', 'John'];
        let pooferNames = ['butt1', 'butt2', 'butt3', 'butt4', 'butt5', 'butt6', 'head1', 'head2'];
        bugNames.forEach(bugName => {
            this.$set(this.currentProgramPatterns, bugName, {});
            pooferNames.forEach(pooferName => {
                this.$set(this.currentProgramPatterns[bugName], pooferName, { pattern: new Array(20).fill(false) });
            });
        });

        getFireProgramNameList()
            .then(list => {
                this.fireProgramNames = list;
            }, error => {
                alert(error);
            });
    },
    methods: {
        selectBug(bugName) {
            this.selectedBug = bugName;
        },
        deleteFireProgram() {
            // nothing happens if nothing is selected
            if (!this.fireProgramToDelete) return;

            deleteFirePattern(this.fireProgramToDelete)
                .then(() => {
                    alert(`Deleted fire program ${this.fireProgramToDelete}`);
                    this.fireProgramToDelete = null;
                }, error => {
                    alert(error);
                });
        }
    }
};
</script>

<style>
    .sequence-builder-section {
        border: 1px solid #999;
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
    .remove-sequences-section select {
        max-width: 220px;
        margin: auto 12px;
    }
    .save-sequence-section input {
        max-width: 240px;
        margin: auto 12px;
        display: inline-block;
    }
</style>
