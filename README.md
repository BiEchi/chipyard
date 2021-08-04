## Chipyard Transformer Accelerator

This is a test repo for chipyard greenhands.

## Environment Construction

### pull the docker images

`docker pull chipyard/chipyard_transformer`

### mount the git gemmini with docker container

https://github.com/BiEchi/chipyard

```shell
docker run -it --name=chipyard-transformer-software \
--mount type=bind,source="/Users/mac/Desktop/Research/chipyard"/gemmini,target=/root/chipyard/generators/gemmini chipyard/chipyard_transformer:software
```

### Test Functionality

```shell
#!/bin/bash
# You're now at ~/chipyard/generators/gemmini/test
sbt "test:runMain modules.Launcher chiselPra_qly" # or other submodule you want to build
```

---

## Learning Resources

### CHISEL Learning Resources

[CSE 293 Agile Hardware Design](https://classes.soe.ucsc.edu/cse293/Spring21/references/chisel/)

[Jupyter Notebook Script](https://github.com/agile-hw/lectures)

### Verliog Learing Web

https://hdlbits.01xz.net/wiki/Main_Page

---

## Project Process

### TODO

#### Software (by *C*)

| TODO                                            | Executer    | Day SPENT | PROCESS |
| ----------------------------------------------- | ----------- | --------- | ------- |
| `math.h` header file.                           | Hao BAI     | 1         | DONE    |
| `softmax()` implementation.                     | Liyang QIAN | 3         | PEND    |
| `normalize()` implementation.                   | Liyang QIAN | 3         | DONE    |
| `WS/OS/CPU` choices implementation.             | Hao BAI     | DONE      | DONE    |
| `sin()` for positional encoding.                | Liyang QIAN | 3         | DONE    |
| `decoder()` part.                               | Wentao YAO  | 3         | DONE    |
| `segmented_linear_exponential.c` implementation | Wentao YAO  | 3         | DONE    |
| `segmented_linear_sqrroot.py` implementation    | Liyang QIAN | 3         | DONE    |
|                                                 |             |           |         |

#### Hardware (by *CHISEL*)

| TODO                                             | Executer    | Day SPENT | PROCESS |
| ------------------------------------------------ | ----------- | --------- | ------- |
| `pipeline_divisor` implementation                | Wentao YAO  | 6         | DONE    |
| `vector_ALU` implementation                      | Hao BAI     | 3         | DONE    |
| `pipeline_multiplicator` implementation          | Liyang QIAN | 4         | DONE    |
| apply `segmented_linear_exponential` to hardware | Wentao YAO  | 7         | ON      |
| apply `segmented_linear_sqrroot` to hardware     | Hao BAI     | 7         | ON      |
| Read the source code                             | Liyang QIAN | 7         | ON      |
|                                                  |             |           |         |

### FIXME

| FIXME | Executer | Day SPENT | STATUS |
| ----- | -------- | --------- | ------ |
|       |          |           |        |
|       |          |           |        |

### Our Working Time

| Time          | Name        |
| ------------- | ----------- |
| 9-7-6         | Liyang QIAN |
| 10-7-6        | Wentao YAO  |
| 9-5-6 / 9-9-5 | Hao BAI     |

---

## Working References

### Layer Normalization

In our code, it shows as:

![LayerNormalization](http://jacklovespictures.oss-cn-beijing.aliyuncs.com/2021-07-09-105533.png)

According to our analysis, we gain the formula
$$
Layer\ Normalization(x_i)=\alpha\times\frac{x_i-\mu_L}{\sqrt{\sigma_L^2+\epsilon}}+\beta
$$
Where $\alpha$, $\beta$ and $\epsilon$ are the super-parameters we deliver to the Layer Normalization Process; $\mu_L$ and $\sigma_L$ are the mean value and standard deviation of the LAYER, and $x_i$ is the value of the $i^{th}$ value of the current layer. To explain in graphs, we have:

![LayerNormalizationDetailed](http://jacklovespictures.oss-cn-beijing.aliyuncs.com/2021-07-09-111223.png)

### Fix `normalization()`

Used `segmented linear` function to substitute the `while` statements.

### Problem With Software-simulated Approach `chipyard-verilator`

Using `spike` software, we do not know how much time clocks it takes for a process. We could only simulate the result (structure).

We used `chipyard-verilator` to transform `chisel` into `verilog`, in order to simulate the time clocks. However, this approach is not exact. The estimated clocks should be 1,600, but the carried-out clocks is 1,300. The `chipyard-verilator` uses `verilator` to transform `chisel` into `verilog`, then run `verilog` on the simulated `SoC-chip` (hardware part). We then used `build.sh` to compile `c` files using `risc-v` (software part). Also, using this approach, the software compilation part is a bit too slow for PCs.

### Switch `chisel` to `verilog`

This process can be found according to `chipyard-document`.

### Read the source code of `gemmini`

Read all the source code files in `/gemmini/src`.



## Gemmini Architecture

### PE

```scala
// Scala uses packages to create namespaces which allow you to modularize programs.
package gemmini

import chisel3._
import chisel3.util._

class PEControl[T <: Data : Arithmetic](accType: T) extends Bundle {
  val dataflow = UInt(1.W) // TODO make this an Enum
  val propagate = UInt(1.W) // Which register should be propagated (and which should be accumulated)?
  val shift = UInt(log2Up(accType.getWidth).W) // TODO this isn't correct for Floats

  override def cloneType: PEControl.this.type = new PEControl(accType).asInstanceOf[this.type]
}

// TODO update documentation
/**
  * A PE implementing a MAC operation (multi-accumulate operation). Configured as fully combinational when integrated into a Mesh.
  * @param width Data width of operands
  */
class PE[T <: Data](inputType: T, outputType: T, accType: T, df: Dataflow.Value, latency: Int, max_simultaneous_matmuls: Int)
                   (implicit ev: Arithmetic[T]) extends Module { // Debugging variables
  import ev._

  val io = IO(new Bundle {
    // in_a: the matrix row number
    val in_a = Input(inputType)
    // in_b : another input
    val in_b = Input(outputType)
    // preload matrix d
    val in_d = Input(outputType)
    val out_a = Output(inputType)
    val out_b = Output(outputType)
    // out_c will be connected to in_d
    val out_c = Output(outputType)

    val in_control = Input(new PEControl(accType))
    val out_control = Output(new PEControl(accType))

    val in_id = Input(UInt(log2Up(max_simultaneous_matmuls).W))
    val out_id = Output(UInt(log2Up(max_simultaneous_matmuls).W))

    val in_last = Input(Bool())
    val out_last = Output(Bool())

    val in_valid = Input(Bool())
    val out_valid = Output(Bool())

    val bad_dataflow = Output(Bool())
  })

  val cType = if (df == Dataflow.WS) inputType else accType

  val a  = ShiftRegister(io.in_a, latency) //a: input
  val b  = ShiftRegister(io.in_b, latency) //b: input
  val d  = ShiftRegister(io.in_d, latency)
  // c1,c2: preload
  val c1 = Reg(cType)
  val c2 = Reg(cType)
  val dataflow = ShiftRegister(io.in_control.dataflow, latency)
  val prop  = ShiftRegister(io.in_control.propagate, latency)
  val shift = ShiftRegister(io.in_control.shift, latency)
  val id = ShiftRegister(io.in_id, latency)
  val last = ShiftRegister(io.in_last, latency)
  val valid = ShiftRegister(io.in_valid, latency) // TODO should we clockgate the rest of the ShiftRegisters based on the values in this ShiftRegisters

  // forwarding a
  io.out_a := a
  io.out_control.dataflow := dataflow
  io.out_control.propagate := prop
  io.out_control.shift := shift
  io.out_id := id
  io.out_last := last
  io.out_valid := valid

  val last_s = RegEnable(prop, valid)
  val flip = last_s =/= prop
  val shift_offset = Mux(flip, shift, 0.U)

  // Which dataflow are we using?
  val OUTPUT_STATIONARY = Dataflow.OS.id.U(1.W)
  val WEIGHT_STATIONARY = Dataflow.WS.id.U(1.W)

  // Is c1 being computed on, or propagated forward (in the output-stationary dataflow)?
  val COMPUTE = 0.U(1.W)
  val PROPAGATE = 1.U(1.W)

  io.bad_dataflow := false.B
  when ((df == Dataflow.OS).B || ((df == Dataflow.BOTH).B && dataflow === OUTPUT_STATIONARY)) {
    when(prop === PROPAGATE) {
      io.out_c := (c1 >> shift_offset).clippedToWidthOf(outputType)
      // forwarding b
      io.out_b := b
      c2 := c2.mac(a, b.asTypeOf(inputType))
      c1 := d.withWidthOf(cType)
    }.otherwise {
      io.out_c := (c2 >> shift_offset).clippedToWidthOf(outputType)
      io.out_b := b
      c1 := c1.mac(a, b.asTypeOf(inputType))
      c2 := d.withWidthOf(cType)
    }
  }.elsewhen ((df == Dataflow.WS).B || ((df == Dataflow.BOTH).B && dataflow === WEIGHT_STATIONARY)) {
    when(prop === PROPAGATE) {
      io.out_c := c1
      // mac is a multiplication-add module, guess: b.mac(a,c) -> b = b + a*c
      io.out_b := b.mac(a, c2.asTypeOf(inputType))
      c1 := d
    }.otherwise {
      io.out_c := c2
      io.out_b := b.mac(a, c1.asTypeOf(inputType))
      c2 := d
    }
  }.otherwise {
    io.bad_dataflow := true.B
    //assert(false.B, "unknown dataflow")
    io.out_c := DontCare
    io.out_b := DontCare
  }

  when (!valid) {
    c1 := c1
    c2 := c2
  }
}

```

### Controller

```
	1. In Controller, the rocc instruction from io.cmd will be unrolled, by loopConv function and loopMatMul function, to a machine code to control load,st,ex unit. The unrolled command will be store in the re-order buffer firstly  after being unrolled.
	2. Load,ex,st unit will drive the dma in Spad unit.
	3. The control have busy and interrupt status judgement and signals.
	4. ROB unit as a queue, or buffer, do not relate to the compile process of cmd, so it maybe not be required to change
```

#### The schema of Controller

![ControllerOrginization](D:\Research\chipyard\graphs\ControllerOrginization.png)



### PE + Tile + Mesh

```
PE：
	1. the previous PE's out_c port will be connected to the next PE's in_d port according to the chisel code in Tile.scala
	2. PE unit has Mac unit(function) to execute matrix multiplication and addition
	3. Don't know where to store the weight when WS situation
Tile:
	Connect the PE units in series by this form of code, see figure below
Mesh:
	Similar connection logic as Tile Unit
```

#### code of Tile.scala to fulfill the connection in series



### Zero writer

```
	1. A small module that update the value of local address, mask, last according to the req config
```

#### 





