# Chipyard Transformer Accelerator

This is a test repo for chipyard greenhands.

## pull the docker images

`docker pull chipyard/chipyard_transformer`

## mount the git gemmini with docker container

https://github.com/BiEchi/chipyard

```shell
docker run -it --name=chipyard-transformer-software \
--mount type=bind,source="/Users/mac/Desktop/Research/chipyard"/gemmini,target=/root/chipyard/generators/gemmini chipyard/chipyard_transformer:software
```

## TODO

### Software (by *C*)

| TODO                                | Executer    | Day SPENT | PROCESS |
| ----------------------------------- | ----------- | --------- | ------- |
| `math.h` header file.               | Hao BAI     | 0         | DONE    |
| `softmax()` implementation.         | Liyang QIAN | 3         | PEND    |
| `normalize()` implementation.       | Liyang QIAN | 3         | PEND    |
| `WS/OS/CPU` choices implementation. | Hao BAI     | DONE      | DONE    |
| `sin()` for positional encoding.    | Liyang QIAN | 3         | DONE    |
| `decoder()` part.                   | Wentao YAO  | 3         | DONE    |

### Hardware (by *CHISEL*)

| TODO                                    | Executer    | Day SPENT | PROCESS |
| --------------------------------------- | ----------- | --------- | ------- |
| `pipeline_divisor` implementation       | Wentao YAO  | 6         | START   |
| `vector_ALU` implementation             | Hao BAI     | 3         | START   |
| `pipeline_multiplicator` implementation | Liyang QIAN | 4         | START   |

### Verliog Learing Web
https://hdlbits.01xz.net/wiki/Main_Page

### Chisel Textbook:
search chisel-book in github

## FIXME

| FIXME                         | Executer    | Day SPENT | STATUS |
| ----------------------------- | ----------- | --------- | ------ |
| `normalize()` implementation. | Liyang QIAN | 3         | PEND   |
|                               |             |           |        |

## Our Working Time

| Time          | Name        |
| ------------- | ----------- |
| 9-7-6         | Liyang QIAN |
| 10-7-6        | Wentao YAO  |
| 9-5-6 / 9-9-5 | Hao BAI     |

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

