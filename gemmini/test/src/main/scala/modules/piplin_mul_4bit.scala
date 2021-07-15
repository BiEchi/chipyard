package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

// 4 bit pipline multipitor:Fundamental process
class pipline_mul_4bit extends Module{
    val io = IO(new Bundle{
        val input_a = Input(SInt(4.W))
        val input_b = Input(SInt(4.W))
        val output = Output(SInt(8.W))
    })
    printf("\nmulStart!\n")
    val adder01,adder23 = RegInit(0.S(8.W))
    // printf("the input_a is %d\n",io.input_a)
    // printf("the input_b is %d\n",io.input_b)
    val posReg1,posReg2,posReg3,posReg4 = RegInit(2.S(8.W))
    // printf("the value of inputb(0) is %d\n",io.input_b(0))

    when(io.input_b(0)){
        // printf("test!\n")
        posReg1:= io.input_a
        printf("the value of posRreg1 is %d\n",posReg1)
        printf("test result should be %d\n",Cat(0.S(4.W),io.input_a))
    }.otherwise{
        posReg1 := 0.S(8.W)
    }
    // printf("the value in posReg1 is %d\n",posReg1)
    when(io.input_b(1)){
        posReg2:= io.input_a << 1;
    }.otherwise{
        posReg2:= 0.S(8.W)
    }
    when(io.input_b(2)){
        posReg3:=io.input_a << 2;
    }.otherwise{
        posReg3:=0.S(8.W)
    }
    when(io.input_b(3)){
        posReg4:=io.input_a << 3;
    }.otherwise{
        posReg4:=0.S(8.W)
    }

    adder01 := posReg1 + posReg2
    adder23 := posReg3 + posReg4
    io.output := adder01 + adder23
    printf("the result is %d\n\n",io.output)
    // val addResult = Wire(Vec(2,Uval input_a = Input(SInt(8.W)))
    // val PosReg = Wire(Vec(4,Uval input_a = Input(SInt(8.W)))
    // // base division
    // when(io.input_b(0)){
    //     PosReg(0):= Cat(0.S(4.W),io.input_a)
    // }.otherwise{
    //     PosReg(0):= 0.S(8.W)
    // }
    // when(io.input_b(1)){
    //     PosReg(1):=Cat(0.S(3.W),io.input_a,0.S(1.W))
    // }.otherwise{
    //     PosReg(1):=0.S(8.W)
    // }
    // when(io.input_b(2)){
    //     PosReg(2):=Cat(0.S(2.W),io.input_a,0.S(2.W))
    // }.otherwise{
    //     PosReg(2):=0.S(8.W)
    // }
    // when(io.input_b(3)){
    //     PosReg(3) := Cat(0.S(1.W),io.input_a,0.S(3.W))
    // }.otherwise{
    //     PosReg(3) := 0.S(8.W)
    // }
    // // add tree
    // addResult(0):= PosReg(0) + PosReg(1)
    // addResult(1):= PosReg(2) + PosReg(3)
    // io.output := addResult(0) + addResult(1) 
}