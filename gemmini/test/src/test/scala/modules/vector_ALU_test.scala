package modules

import chisel3.iotesters.PeekPokeTester

class vector_ALU_test(c: vector_ALU) extends PeekPokeTester(c) {
    poke(c.io.circuit_selector, 2)
    for (i <- 0 until 8){
        poke(c.io.in_vector_a(i), i)
        poke(c.io.in_vector_b(i), i+1)
    }
    step(4)
    for (i <- 0 until 8){
        println("vector: " + peek(c.io.out_vector(i)).toString)
    }
    println("mul val: " + peek(c.io.out_dotmul_value).toString)
    step(2)
}
