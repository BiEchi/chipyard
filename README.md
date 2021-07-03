# CY Transformer Beta Version 0.4.1 Released

##  Installation Process

Please be sure to use the `docker` image instead of local installation, which consumes a HUGE time (unless you want to improve your *Linux* project arrangement ability). If you want to take a fast start do as exactly what is mentioned below:

1.  Use a Mac OS X system or Ubuntu Linux system.

2.  If you are a Mac OS user, please go to [the official docker website](https://www.docker.com/) to download the `docker` desktop.

3.  If you are a Linux user, please type 

    ```bash
    sudo apt-get install curl
    curl -fsSL https://get.docker.com -o get-docker.sh
    sudo sh get-docker.sh
    ```

4.  Then go to the place where you want to install *chipyard*, and type `sudo docker pull ucbbar/chipyard-image`. You will see something like this:

    ![image-20210630162242987](http://jacklovespictures.oss-cn-beijing.aliyuncs.com/2021-06-30-082243.png)

5.  At last you need to type `sudo docker run -it ucbbar/chipyard-image bash` and you’re all done.

Alternatively, if you insist on installing the image on your own computer, be sure to use a VPN to expedite downloading images from the overseas servers.

## What to do next

`sudo docker run -it ucbbar/chipyard-image bash`

After installation, please be sure you know about the basic workflow of the `gemmini` project. Please go to [this website](https://github.com/ucb-bar/gemmini) for detailed information.

## pull the docker images

`docker pull chipyard/chipyard_transformer`

## mount the git gemmini with docker container

https://github.com/BiEchi/chipyard

```shell
docker run -it --name=chipyard-transformer \
--mount type=bind,source="/Users/mac/Desktop/Research/chipyard"/gemmini,target=/root/chipyard/generators/gemmini chipyard/chipyard_transformer
```

