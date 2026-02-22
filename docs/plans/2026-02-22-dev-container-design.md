# Dev Container Design for CC3220SF Firmware

## Goal

Isolate the firmware build environment (ARM GCC cross-compiler, TI SimpleLink SDK, FreeRTOS Kernel) into a reproducible dev container. Build-only scope — flashing remains on the host via UniFlash/CCS.

## Files

```
.devcontainer/
  Dockerfile
  devcontainer.json
```

The TI SDK is placed at `.devcontainer/sdk/` before building (one-time copy from host). This directory is `.gitignore`'d and `.dockerignore`'d.

## Dockerfile

**Base:** `ubuntu:22.04`

**Layers (ordered for cache efficiency):**

1. System packages: `build-essential`, `make`, `git`, `wget`, `xz-utils`
2. TI SimpleLink CC32xx SDK v7.10: `COPY` from `.devcontainer/sdk/` to `/opt/ti/simplelink_cc32xx_sdk_7_10_00_13/` (~939MB, changes least, cached early)
3. ARM GNU Toolchain 15.2 Rel1: downloaded from ARM CDN (`developer.arm.com`), x86_64 Linux tarball, installed to `/opt/arm-gnu-toolchain/`
4. FreeRTOS Kernel: `git clone` at pinned commit `0f8efd98` to `/opt/FreeRTOS-Kernel/`

**Environment variables:**

- `GCC_ARMCOMPILER=/opt/arm-gnu-toolchain`
- `SDK_INSTALL_DIR=/opt/ti/simplelink_cc32xx_sdk_7_10_00_13`
- `FREERTOS_KERNEL=/opt/FreeRTOS-Kernel`

The Makefile reads these via `?=`, so `make` works with zero arguments inside the container.

## devcontainer.json

- **Extensions:** `ms-vscode.cpptools`
- **Settings:** C/C++ IntelliSense configured with container paths
- **Post-create command:** `make clean` to verify the toolchain
- **workspaceFolder:** `/workspaces/home-env-monitor`

## Setup Flow

1. One-time: `cp -r /Applications/ti/simplelink_cc32xx_sdk_7_10_00_13 .devcontainer/sdk/`
2. VS Code: "Reopen in Container"
3. Terminal: `make`

## .gitignore / .dockerignore

- `.devcontainer/sdk/` in `.gitignore`
- `build/` and `.devcontainer/sdk/` in `.dockerignore`

## Approach

Single-stage Dockerfile. Chosen over multi-stage (marginal caching benefit, more complexity) and Docker Compose + volume (extra setup, not self-contained). Dependencies change rarely, so simplicity wins.

## Pinned Versions

- ARM GNU Toolchain: 15.2.Rel1
- TI SimpleLink CC32xx SDK: v7.10.00.13
- FreeRTOS Kernel: commit `0f8efd98ccd1194a5ee3d9f25df6dad37416d5f4`
- Ubuntu: 22.04
