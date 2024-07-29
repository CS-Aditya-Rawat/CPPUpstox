# Upstox API C++ Client

## Overview

The unoficial C++ client for communicating with [Upstox Uplink API](https://upstox.com/developer/api-documentation/).

CPPUpstox is header-only library that wraps around Upstox Uplink REST API. It saves you the hassle of directly communicating with the APIs and provides an easy to use, native and modern C++ interface.

## Dependencies
CPPUpstox requires C++17 and following dependencies
- [OpenSSL (devel)](https://github.com/openssl/openssl "OpenSSL")
- [fmt](https://github.com/fmtlib/fmt "Fmt")

## Getting dependencies
- On Arch Linux:
`sudo pacman -S openssl`
- On Fedora 32:
`sudo dnf install openssl-devel`
- On Ubuntu:
`sudo apt install libssl-dev`

## Building & Installation

Clone the repository

```sh
https://github.com/CS-Aditya-Rawat/CPPUpstox.git
```

CPPUpstox is a header-only library. Copy the `include` folder to system or project's include path.

## Building
```bash
mkdir build && cd build
cmake .. <other-options>
make
```