
package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

class div_pe(val width:Int) extends Module
{
    val io = IO(new Bundle {
        val in_bit = Input(SInt(width.W))
        val remainder_in = Input(SInt((2*width).W))
        val div_in = Input(SInt((2*width).W))
        val in_valid = Input(Bool())

        val remainder_out = Output(SInt((2*width).W))
        val div_out = Output(SInt((2*width).W))
        val out_valid = Output(Bool())
        val out_bit = Output(SInt(width.W))
    })    
    when(io.remainder_in>=io.div_in)
    {
        io.remainder_out:=io.remainder_in-io.div_in
        io.out_bit:=(io.in_bit<<1)+1.S 
    }
    .otherwise
    {
        io.remainder_out:=io.remainder_in
        io.out_bit:=io.in_bit<<1
    }
    io.div_out:=io.div_in>>1
    io.out_valid:=io.in_valid
}

class pipline_div(val width:Int) extends Module
{
    val io = IO(new Bundle{
        val dividend = Input(SInt(width.W))
        val div = Input(SInt(width.W))
        val result = Output(SInt(width.W))
        val en = Input(Bool())
        val valid = Output(Bool())
    })
    val alu_array = Seq.fill(width+1)(Module(new div_pe(width)))

    val remainders = Seq.fill(width)(RegInit(0.S))
    val divs = Seq.fill(width)(RegInit(0.S))
    val bits = Seq.fill(width)(Reg(SInt(width.W)))
    val valids = Seq.fill(width)(RegInit(0.B))

    alu_array(0).io.remainder_in:=io.dividend
    alu_array(0).io.div_in:=io.div<<width
    alu_array(0).io.in_bit:=0.S 
    alu_array(0).io.in_valid:=io.en
    for(i<-0 until width)
    {
        remainders(i):=alu_array(i).io.remainder_out
        divs(i):=alu_array(i).io.div_out
        bits(i):=alu_array(i).io.out_bit
        valids(i):=alu_array(i).io.out_valid

        alu_array(i+1).io.remainder_in:= remainders(i)
        alu_array(i+1).io.div_in:= divs(i)
        alu_array(i+1).io.in_bit:= bits(i)
        alu_array(i+1).io.in_valid:=valids(i)

        // alu_array(i+1).io.remainder_in:=alu_array(i).io.remainder_out
        // alu_array(i+1).io.div_in:=alu_array(i).io.div_out
        // alu_array(i+1).io.in_bit:=alu_array(i).io.out_bit
        // alu_array(i+1).io.in_valid:=alu_array(i).io.out_valid
    }

    io.result:=alu_array(width).io.out_bit
    io.valid:=alu_array(width).io.out_valid
    // printf("\nThe result of %d/%d is:%d\n\n",io.dividend,io.div,io.result)

    // for(i<-0 until width+1)
    // {
    //     printf("%d ",alu_array(i).io.out_bit)
    // }
    // printf("%d ",io.result)
    // printf("\n")


}