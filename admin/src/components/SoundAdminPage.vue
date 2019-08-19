<template>
    <div class="sound-admin-page container">
        <div class="row">
            <div class="col-12 sound-admin-balance-controls">
                <label>Set Individual Speaker Level Controls:</label>
                <div class="balance-controls-list">
                    <BalanceControl v-for="(sinkObj, sinkName) in sinks" v-model="sinkObj.volume">{{sinkName}}</BalanceControl>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import BalanceControl from './BalanceControl';
import { getSoundscapesList, getDefaultSoundscapeId, getAudioSinks, setAudioSinkVolumes } from '../requests';

export default {
    data: function() {
        return {
            soundscapeNames: [],
            defaultSoundscapeName: null,
            sinks: {}
        };
    },
    beforeMount() {
        getSoundscapesList()
            .then(soundscapes => {
                    this.soundscapeNames = soundscapes;
                }, error => {
                    alert(error);
                });

        getDefaultSoundscapeId()
            .then(name => {
                    this.defaultSoundscapeName = name;
                    console.log('got default soundscape name ', this.defaultSoundscapeName);
                }, error => {
                    alert(error);
                });

        getAudioSinks()
            .then(sinks => {
                    this.sinks = sinks;
                }, error => {
                    alert(error);
                });
    },
    watch: {
        sinks: {
            handler() {
                // save sinks, since they've changed
                setAudioSinkVolumes(this.sinks)
                    .then(() => {
                        console.log('Saved sink levels successfully');
                    }, error => {
                        alert(error);
                    });
            },
            deep: true
        }
    },
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
.sound-admin-balance-controls .balance-controls-list {
    padding-left: 20px;
}
.sound-admin-select-sounds {
    padding-top: 20px;
}
</style>
