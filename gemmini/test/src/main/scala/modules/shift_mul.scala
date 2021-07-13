
package modules

import chisel3._

class shift_mul extends Module {
      val io = IO(new Bundle {
        val mul1 = Input(UInt(8.W))
        val mul2 = Input(UInt(4.W))
        val load  = Input(Bool())
        val out = Output(UInt(8.W))
        val valid = Output(Bool())
    })
    val count = RegInit(0.U(8.W))
    val mulcand = Reg(UInt(8.W))
    val mulper = Reg(UInt(4.W))
    val tempresult = RegInit(0.U(8.W))
    when(io.load)
    {
      mulcand:=io.mul1
      mulper:=io.mul2
      count:=0.U
    }
    .otherwise
    {
      when(mulper(0)===1.U)
      {
        tempresult:=tempresult+mulcand
      }
      mulcand:=mulcand<<1
      mulper:=mulper>>1
      count:=count+1.U
    }

    io.valid:= count===4.U
    io.out := tempresult  
    printf("%d %d\n",tempresult,io.valid);    
}