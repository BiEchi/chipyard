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

docker run -it --name=chipyard-transformer-ws-sentence \
--mount type=bind,source="/home/user/gemmini"/software,target=/root/chipyard/generators/gemmini/software chipyard_wzs:v2

docker run -it --name=chipyard-transformer-software \
--mount type=bind,source="/mnt/d/Research/chipyard"/gemmini,target=/root/chipyard/generators/gemmini chipyard/chipyard_transformer:software
```

### Use the software simulator

1. Spike

   ```
   spike --extension=gemmini <some/gemmini/baremetal/test>
   ```

2. verilator

   ```
   cd ~/chipyard/sims/verilator # or "sims/vcs"
   make CONFIG=GemminiRocketConfig
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

In this part, you write about the goals from each component.

### Overall Architechture

![OverallArchitecture](http://jacklovespictures.oss-cn-beijing.aliyuncs.com/2021-08-04-082749.png)

### Controller

1. In Controller, the rocc instruction from io.cmd will be unrolled, by loopConv function and loopMatMul function, to a pile of simple Rocc instructions (guess)  to control load,st,ex unit. The unrolled command will be store in the re-order buffer firstly  after being unrolled, decompose the instruction into fundamental instruction.
2. ROB unit as a queue, or buffer, do not relate to the compile process of cmd, so it maybe not be required to change, identified by inst_id.
3. Load,ex,st unit will drive the dma in Spad unit.
4. In load, ex, st controller will compile the rocc instruction into machine instruction.
5. The control have busy and interrupt status judgement and signals.

#### The schema of Controller

![image-20210809203038548](graphs/image-20210809203038548.png)

### PE + Tile + Mesh

#### PE：

 	1. the previous PE's out_c port will be connected to the next PE's in_d port according to the chisel code in Tile.scala
 	2. PE unit has Mac unit(function) to execute matrix multiplication and addition
 	3. Don't know where to store the weight when WS situation

#### Tile:

​	1. Connect the PE units in series by this form of code, see figure below、

#### Mesh:

​	1. Similar connection logic as Tile Unit

### Execute controller



#### code of Tile.scala to fulfill the connection in series

![PEunitForWSandOS](http://jacklovespictures.oss-cn-beijing.aliyuncs.com/2021-08-04-082812.png)

<img src="http://jacklovespictures.oss-cn-beijing.aliyuncs.com/2021-08-04-082818.png" alt="TileSeriesCode" style="zoom: 67%;" />



### Spad module

1. Io.srams will control the sp_bank directly, and also the io.acc

2. There is a vec_mul unit to calculate vector multiplication and write back to the sp_banks and acc_banks

3. Every reading instruction of sp_banks and acc_banks will feedback the result to writer

4. DMA: control the interaction with DRAM and TLB

5. The function of Reader: DRAM -> Spad by TLB + dma.rd.req.vaddr(virtual address), connect with load controller

6. The function of Writer:  Spad -> DRAM by TLB + dma.w.req.vaddr(virtual address), connect with store controller

7. Zero writer:

   1. A small module that update the value of local address, mask, last according to the req config
   2. function: write 0 into banks

   ![image-20210809194005184](graphs/image-20210809194005184.png)

   #### Unit of Spad

   ![image-20210809202858309](graphs/image-20210809202858309.png)

   ![image-20210809202923894](graphs/image-20210809202923894.png)

   ### Load controller, same as store controller

   1. It connects with the read input of DMA in spad, having the assignment to load data from DRAM into spad

   2. There is a cmd tracker to record and track the total byte for each RoccCmd if it has empty position

   3. The load controller uses the idea of FSM to config different signals and wires inside it. There are three states: waiting for cmd, waiting for dma and sending rows. 

   4. At waiting for cmd state, the controller will choose to config the stride, scale, shrink, block_stride arguments or change to next states according to the **funct** of Input RoccCmd. The cmd tracker will  buffer the dma if it has room, by storing the total byte of this command in cmd tracker, and return the position back to dma.req signal. If the cmd tracker doesn't have room to store cmd, the FSM will stay until there is a room. Then if the DMA unit of the spad has finished dealing with the request, then will the FSM go into sending_rows state or waiting for spad's DMA unit finish.

   5. At sending rows state, the machine will wait until the last row has been read into the spad.

   6. For row counter, the row counter is kept in a relative distance with the vaddr of cmd. It marks were the store address begin within the vaddr block decided by **vaddr** and **actual_read_rows** argument of one dma request.(one cmd)

   7. each time there is a dma resp, meaning a part of the instruction has been completed, then the cmd_tracker will update the byte needed to be written.

      ![image-20210809202345913](graphs/image-20210809202345913.png)

![image-20210809202356271](graphs/image-20210809202356271.png)

