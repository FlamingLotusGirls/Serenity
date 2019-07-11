<template>
  <div id="app">
    <Header/>
    <MainContentContainer/>
    <Footer/>
  </div>
</template>

<script>
import Header from './components/Header.vue'
import MainContentContainer from './components/MainContentContainer.vue'
import Footer from './components/Footer.vue'

export default {
  name: 'app',
  components: {
    Header,
    MainContentContainer,
    Footer
  },
  data: () => ({
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
  }),
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
    increaseVolume: function() {
        // TODO: make REST request to server to increase volume
    },
    decreaseVolume: function() {
        // TODO: make REST request to server to decrease volume
    },
    showPlaySoundEffectInterface: function() {

    },
  }
}
</script>

<style></style>
