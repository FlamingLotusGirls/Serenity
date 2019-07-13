<template>
  <header>
      <h2>Flaming Lotus Girls: Serenity</h2>
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
    margin-right: 40px;
    margin-top: 4px;
    font-size: 24px;
  }
  header .tabs a {
    margin: 10px 4px;
    color: #A19FA1;
  }
  header .tabs a:hover {
    color: #DE019E;
  }
  header .tabs a.selected {
    font-weight: bold;
    color: #DE019E
  }
</style>

