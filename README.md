# Mines

There are many implementations out there; this one is mine.

## Building

You will need:
- Qt 6
- C++ tools that support C++20 (mainline clang and gcc ca July 2024 suffice)
- CMake 3.21.1 or later
- Ninja

To build:

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH=/path/to/qt/version/arch/lib/cmake
cmake --build build
```

## Releasing

### macOS

To prepare an appstore-compliant build, add the following definitions to the cmake project:
```
-DCODESIGN_IDENTITY="your apple developer certificat name" -DMACOSX_SIGN_BUNDLE=ON
```

Then build as normal: `cmake --build build`

TODO: How to package for app store?

---

Licensed under GPL v3.0 or later, at your discretion.

Copyright 2024 Benjamin Bader
