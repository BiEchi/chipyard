package modules

import chisel3._
import chisel3.iotesters._

class TesterSimple(dut: DeviceUnderTest) extends 
PeekPokeTester(dut){
    poke(dut.io.a, 0)
    poke(dut.io.b, 1)
    step(1)
    println("Result is " + peek(dut.io.out).toString)
}