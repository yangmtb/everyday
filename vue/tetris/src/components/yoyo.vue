<template lang='jade'>
.screen
  .panel
    .matrix
      b(v-for='i of bgArray', :class='{"c": i.value}')
    .btnGroup
      button.left(@click='left' @keyup.left='left') left
      button.right(@click='right') right
      button.rotate(@click='rotate') rotate
      button.down(@click='down') dddow
      button.con(@click='con') con
      button.close(@click='close') close
    .txt
</template>
<script>
export default {
       name: 'xixi',
       data() {
              return {
                     ws: '',
                     row: 20,
                     cell: 10,
                     bgArray: []
              }
       },
       methods: {
                init() {
                       this.bgArray = []
                       for (let r = 0; r < this.row; r++) {
                           for (let c = 0; c < this.cell; ++c) {
                               this.bgArray.push({rIndex: r, cIndex: c, value: 0})
                           }
                       }
                },
                left() {
                       this.ws.send('left')
                },
                right() {
                       this.ws.send('right')
                },
                rotate() {
                       this.ws.send('rotate')
                },
                down() {
                       this.ws.send('down')
                },
                con() {
                      this.ws = new WebSocket('ws://192.168.1.101:9999')
                      const that = this
                      this.ws.onopen = function(event) {
                                console.log('it\'s open');
                                that.ws.send('begin')
                      };
                      this.ws.onmessage = function(event) {
                                that.init()
                                for (let i of that.bgArray) {
                                    for (let j of JSON.parse(event.data)) {
                                        if (i.rIndex == j.x && i.cIndex == j.y) {
                                           i.value = 1
                                        }
                                    }
                                }
                      };
                },
                close() {
                        this.ws.close()
                },
       },
       mounted() {
       },
       created() {
                 window.addEventListener('keyup', (event) => {
console.log(event)                                                  
                       if (37 === event.keyCode) {
                           this.left()
                       }
                 })
                 this.init()
       }
}
</script>
<style lang='stylus'>
.screen
  width 390px
  height 478px
  border solid 5px
  border-color #987f0f #fae36c #fae36c #987f0f
  margin 0 auto
  position relative
  .panel
    width 370px
    height 458px
    margin 0 auto
    background #9ead86
    padding 8px
    border 2px solid #494536
  .txt
    width 100px
    height 20px
    background #eee
  .btnGroup
    display block
    .left
      display inline-block
      margin-right 10px
    .right
      display inline-block
      margin-right 10px
    .rotate
      display inline-block
      margin-right 10px
    .down
      display inline-block
      margin-right 10px
    .con
      display inline-block
  .matrix
    border 2px solid #000
    padding 3px 1px 1px 3px
    width 220px
    height --webkit-fill-available
    display inline-block
    p
      width 220px
      height 22px
      margin 0
    b
      display block
      width 12px
      height 12px
      padding 2px
      border 2px solid #879372
      margin 0 2px 2px 0
      float left
      &:after
        content ''
        display block
        width 12px
        height 12px
        background #879372
        overflow hidden
    .c
      display block
      width 12px
      height 12px
      padding 2px
      border 2px solid #333
      margin 0 2px 2px 0
      float left
      &:after
        content ''
        display block
        width 12px
        height 12px
        background #555
        overflow hidden
</style>