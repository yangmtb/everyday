import Vue from 'vue'
import Router from 'vue-router'
import HelloWorld from '@/components/HelloWorld'
import XXX from '@/components/yoyo'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'xixi',
      component: XXX
    }
  ]
})
