package modules

import chisel3.iotesters.PeekPokeTester

class shift_mul_test(c: shift_mul) extends PeekPokeTester(c) {
    poke(c.io.load,true)
    poke(c.io.mul1,5)
    poke(c.io.mul2,7)
    step(1)
    poke(c.io.load,false)
    step(10)
}