
package modules

import chisel3._
import chisel3.util._
import chisel3.experimental._
import scala.collection._

class vector_ALU(val width:Int) extends Module
{
    val io = IO(new Bundle {
        val in_vector_a = Input(Vec(8, SInt((width+4).W)))
        val in_vector_b = Input(Vec(8, SInt((width+4).W)))
        val circuit_selector = Input(SInt(2.W))  // only 0, 1, 2 are allowed
        
        val out_vector = Output(Vec(8, SInt((width+4).W)))  // for cs == 0/1/2
        val out_dotmul_value = Output(SInt(15.W))  // for cs == 2
    })

    when (io.circuit_selector === 0.S){ // ADD
        for(i <- 0 until 8){
            io.out_vector(i) := io.in_vector_a(i) + io.in_vector_b(i)
        }
        io.out_dotmul_value := 0.S
    } .elsewhen (io.circuit_selector === 1.S){ // SUB
        for(i <- 0 until 8){
            io.out_vector(i) := io.in_vector_a(i) - io.in_vector_b(i)
        }
        io.out_dotmul_value := 0.S
    } .otherwise { // MUL
        for(i <- 0 until 8){
            // dot producter, multiply two vectors
            io.out_vector(i) := io.in_vector_a(i) * io.in_vector_b(i)
        }
        val adder_tree_obj = Module(new adder_array(width+10, 8))
        adder_tree_obj.io.input_vec := io.out_vector
        io.out_dotmul_value := adder_tree_obj.io.out
    }

}
