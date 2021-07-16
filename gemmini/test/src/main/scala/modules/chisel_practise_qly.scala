package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

// Attention: all of this file can become comment

class DeviceUnderTest extends Module{
    val io = IO(new Bundle {
        val a = Input(UInt(2.W))
        val b = Input(UInt(2.W))
        val out = Output(UInt(2.W))
    })
    io.out := io.a & io.b 
}

class CompA extends Module{
    val io = IO(new Bundle{
        val a = Input(UInt(8.W))
        val b = Input(UInt(8.W))
        val x = Output(UInt(8.W))
        val y = Output(UInt(8.W))
    })
}

class CompB extends Module{
    val io = IO(new Bundle{
        val in1 = Input(UInt(8.W))
        val in2 = Input(UInt(8.W))
        val out = Output(UInt(8.W))
    })
}

// connect component A, component B with component C
// 由层级可以看出是相互包含的关系
class compC extends Module{
    val io = IO(new Bundle{
        val in_a = Input(UInt(8.W))
        val in_b = Input(UInt(8.W))
        val in_c = Input(UInt(8.W))
        val out_x = Output(UInt(8.W))
        val out_y = Output(UInt(8.W))
    })

    val compA =  Module(new CompA())
    val compB =  Module(new CompB())
    compA.io.a := io.in_a 
    compA.io.b := io.in_b
    io.out_x := compA.io.x

    compB.io.in1 := compA.io.y 
    compB.io.in2 := io.in_c
    io.out_y := compB.io.out
}


// ALU:Buggy but can be used by jupyternotebook 
// class Mux4 extends Module {
//   val io = IO(new Bundle {
//     val a = Input(UInt(1.W))
//     val b = Input(UInt(1.W))
//     val c = Input(UInt(1.W))
//     val d = Input(UInt(1.W))
//     val sel = Input(UInt(2.W))
//     val y = Output(UInt(1.W))
//   })
//   io.y := 0.U
//   switch(io.sel) {
//     is("b00".U) {
//       io.y := io.a
//     }
//     is("b01".U) {
//       io.y := io.b
//     }
//     is("b10".U) {
//       io.y := io.c
//     }
//     is("b11".U) {
//       io.y := io.d
//     }
//   }
// }

class ALU extends Module{
    val io = IO(new Bundle{
        val input_a = Input(SInt(8.W))
        val input_b = Input(SInt(8.W))
        val fn = Input(UInt(2.W))
        val output_a = Output(UInt(8.W))
    })
    io.output_a := 0.U
    // io.fn match{
    //     case 0.U => io.output_a := io.input_a + io.input_b
    //     case 1.U => io.output_a := io.input_a - io.input_b
    //     case 2.U => io.output_a := io.input_a & io.input_b
    //     case 3.U => io.output_a := io.input_a | io.input_b
    //     case _ => 0.U
    // }
    // switch(io.fn)
    // {
    //     is(0.U){io.output_a := io.input_a + io.input_b}
    //     is(1.U){io.output_a := io.input_a - io.input_b}
    //     is(2.U){io.output_a := io.input_a & io.input_b}
    //     is(3.U){io.output_a := io.input_a | io.input_b}
    // }
}

class Fetch extends Module{
    val io = IO(new Bundle{
        val instr = Output(UInt(32.W))
        val pc = Output(UInt(32.W))
    })
}

class Decode extends Module{
    val io = IO(new Bundle{
        val instr = Input(UInt(32.W))
        val pc = Input(UInt(32.W))
        val aluOp = Output(UInt(5.W))
        val regA = Output(UInt(32.W))
        val regB = Output(UInt(32.W))
    })
}

class Execute extends Module{
    val io = IO(new Bundle{
        val aluOp = Input(UInt(5.W))
        val regA = Input(UInt(32.W))
        val regB = Input(UInt(32.W))
        val result = Output(UInt(32.W))
    })
}

class ResultCircuit extends Module{
    val io = IO(new Bundle{
    })
    val fetch = Module(new Fetch)
    val decode = Module(new Decode)
    val execute = Module(new Execute)
    // these modules are connected in series
    fetch.io <> decode.io
    decode.io <> execute.io 
    io<>execute.io
}


class chapt7 extends Module{
    io = IO(new Bundle{
        val input = Input(UInt(2.W))
    })
    val cond = io.input
    val w = WireDefault(0.U)
    // when{}.elsewhen{}.otherwise{} -> form a series of mux in line
    when(cond){
        w:=3.U
    }
}

class chapt8 extends Module{
    io = IO(new Bundle{
        val N = input(UInt(8.W))
    })
    val q = RegNext(d)
    // RegNext always has one cycle delay(output the state in precedent cycle)
    val enableReg = RegInit(0.U(4.W))
    when(enable){
        enableReg := inVal
    }
    // detecting the posedge
    val risingEdge = din &! RegNext(din)
    // val risingEdge = din &! Reg(din)

    // counter
    val cntReg = RegInit(0.U(8.W))
    cntReg:= cntReg + 1.U
    when(cntReg === io.N){
        cntReg := 0.U
    }
    // which is equal to 
    cntReg := Mux(cntReg === io.N, 0.U, cntReg + 1.U)
    // counting down
    cntReg := Mux(cntReg === 0, io.N, cntReg - 1.U)

    // generating a counter
    def genCounter(n:Int) = {
        val cntReg = RegInit(0.U(8.W))
        cntReg := Mux(cntReg === n.U, 0.U, cntReg + 1.U)
        cntReg
    }

    // Specific timing circuit
    val tickCounterReg = RegInit(0.U(4.W))
    val tick = tickCounterReg === (N-1).U
    tickCounterReg := tickCounterReg+1.U
    val lowFrequCntReg = RegInit(0.U(4.W))
    // trigger the accident by a fixed frequency, a slow logic
    when(tick){
        tickCounterReg := 0.U 
        lowFrequCntReg := lowFrequCntReg + 1.U 
    }
    // make some optimizion to the counter
    // from (N-2) to -1, then we need only the most significant bit to decide whether the counter needed to be reseted
    // decreasing the cotenent of 
}


