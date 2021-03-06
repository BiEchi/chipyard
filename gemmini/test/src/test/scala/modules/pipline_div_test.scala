package modules

import chisel3.iotesters.PeekPokeTester

class pipline_div_test(c: pipline_div) extends PeekPokeTester(c) {
    poke(c.io.en,true)
    poke(c.io.dividend,11)
    poke(c.io.div,4)
    step(1)
    poke(c.io.en,true)
    poke(c.io.dividend,-16)
    poke(c.io.div,3)
    step(1)
    poke(c.io.en,true)
    poke(c.io.dividend,16)
    poke(c.io.div,4)
    step(6)
    println("The result of division is: " + peek(c.io.result).toString)
    step(1)
    println("The result of division is: " + peek(c.io.result).toString)
    step(1)
    println("The result of division is: " + peek(c.io.result).toString)
   
}