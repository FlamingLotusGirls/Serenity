<template>
  <header>
      <h2><span class="flg-text">Flaming Lotus Girls: </span>Serenity</h2>
      <div class="tabs">
        <a href="#BigBugs" v-bind:class="{ selected: selectedTab === 'BigBugs' }">Big Bugs</a>
        |
        <a href="#Sound" v-bind:class="{ selected: selectedTab === 'Sound' }">Sound</a>
        |
        <a href="#Light" v-bind:class="{ selected: selectedTab === 'Light' }">Light</a>
        |
        <a href="#Admin" v-bind:class="{ selected: selectedTab === 'Admin' }">Admin</a>
      </div>
  </header>
</template>

<script>
export default {
  props: ['selectedTab']
};
</script>

<style>
  @media (max-device-width: 1030px) {
    header h2 .flg-text {
      display: none;
    }
  }
  @media (max-device-width: 640px) {
    header h2 {
      display: block;
      text-align: center;
      width: 100%;
      margin-left: 0 !important;
    }
    header .tabs {
      width: 100%;
      display: block;
      padding: 10px;
      text-align: center;
      margin: 0 auto;
      font-size: 19px;
    }
  }

  header {
    background: #000;
    height: 40px;
    width: 100%;
    color: #fff;
    align: left;
  }

  header h2 {
    float: left;
    margin-left: 40px;
  }

  header .tabs {
    float: right;
  }
</style>

