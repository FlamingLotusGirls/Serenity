<template>
    <div class="sound-admin-page container">
        <form class="row">
            <div class="col-8">
                <div class="row sound-admin-top-section">
                    <div class="col-4 sound-admin-select-ambient-track">
                        <label>Select An Ambient Track</label>
                        <select class="custom-select">
                            <option value="soothingFlow" selected>soothingFlow</option>
                        </select>
                    </div>
                    <div class="col-8 sound-admin-overall-volume">
                        <label>Set Overall Volume</label>
                        <input type="range" />
                    </div>
                </div>
                <div class="row">
                    <div class="col-12 sound-admin-select-sounds">
                        <label>Select Sounds to Activate (up to 6)</label>
                        <div>
                            <input type="checkbox" /> Cricket
                            <input type="range" />
                        </div>
                        <div>
                            <input type="checkbox" /> Cicada
                            <input type="range" />
                        </div>
                        <div>
                            <input type="checkbox" /> Wind
                            <input type="range" />
                        </div>
                    </div>
                </div>
            </div>
            <div class="col-4 sound-admin-balance-controls">
                <label>Set Speaker Balance Controls:</label>
                <div class="balance-controls-list">
                    <BalanceControl controllerId="leftpergolanear">Left Pergola Near</BalanceControl>
                    <BalanceControl controllerId="leftpergolamid">Left Pergola Mid</BalanceControl>
                    <BalanceControl controllerId="leftpergolafar">Left Pergola Far</BalanceControl>
                    <BalanceControl controllerId="rightpergolanear">Right Pergola Near</BalanceControl>
                    <BalanceControl controllerId="rightpergolamid">Right Pergola Mid</BalanceControl>
                    <BalanceControl controllerId="rightpergolafar">Right Pergola Far</BalanceControl>
                    <BalanceControl controllerId="metric">Metric</BalanceControl>
                    <BalanceControl controllerId="brazen">Brazen</BalanceControl>
                    <BalanceControl controllerId="john">John</BalanceControl>
                    <BalanceControl controllerId="jar3">Jar 3</BalanceControl>
                    <BalanceControl controllerId="fueldepot">Fuel Depot</BalanceControl>
                </div>
            </div>
        </form>
    </div>
</template>

<script>
import BalanceControl from './BalanceControl';

export default {
    components: {
        BalanceControl
    }
};
</script>

<style>
.sound-admin-top-section {
    border-bottom: 1px solid rgb(151, 151, 151);
}
.sound-admin-select-ambient-track {
    border-right: 1px solid rgb(151, 151, 151);
    padding-bottom: 16px;
    padding-right: 26px;
}
.sound-admin-overall-volume {
    padding-left: 48px;
    padding-right: 36px;
}
.sound-admin-overall-volume label {
    display: block;
    margin-bottom: 16px;
}
.sound-admin-overall-volume input {
    width: 100%;
}
.sound-admin-balance-controls {
    border-left: 1px solid rgb(151, 151, 151);
}
.sound-admin-balance-controls .balance-controls-list {
    padding-left: 20px;
}
.sound-admin-select-sounds {
    padding-top: 20px;
}
</style>
