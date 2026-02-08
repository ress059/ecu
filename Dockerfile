# syntax=docker/dockerfile:1

# Layer 1: Ubuntu 24.04 - Noble base image.
FROM ubuntu@sha256:74f92a6b3589aa5cac6028719aaac83de4037bad4371ae79ba362834389035aa
WORKDIR /workspaces/ecu
ENV DEBIAN_FRONTEND=noninteractive LC_ALL=C

# Layer 2: Core packages. Python, pip, build-essential, etc.
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends \
    build-essential=12.10ubuntu1 \
    python3=3.12.3-0ubuntu2.1 \
    python3-pip \
    wget

# Layer 3: Set up certificates.
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends ca-certificates
RUN update-ca-certificates

# Layer 4: Install ARM GNU toolchain v14.3
RUN wget -P /opt https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    chmod 744 /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    tar -xf /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz -C /opt && \
    cp -r /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi/* /usr/local/ && \
    rm /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    rm -r /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi

# Layer 5: Install remaining project-specific tools from package manager
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends \
    clang-format=1:18.0-59~exp2 \
    cmake=3.28.3-1build7 \
    doxygen=1.9.8+ds-2build5 \
    gdb=15.0.50.20240403-0ubuntu1 \
    graphviz=2.42.2-9build1 \
    git \
    usbutils

# Layer 6: Install project-specific python packages.
# No need to create python virtual environment
# since we are already using docker container. --break-system-packages
# installs python packages system wide and prevents warning message 
# saying to use virtual environment.
COPY requirements.txt .
RUN pip install -r requirements.txt --break-system-packages