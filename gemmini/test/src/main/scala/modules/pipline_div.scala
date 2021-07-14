
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
    })
    val alu_array = Seq.fill(width+1)(Module(new div_pe(width)))
    alu_array(0).io.remainder_in:=io.dividend
    alu_array(0).io.div_in:=io.div<<width
    alu_array(0).io.in_bit:=0.S 
    alu_array(0).io.in_valid:=io.en
    for(i<-0 until width)
    {
        alu_array(i+1).io.remainder_in:=alu_array(i).io.remainder_out
        alu_array(i+1).io.div_in:=alu_array(i).io.div_out
        alu_array(i+1).io.in_bit:=alu_array(i).io.out_bit
        alu_array(i+1).io.in_valid:=alu_array(i).io.out_valid
    }
    io.result:=alu_array(width).io.out_bit
    printf("\nThe result of %d/%d is:%d\n\n",io.dividend,io.div,io.result)

    // printf("%d %d %d %d %d %d %d %d %d\n",alu_array(0).io.out_bit,alu_array(1).io.out_bit,alu_array(2).io.out_bit,alu_array(3).io.out_bit,alu_array(4).io.out_bit,alu_array(5).io.out_bit,alu_array(6).io.out_bit,alu_array(7).io.out_bit,alu_array(8).io.out_bit)

}