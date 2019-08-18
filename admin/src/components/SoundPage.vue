<template>
    <div class="sound-page container">
        <div class="soundscape-row">
            <SoundscapeSelector class="col-12" />
        </div>
        <hr>
        <div class="main-section row">
            <div class="left-column col-4">
                <div class="master-volume-control">
                    <label for="masterVolume">Set Master Volume:</label>
                    <input type="range" name="masterVolume" id="masterVolume" />
                </div>
                <div class="zone-volume-controls">
                    <label>Set Zone Volume Controls:</label>
                    <BalanceControl controllerId="pergolaleft">Pergola Left</BalanceControl>
                    <BalanceControl controllerId="pergolaright">Pergola Right</BalanceControl>
                    <BalanceControl controllerId="jars">Jars</BalanceControl>
                    <BalanceControl controllerId="fueldepot">Fuel Depot</BalanceControl>
                </div>
            </div>
            <div class="right-columns col-8">
                <div class="ambient-tracks row">
                    <div class="col-6">
                        <label for="ambientTrackName">Select An Ambient Track:</label>
                        <select class="custom-select" name="ambientTrackName" id="ambientTrackName">
                            <option value="0" selected>soothingFlow</option>
                            <option value="1">Charlie's Cool Soundscape</option>
                            <option value="2">Lots and lots of crickets</option>
                            <option value="3">Burn Baby Burn</option>
                        </select>
                    </div>
                    <div class="col-6">
                        <label for="ambientTrackName">Set Ambient Track Volume:</label>
                        <input type="range" />
                    </div>
                </div>
                <p>
                    Tap a button to activate a sound. Double tap to play moderate density of sound.  Triple tap to get maximum density of sound.  Volume can be set for each sound.
                </p>
                <p>
                    When activated, tap once to turn off. 
                </p>
                <label>Set individual sound densities and volumes</label>
                <div class="single-sound-buttons">
                    <div class="individual-sound-row row">
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Wind</SoundSnippetButton>
                            <input type="range" />
                        </div>
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Owl</SoundSnippetButton>
                            <input type="range" />
                        </div>
                    </div>
                    <div class="individual-sound-row row">
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Leaves Rustling</SoundSnippetButton>
                            <input type="range" />
                        </div>
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Wind 2</SoundSnippetButton>
                            <input type="range" />
                        </div>
                    </div>
                    <div class="individual-sound-row row">
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Dry Grasses</SoundSnippetButton>
                            <input type="range" />
                        </div>
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Distant Thunder</SoundSnippetButton>
                            <input type="range" />
                        </div>
                    </div>
                    <div class="individual-sound-row row">
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Songbird</SoundSnippetButton>
                            <input type="range" />
                        </div>
                        <div class="col-6 single-sound">
                            <SoundSnippetButton>Frog</SoundSnippetButton>
                            <input type="range" />
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <div class="save-soundscape-section row">
            <input type="text" placeholder="Enter a unique name">
            <button class="btn btn-primary" type="submit">Save This Soundscape</button>
        </div>
    </div>
</template>

<script>
import BalanceControl from './BalanceControl';
import SoundSnippetButton from './SoundSnippetButton';
import SoundscapeSelector from './SoundscapeSelector';

export default {
    data: function() {
        return {
            
        };
    },
    components: {
        BalanceControl,
        SoundSnippetButton,
        SoundscapeSelector
    },
    methods: {
    }
};
</script>

<style>
.soundscape-selector label {
    margin-right: 16px;
}
.soundscape-selector select {
    width: auto;
}
.bottom-section {
    margin: 40px 0;
}
.bottom-section p {
    font-weight: bold;
}
</style>

