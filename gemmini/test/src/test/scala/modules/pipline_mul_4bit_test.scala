package modules

import chisel3._
import chisel3.iotesters.PeekPokeTester

class pipline_mul_4bit_tester(dut: pipline_mul_4bit) extends PeekPokeTester(dut){
    poke(dut.io.input_a,-4)
    poke(dut.io.input_b,3)
    println("the inputA is " + peek(dut.io.input_a).toString)
    println("the inputB is " + peek(dut.io.input_b).toString)
    println("the result is " + peek(dut.io.output).toString)
    step(1)
    println("the inputA is " + peek(dut.io.input_a).toString)
    println("the inputB is " + peek(dut.io.input_b).toString)
    println("the result is " + peek(dut.io.output).toString)
    step(1)
    println("the inputA is " + peek(dut.io.input_a).toString)
    println("the inputB is " + peek(dut.io.input_b).toString)
    println("the result is " + peek(dut.io.output).toString)
    step(1)
    println("the inputA is " + peek(dut.io.input_a).toString)
    println("the inputB is " + peek(dut.io.input_b).toString)
    println("the result is " + peek(dut.io.output).toString)
}