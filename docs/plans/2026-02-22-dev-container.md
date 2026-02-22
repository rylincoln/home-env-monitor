# Dev Container Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Create a VS Code dev container that provides a fully self-contained build environment for the CC3220SF firmware.

**Architecture:** Single-stage Dockerfile based on Ubuntu 22.04. The TI SDK is COPY'd from a local directory, ARM GCC is downloaded from ARM's CDN, and FreeRTOS-Kernel is cloned at a pinned commit. Environment variables match the Makefile's `?=` defaults so `make` works with zero arguments.

**Tech Stack:** Docker, VS Code Dev Containers, ARM GNU Toolchain 15.2, TI SimpleLink CC32xx SDK v7.10

---

### Task 1: Create .dockerignore

**Files:**
- Create: `.dockerignore`

**Step 1: Write .dockerignore**

```
build/
.devcontainer/sdk/
.git/
.vscode/
*.bin
*.out
*.map
```

**Step 2: Commit**

```bash
git add .dockerignore
git commit -m "Add .dockerignore for dev container build context"
```

---

### Task 2: Create Dockerfile

**Files:**
- Create: `.devcontainer/Dockerfile`

**Step 1: Write the Dockerfile**

```dockerfile
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

# ARM GNU Toolchain 15.2 Rel1 (bare-metal, x86_64 Linux)
ARG ARM_GCC_URL=https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz
RUN wget -q -O /tmp/gcc-arm.tar.xz "${ARM_GCC_URL}" \
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
```

**Step 2: Commit**

```bash
git add .devcontainer/Dockerfile
git commit -m "Add Dockerfile for CC3220SF firmware build environment"
```

---

### Task 3: Create devcontainer.json

**Files:**
- Create: `.devcontainer/devcontainer.json`

**Step 3: Write devcontainer.json**

```json
{
    "name": "CC3220SF Firmware",
    "build": {
        "dockerfile": "Dockerfile",
        "context": "."
    },
    "workspaceFolder": "/workspaces/home-env-monitor",
    "customizations": {
        "vscode": {
            "extensions": [
                "ms-vscode.cpptools"
            ],
            "settings": {
                "C_Cpp.default.compilerPath": "/opt/arm-gnu-toolchain/bin/arm-none-eabi-gcc",
                "C_Cpp.default.cStandard": "c99",
                "C_Cpp.default.intelliSenseMode": "gcc-arm",
                "C_Cpp.default.includePath": [
                    "${workspaceFolder}/firmware",
                    "/opt/ti/simplelink_cc32xx_sdk_7_10_00_13/source",
                    "/opt/ti/simplelink_cc32xx_sdk_7_10_00_13/source/ti/posix/gcc",
                    "/opt/ti/simplelink_cc32xx_sdk_7_10_00_13/kernel/freertos",
                    "/opt/ti/simplelink_cc32xx_sdk_7_10_00_13/kernel/freertos/builds/cc32xx/release/pregenerated_configuration",
                    "/opt/FreeRTOS-Kernel/include",
                    "/opt/FreeRTOS-Kernel/portable/GCC/ARM_CM3"
                ],
                "C_Cpp.default.defines": [
                    "DeviceFamily_CC3220",
                    "_REENT_SMALL"
                ]
            }
        }
    },
    "postCreateCommand": "arm-none-eabi-gcc --version && make clean"
}
```

**Step 2: Commit**

```bash
git add .devcontainer/devcontainer.json
git commit -m "Add devcontainer.json with C/C++ IntelliSense config"
```

---

### Task 4: Update .gitignore

**Files:**
- Modify: `.gitignore`

**Step 1: Add SDK directory exclusion**

Add `.devcontainer/sdk/` to `.gitignore` so the 939MB SDK is never committed.

**Step 2: Commit**

```bash
git add .gitignore
git commit -m "Ignore .devcontainer/sdk/ (TI SDK, user-provided)"
```

---

### Task 5: Verify — build the container and compile firmware

**Step 1: Copy SDK into place**

```bash
cp -r /Applications/ti/simplelink_cc32xx_sdk_7_10_00_13 .devcontainer/sdk/
```

**Step 2: Build the Docker image**

```bash
docker build -t env-monitor-dev .devcontainer/
```

Expected: Image builds successfully. ARM GCC downloaded, SDK copied, FreeRTOS cloned.

**Step 3: Run `make` inside the container**

```bash
docker run --rm -v "$(pwd)":/workspaces/home-env-monitor -w /workspaces/home-env-monitor env-monitor-dev make clean all
```

Expected: All 46 files compile, linker produces `build/env_monitor.bin` (~75KB), `arm-none-eabi-size` prints text/data/bss sizes.

**Step 4: Commit any fixups needed**

---

### Task 6: Update CLAUDE.md

**Files:**
- Modify: `CLAUDE.md`

**Step 1: Add dev container section**

Add a "Dev Container" section documenting:
- One-time SDK copy step
- How to open in VS Code
- How to build with plain Docker

**Step 2: Commit**

```bash
git add CLAUDE.md
git commit -m "Document dev container setup in CLAUDE.md"
```
