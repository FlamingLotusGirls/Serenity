<template>
  <div id="app">
    <Header v-bind:selectedTab="selectedTab"/>
    <keep-alive>
      <component class="page-content" v-bind:is="pageComponent"></component>
    </keep-alive>
    <Footer/>
  </div>
</template>

<script>
import Header from './components/Header.vue'
import BigBugsPage from './components/BigBugsPage.vue'
import SoundPage from './components/SoundPage.vue'
import LightPage from './components/LightPage.vue'
import AdminPage from './components/AdminPage.vue'
import Footer from './components/Footer.vue'

export default {
  name: 'app',
  components: {
    Header,
    BigBugsPage,
    SoundPage,
    LightPage,
    AdminPage,
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
      selectedTab: 'BigBugs',
      interfacesShown: {
          poofing: false
      }
  }),
  computed: {
    pageComponent: function() {
      return this.selectedTab + 'Page';
    }
  },
  mounted: function() {
    window.addEventListener('hashchange', () => {
      this.hashChanged();
    });
  
    // immmediately pretend the hash changed when we load the page, to initialize
    // the right tab
    this.hashChanged();
  },
  methods: {
    hashChanged: function() {
      // alert(`hash changed to ${location.hash}`)
      const validTabs = ['BigBugs', 'Sound', 'Light', 'Admin'];
      let targetTab = location.hash.slice(1);
      if (validTabs.includes(targetTab)) {
        this.selectedTab = targetTab;
      } else {
        console.log('validTabs ', validTabs, 'does not include', targetTab);
        // if we got to an invalid URL, just head back to the big bugs
        location.hash = "#BigBugs";
      }
    },

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

<style>
.page-content {
  padding: 20px 50px;
}
</style>
