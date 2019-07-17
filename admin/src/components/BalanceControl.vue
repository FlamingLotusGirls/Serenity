<template>
    <div class="balance-control">
        <input type="range" v-bind:name="controllerId" /> &nbsp; <slot/>
    </div>
</template>

<script>
    export default {
        props: ['controllerId']
    };
</script>

<style>
.balance-control {
    margin-top: 6px;
}
</style>
