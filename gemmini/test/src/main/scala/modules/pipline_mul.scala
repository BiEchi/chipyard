package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

// PS: at the beginning, parameter:length(nummber of register) is equal to the bitwith 
class parallel_addition_tree(val length:Int,val bitwith:Int) extends Module{
    val io = IO(new Bundle{
        val inputReg = Input(Vec(length,SInt(bitwith.W)))
        val output = Output(SInt((2*bitwith).W))
    })
    when((length <= 2).B){
        io.output := io.inputReg(0) + io.inputReg(1)
    }.otherwise{
        printf("start call 1 !\n")
        val precedent_part = Module(new parallel_addition_tree(length/2, bitwith))
        printf("start call 2 !")
        val second_part = Module(new parallel_addition_tree(length/2, bitwith))
        printf("start config")
        for(i <- 0 until length/2){
            precedent_part.io.inputReg(i) := io.inputReg(i)
            second_part.io.inputReg(i) := io.inputReg(length/2+i)
        }
        println("breakpoint2")
        io.output := precedent_part.io.output + second_part.io.output
    }
}

// mutable pipline multipitor
class pipline_mul(val bitwith: Int) extends Module{
    val io = IO(new Bundle{
        val input_multiplicand = Input(SInt(bitwith.W))
        val input_multiplier = Input(SInt(bitwith.W))
        val output = Output(SInt((2*bitwith).W))
    })
    println("allocate the adder tree module")
    val adder_tree = Module(new parallel_addition_tree(bitwith,bitwith))
    println("mulStart!")
    for(i <- 0 until bitwith){
        when(io.input_multiplier(i)){
            adder_tree.io.inputReg(i) := io.input_multiplicand << i
        }.otherwise{
            adder_tree.io.inputReg(i) := 0.S(bitwith.W)
        }
    }
    io.output := adder_tree.io.output
}