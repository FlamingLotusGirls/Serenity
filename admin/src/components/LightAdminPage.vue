<template>
    <div class="light-admin-page">
        <h4>set custom LED color and flash patterns by swarm</h4>
        <SwarmLightAdminWidget swarmNumberString="One" />
        <SwarmLightAdminWidget swarmNumberString="Two" />
        <SwarmLightAdminWidget swarmNumberString="Three" />
        <SwarmLightAdminWidget swarmNumberString="Four" />
        <div class="save-section">
            <input type="text" placeholder="Enter a unique name" />
            <button class="btn btn-primary">Save Lights</button>
        </div>
    </div>
</template>

<script>
import SwarmLightAdminWidget from './SwarmLightAdminWidget';

export default {
    components: {
        SwarmLightAdminWidget
    }
};
</script>

<style>
.save-section {
    text-align: right;
    padding-top: 20px;
    padding-bottom: 20px;
}
.save-section input {
    margin-right: 8px;
}
</style>
