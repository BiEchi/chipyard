package modules

import chisel3.iotesters.PeekPokeTester

class vec_alu_test(c: vec_alu) extends PeekPokeTester(c) {
    for(i<-0 until 3)
    {
        poke(c.io.a_vec(i),i*2)
        poke(c.io.b_vec(i),i)
    }
    poke(c.io.control,1)
    step(1)

    
}