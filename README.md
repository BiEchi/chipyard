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

-   `math.h` header file.
-   `softmax()` implementation.
-   `normalize()` implementation.
-   `WS/OS/CPU` choices implementation.
-   `decoder()` part.
-   `sin()` for positional encoding.

