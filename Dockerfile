# syntax=docker/dockerfile:1

# Ubuntu 24.10 - Oracular
FROM ubuntu@sha256:102bc1874fdb136fc2d218473f03cf84135cb7496fefdb9c026c0f553cfe1b6d
ENV DEBIAN_FRONTEND=noninteractive

# Layer for installing tools from Ubuntu package manager. We install native GNU 
# toolchain for running unit tests on the host.
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends binutils-arm-none-eabi \
    build-essential \
    clang-format \
    cmake \
    doxygen \
    gcc-arm-none-eabi \
    git \
    libnewlib-arm-none-eabi \
    python3 \
    python3-pip