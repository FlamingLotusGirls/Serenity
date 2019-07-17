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
        <form class="jar-color-palettes">
            <h3>create jar color palettes from photographs</h3>
            <input type="file" placeholder="Choose an image source">
            <button class="btn btn-primary">Upload Image</button>
        </form>
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

.jar-color-palettes {
    border-top: 1px solid rgb(151, 151, 151);
    padding-top: 12px;
}
</style>
