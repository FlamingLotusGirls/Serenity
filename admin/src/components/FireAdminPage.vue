<template>
    <form class="custom-fire-by-poofer">
        <h4>set custom fire controls by big bug</h4>
        <BigBugFireBuilder bugName="Metric" />
        <BigBugFireBuilder bugName="Brazen" />
        <BigBugFireBuilder bugName="John" />
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
import BigBugFireBuilder from './BigBugFireBuilder.vue';

export default {
    components: {
        BigBugFireBuilder
    }
};
</script>

<style>
    .custom-fire-by-poofer {
        background: #bbb;
        border: 1px solid #999;
        margin: 10px;
    }
    .manage-sequences-section {
        margin-top: 20px;
    }
</style>
