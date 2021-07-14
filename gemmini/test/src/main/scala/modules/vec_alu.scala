
package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

class vec_pe(val width:Int) extends Module
{
    val io = IO(new Bundle {
        val in_a = Input(SInt(width.W))
        val in_b = Input(SInt(width.W))
        val control = Input(UInt(2.W))
        val valid_in = Input(Bool())
        val partial_in = Input(SInt(width.W))

        val valid_out = Output(Bool())
        val out = Output(SInt(width.W))
    })    
    val out_reg = Reg(SInt(width.W))
    val valid_reg = Reg(Bool())
    val addin = Reg(SInt(width.W))

    when(io.control===0.U)
    {
        when(io.valid_in)
        {
            out_reg:=io.in_a+io.in_b
            io.out:=out_reg
            io.valid_out:=io.valid_in
        }
        .otherwise
        {
            io.out:=0.S 
            io.valid_out:=0.B 
        }
    }
    .elsewhen(io.control===1.U)
    {
        when(io.valid_in)
        {
            out_reg:=io.in_a-io.in_b
            io.out:=out_reg
            io.valid_out:=io.valid_in
        }
        .otherwise
        {
            io.out:=0.S 
            io.valid_out:=0.B 
        }
    }
    .otherwise
    {
        when(io.valid_in)
        {
            io.out:=io.in_a*io.in_b+io.partial_in
            io.valid_out:=1.B
        }
        .otherwise
        {
            io.valid_out:=0.B 
            io.out:=0.S 
        }

    }

}

class vec_alu(val width:Int,val num:Int) extends Module{
    val io = IO(new Bundle {
        val a_vec = Input(Vec(num,SInt(width.W)))
        val b_vec = Input(Vec(num,SInt(width.W)))
        val control = Input(UInt(2.W))
        val out = Output(Vec(num,SInt(width.W)))
        val out_mul = Output(SInt(8.W))
    })    
    val pe_array = Seq.fill(num)(Module(new vec_pe(width)))

    for(i<-0 until num)
    {
        pe_array(i).io.in_a:=io.a_vec(i)
        pe_array(i).io.in_b:=io.b_vec(i)
        pe_array(i).io.control:=io.control
        io.out(i):=pe_array(i).io.out
    }
    pe_array(0).io.valid_in:=1.B
    pe_array(0).io.partial_in:=0.S
    for(j<-0 until num-1)
    {
        pe_array(j+1).io.valid_in:=pe_array(j).io.valid_out
        pe_array(j+1).io.partial_in:=pe_array(j).io.out
    }
    io.out_mul:=pe_array(num-1).io.out

    printf("%d\n",io.out_mul)

}

