package modules

import chisel3._
import chisel3.iotesters.PeekPokeTester

class pipline_mul_tester(dut: pipline_mul) extends PeekPokeTester(dut){
    poke(dut.io.input_multiplicand,4)
    poke(dut.io.input_multiplier,3)
    println("the inputA is " + peek(dut.io.input_multiplicand).toString)
    println("the inputB is " + peek(dut.io.input_multiplier).toString)
    println("the result is " + peek(dut.io.output).toString)
    step(1)
}