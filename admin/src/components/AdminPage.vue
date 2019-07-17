<template>
  <div class="admin-page">
    <div class="row">
        <div class="col-12 tabs">
            <a href="#Admin/Fire" v-bind:class="{ selected: selectedAdminTab === 'Fire' }">Fire Admin</a>
            |
            <a href="#Admin/Sound" v-bind:class="{ selected: selectedAdminTab === 'Sound' }">Sound Admin</a>
            |
            <a href="#Admin/Light" v-bind:class="{ selected: selectedAdminTab === 'Light' }">Light Admin</a>
        </div>
    </div>
    <keep-alive>
      <component class="admin-subpage-content" v-bind:is="adminPageComponent"></component>
    </keep-alive>
  </div>
</template>

<script>
import FireAdminPage from './FireAdminPage.vue';
import SoundAdminPage from './SoundAdminPage.vue';
import LightAdminPage from './LightAdminPage.vue';

export default {
    props: ['selectedAdminTab'],
    name: 'AdminPage',
    computed: {
        adminPageComponent: function() {
            return this.selectedAdminTab + 'AdminPage';
        }
    },
    components: {
        FireAdminPage,
        SoundAdminPage,
        LightAdminPage
    }
};
</script>

<style>
.admin-page .tabs {
    font-size: 28px;
    margin-bottom: 15px;
}
</style>
