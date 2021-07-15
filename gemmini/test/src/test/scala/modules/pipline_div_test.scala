package modules

import chisel3.iotesters.PeekPokeTester

class pipline_div_test(c: pipline_div) extends PeekPokeTester(c) {
    poke(c.io.en,true)
    poke(c.io.dividend,15)
    poke(c.io.div,3)
    step(1)
    
}