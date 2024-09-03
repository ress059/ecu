# syntax=docker/dockerfile:1

# Ubuntu 22.04 - Jammy Jellyfish
FROM ubuntu@sha256:075680e983398fda61b1ac59ad733ad81d18df4bc46411666bb8a03fb9ea0195

# Misc dependencies layer - wget, xz-utils (unpack .tar.xz files), certificates.
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends wget \
    xz-utils \
    ca-certificates && \
    update-ca-certificates

# Layer for installing tools from Ubuntu package manager. We install native GNU 
# toolchain for running unit tests on the host.
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends clang-format-15 \
    make \
    gcc \
    g++ \
    cmake \
    python3 \
    doxygen \
    git

# ARM GNU toolchain layer.
# Manually download ARM GNU toolchain v13.3. We can't use Ubuntu package manager since the version is very old.
RUN cd /opt && \
    wget https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    chmod 744 arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    tar -xf arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    rm arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ar /usr/bin/arm-none-eabi-ar && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-as /usr/bin/arm-none-eabi-as && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++ && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ld /usr/bin/arm-none-eabi-ld && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-nm /usr/bin/arm-none-eabi-nm && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objdump /usr/bin/arm-none-eabi-objdump && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ranlib /usr/bin/arm-none-eabi-ranlib && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-readelf /usr/bin/arm-none-eabi-readelf && \
    ln -s /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size