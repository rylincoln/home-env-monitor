FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

# System packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    git \
    wget \
    xz-utils \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# TI SimpleLink CC32xx SDK v7.10 (copied from .devcontainer/sdk/)
COPY sdk/ /opt/ti/simplelink_cc32xx_sdk_7_10_00_13/

# ARM GNU Toolchain 15.2 Rel1 (bare-metal, multi-arch)
ARG TARGETARCH
RUN ARCH=$([ "$TARGETARCH" = "arm64" ] && echo "aarch64" || echo "x86_64") \
    && URL="https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-${ARCH}-arm-none-eabi.tar.xz" \
    && wget -q -O /tmp/gcc-arm.tar.xz "${URL}" \
    && mkdir -p /opt/arm-gnu-toolchain \
    && tar xf /tmp/gcc-arm.tar.xz -C /opt/arm-gnu-toolchain --strip-components=1 \
    && rm /tmp/gcc-arm.tar.xz

# FreeRTOS Kernel (pinned commit)
ARG FREERTOS_COMMIT=0f8efd98ccd1194a5ee3d9f25df6dad37416d5f4
RUN git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git /opt/FreeRTOS-Kernel \
    && cd /opt/FreeRTOS-Kernel \
    && git checkout ${FREERTOS_COMMIT}

# Environment variables matching Makefile ?= defaults
ENV GCC_ARMCOMPILER=/opt/arm-gnu-toolchain
ENV SDK_INSTALL_DIR=/opt/ti/simplelink_cc32xx_sdk_7_10_00_13
ENV FREERTOS_KERNEL=/opt/FreeRTOS-Kernel
