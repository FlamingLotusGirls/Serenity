<template>
    <form class="custom-fire-by-poofer">
        <h4>set custom fire controls by big bug</h4>
        <BigBugFireBuilder bugName="Metric" />
        <BigBugFireBuilder bugName="Brazen" />
        <BigBugFireBuilder bugName="John" />
        <div class="save-section">
            <input type="text" placeholder="Enter a unique name">
            <button type="submit" class="btn btn-primary">Save Fire</button>
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
    .save-section {
        margin-top: 20px;
    }
</style>
