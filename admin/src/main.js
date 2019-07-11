import Vue from 'vue'
import App from './App.vue'

// new Vue({
//   render: h => h(App),
// }).$mount('#app')

/* Main Client-Side Entry Point */
Vue.config.productionTip = false


window.addEventListener('load', function() {
  new Vue({
    render: h => h(App)
  }).$mount('#app');
})
