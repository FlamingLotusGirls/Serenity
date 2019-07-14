<template>
    <div class="balance-control">
        <input type="range" v-bind:name="controllerId" /> {{controllerName}}
    </div>
</template>

<script>
    export default {
        props: ['controllerName', 'controllerId']
    };
</script>

<style>
.balance-control {
    margin-top: 6px;
}
</style>
