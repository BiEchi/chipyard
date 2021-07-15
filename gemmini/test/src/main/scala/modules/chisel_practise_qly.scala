package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

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
    fetch.io <> decode.io
    decode.io <> execute.io 
    io<>execute.io
}



