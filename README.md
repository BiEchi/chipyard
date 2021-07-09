# Chipyard Transformer Accelerator

This is a test repo for chipyard greenhands.

## pull the docker images

`docker pull chipyard/chipyard_transformer`

## mount the git gemmini with docker container

https://github.com/BiEchi/chipyard

```shell
docker run -it --name=chipyard-transformer \
--mount type=bind,source="/Users/mac/Desktop/Research/chipyard"/gemmini,target=/root/chipyard/generators/gemmini chipyard/chipyard_transformer
```

## Things TODO

| TODO                                | Executer    | Day SPENT |
| ----------------------------------- | ----------- | --------- |
| `math.h` header file.               | Hao BAI     | 0         |
| `softmax()` implementation.         | Liyang QIAN | 3         |
| `normalize()` implementation.       | Liyang QIAN | 3         |
| `WS/OS/CPU` choices implementation. | Hao BAI     | DONE      |
| `sin()` for positional encoding.    | Liyang QIAN | 3         |
| `decoder()` part.                   | Wentao YAO  | 3         |

## Our Working Time

| Time          | Name        |
| ------------- | ----------- |
| 9-7-6         | Liyang QIAN |
| 10-7-6        | Wentao YAO  |
| 9-5-6 / 9-9-5 | Hao BAI     |

