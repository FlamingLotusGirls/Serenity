<template>
    <div class="sound-page container">
        <div class="top-section row">
            <div class="master-volume-control col-3">
                <h3>🔊</h3>
                <div class="volume-control-internal">
                    <label for="masterVolume">Set Master Volume:</label>
                    <input type="range" name="masterVolume" id="masterVolume" />
                </div>
            </div>
            <div class="soundscape-selector col-3">
                <label for="soundscapeName">Select a Soundscape:</label>
                <select class="custom-select" name="soundscapeName" id="soundscapeName">
                    <option value="0" selected>Texas Hill Country</option>
                    <option value="1">Charlie's Cool Soundscape</option>
                    <option value="2">Lots and lots of crickets</option>
                    <option value="3">Burn Baby Burn</option>
                </select>
            </div>
            <div class="balance-controls col-4">
                <label>Set Balance Controls:</label>
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
            <button class="toggle-mute-button col-2 btn" v-on:click="toggleMute">
                <span class="unmute-icon">{{ muted ? '🔊' : '🔇' }}</span>
                <br>
                {{ muted ? 'Unmute' : 'Mute' }} All Sound
            </button>
        </div>
        <div class="bottom-section row">
            <p>Tap a button to add that sound. Double-tap to add more of that sound. Tap a third time to turn off. Max 6.</p>
            <div class="single-sound-buttons">
                <SoundSnippetButton>Wind</SoundSnippetButton>
                <SoundSnippetButton>Owl</SoundSnippetButton>
                <SoundSnippetButton>Leaves Rustling</SoundSnippetButton>
                <SoundSnippetButton>Wind 2</SoundSnippetButton>
                <SoundSnippetButton>Dry Grasses</SoundSnippetButton>
                <SoundSnippetButton>Distant Thunder</SoundSnippetButton>
                <SoundSnippetButton>Songbird</SoundSnippetButton>
                <SoundSnippetButton>Frog</SoundSnippetButton>
                <SoundSnippetButton>Cricket</SoundSnippetButton>
                <SoundSnippetButton>Cicada</SoundSnippetButton>
                <SoundSnippetButton>Sparrow Chirp</SoundSnippetButton>
                <SoundSnippetButton>Raven</SoundSnippetButton>
            </div>
        </div>
    </div>
</template>

<script>
import BalanceControl from './BalanceControl';
import SoundSnippetButton from './SoundSnippetButton';

export default {
    data: function() {
        return {
            muted: false
        };
    },
    components: {
        BalanceControl,
        SoundSnippetButton
    },
    methods: {
        toggleMute: function() {
            this.muted = !this.muted;
        }
    }
};
</script>

<style>
.master-volume-control > * {
    display: inline-block;
}
.master-volume-control h3 {
    font-size: 48px;
    margin-right: 14px;
}
.master-volume-control .volume-control-internal > * {
    display: block;
}
.toggle-mute-button {
    font-size: 17px;
    white-space: normal;
    height: 136px;
    vertical-align: middle;
    background: #eee;
    border: 1px solid #ccc
}
.toggle-mute-button:hover {
    border: 1px solid #777;
}
.toggle-mute-button .unmute-icon {
    font-size: 64px;
}
.bottom-section {
    margin: 40px 0;
}
.bottom-section p {
    font-weight: bold;
}
</style>

