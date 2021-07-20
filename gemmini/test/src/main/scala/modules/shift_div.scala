
package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

class shift_div(val width:Int) extends Module {
      val io = IO(new Bundle {
        val dividend = Input(SInt(width.W))
        val div = Input(SInt(width.W))
        val result = Output(SInt(width.W))
        val load = Input(Bool())
        val valid = Output(Bool())
    })
    val count = RegInit(0.U((2*width).W))

    val remainder = Reg(SInt((2*width).W))
    val divs = Reg(SInt((2*width).W))
    val quo = RegInit(0.S(width.W))
    when(io.load)
    {
      count:=0.U
      divs:=io.div<<width
      remainder:=io.dividend
    }  
    .elsewhen(io.valid===0.B)
    {
      when(remainder>=divs)
      {
        remainder:=remainder-divs
        quo:=(quo<<1)+1.S
      }
      .otherwise
      {
        quo:=quo<<1
      }
      divs:=divs>>1
      count:=count+1.U
    }
    io.valid:= count===(width+1).U
    io.result:= quo
    // printf("%d\n",remainder)
    printf("%d %d\n",quo,io.valid)
}