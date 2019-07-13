<template>
    <div class="sound-page">
        <div class="top-section">
            <div class="master-volume-control">
                🔊
                <label for="masterVolume">Set Master Volume:</label>
                <input type="range" name="masterVolume" id="masterVolume" />
            </div>
            <div class="soundscape-selector">
                <label for="soundscapeName">Select a Soundscape:</label>
                <select class="custom-select" name="soundscapeName" id="soundscapeName">
                    <option value="0" selected>Texas Hill Country</option>
                    <option value="1">Charlie's Cool Soundscape</option>
                    <option value="2">Lots and lots of crickets</option>
                    <option value="3">Burn Baby Burn</option>
                </select>
            </div>
            <div class="balance-controls">
                <div class="balance-control">
                    <label for="balanceControl">Left Pergola Near</label>
                    <input type="range" name="balanceControl" id="balanceControl" />
                </div>
            </div>
            <button class="toggle-sound-button">🔇Mute</button>
        </div>
        <div class="bottom-section">
            <p>Tap a button to add that sound. Double-tap to add more of that sound. Tap a third time to turn off. Max 6.</p>
            <div class="single-sound-buttons">
                <button>Wind</button>
                <button>Owl</button>
                <button>Leaves Rustling</button>
                <button>Wind 2</button>
            </div>
        </div>
    </div>
</template>

<script>
</script>

<style>
</style>

