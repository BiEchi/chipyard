
package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

class vector_adder(val width:Int) extends Module
{
    val io = IO(new Bundle {
        val in_vector_a = Input(SInt(width.W))
        val in_vector_b = Input(SInt(width.W))
        
        val out_vector = Output(SInt(width.W))
    })   

    for

}