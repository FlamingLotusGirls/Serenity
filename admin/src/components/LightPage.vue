<template>
    <div class="lights-page">
        <div class="jars-section">
            <div class="jars-header">
                <h2>⚱️Jars</h2>
                <a href="#">view color palette sources</a>
                <button>Synchronize Jars</button>
            </div>
            <div class="jars-content">
                <div class="jar-lighting-widget">
                    <label>Set Metric's Color Palette</label>
                    <select class="custom-select">
                        <option value="0" selected>🔴🔵🎨</option>
                    </select>
                    <label>Select a Jar Pattern for Metric</label>
                    <select class="custom-select">
                        <option value="0" selected>soothingFlow</option>
                    </select>
                    <label for="jarBrightness">Set Metric's Jar Brightness:</label>
                    <input type="range" name="jarBrightness" id="jarBrightness" />
                </div>
            </div>
        </div>
        <div class="small-bugs-section">
            <div class="small-bugs-header">
                <h2>Small Bugs</h2>
                <button>Synchronize Jars</button>
            </div>
            <div class="small-bugs-content">
                <div class="swarm-lighting-widget">
                    <h5>Swarm One</h5>
                    <label>Off/On</label>
                    <input type="checkbox"/>
                    <label for="swarmBrightness">Set Swarm Brightness:</label>
                    <input type="range" name="swarmBrightness" id="swarmBrightness" />
                    <label>Select a Light Program:</label>
                    <select class="custom-select">
                        <option value="0" selected>matingDance2</option>
                    </select>
                </div>
            </div>
        </div>
        <div class="extra-lights-section">
            <label>👀Big Bug Eyes</label>
            <input type="checkbox"/>
            <label for="benchBrightness">💺 Set Bench Brightness:</label>
            <input type="range" name="benchBrightness" id="benchBrightness" />
        </div>
        <button>💡Apply Changes</button>
    </div>
</template>

<script>
</script>

<style>
</style>

