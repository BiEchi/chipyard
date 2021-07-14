package modules

import chisel3.iotesters.PeekPokeTester

class shift_div_test(c: shift_div) extends PeekPokeTester(c) {
    poke(c.io.load,true)
    poke(c.io.dividend,16)
    poke(c.io.div,3)
    step(1)
    poke(c.io.load,false)
    step(15)
}