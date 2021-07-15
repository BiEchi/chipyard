package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

// PS: at the beginning, parameter:length(nummber of register) is equal to the bitwidth 
class parallel_addition_tree(val length:Int,val bitwidth:Int) extends Module{
    val io = IO(new Bundle{
        val inputReg = Input(Vec(length,SInt(bitwidth.W)))
        val output = Output(SInt((2*bitwidth).W))
    })
    if(length <= 2){
        io.output := io.inputReg(0) + io.inputReg(1)
    }else{
        val precedent_part = Module(new parallel_addition_tree(length/2, bitwidth))
        val second_part = Module(new parallel_addition_tree(length/2, bitwidth))
        // println("length is " + length)
        for(i <- 0 until length/2){
            precedent_part.io.inputReg(i) := io.inputReg(i)
            second_part.io.inputReg(i) := io.inputReg(length/2+i)
        }
        io.output := precedent_part.io.output + second_part.io.output
    }
}

// mutable pipline multipitor
class pipline_mul(val bitwidth: Int) extends Module{
    val io = IO(new Bundle{
        val input_multiplicand = Input(SInt(bitwidth.W))
        val input_multiplier = Input(SInt(bitwidth.W))
        val output = Output(SInt((2*bitwidth).W))
    })
    val adder_tree = Module(new parallel_addition_tree(bitwidth,bitwidth))
    for(i <- 0 until bitwidth){
        when(io.input_multiplier(i)){
            adder_tree.io.inputReg(i) := io.input_multiplicand << i
            printf("the value is %d\n",adder_tree.io.inputReg(i)) 
        }.otherwise{
            adder_tree.io.inputReg(i) := 0.S(bitwidth.W)
        }
    }
    io.output := adder_tree.io.output
}