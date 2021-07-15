package modules

import chisel3.iotesters.PeekPokeTester

class adder_tree_test(c: adder_array) extends PeekPokeTester(c) {
    for(i<-0 until 16)
    {
        poke(c.io.input_vec(i),i)
    }
    step(6)
    println("ans: "+peek(c.io.out).toString)

}