/* Main Client-Side Entry Point */
!!(function() {
    Vue.component('poofer-form', {
        template: '#poofer-form-template'
    });

    var app = new Vue({
        el: '#app',
        data: {
            sound: {
                volumes: {
                    a: 0.5,
                    b: 0.7,
                    c: 0.2
                }
            },
            interfacesShown: {
                poofing: false
            }
        },
        methods: {
            /* Fire */
            stopAllFireEffects: function() {
                // TODO: make REST request to server to stop all fire effects
                alert('All fire effects stopped! (OK, not really)');
            },
            showPoofingInterface: function() {
                this.interfacesShown.poofing = !this.interfacesShown.poofing;
            },
            showRunFireSequenceInterface: function() {

            },
            showFireSequenceBuilder: function() {

            },
            
            /* Light */
            showChangeLightPatternInterface: function() {

            },
            showChangeLightColorsInterface: function() {

            },

            /* Sound */
            increaseVolume: function(location) {
                // TODO: make REST request to server to inrease volume
            },
            decreaseVolume: function(location) {
                // TODO: make REST request to server to decrease volume
            },
            showPlaySoundEffectInterface: function(location) {

            },
        }
    });
})();