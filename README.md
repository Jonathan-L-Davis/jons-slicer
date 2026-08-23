# My slicer

## Plans

The plan is to build a slicer core lib that can be built as a static or dynamic library. Then to wrap that in both a CLI tool & a GUI tool.

Also planning to make a plugin framework for modifying the workflow of slicing by 3rd parties easier.

## Building
Run the ./build.sh script from the repo root directory. It should automatically initialize repositories & build dependencies.

    ./build.sh --build-deps

Will build all dependencies & the main library.
If you are building for the first time, expect it to take a long time for the script to pull the submodules & build them.

## Licensing
All my code is MIT because I strongly prefer that license.

Submodules are licensed according to the licenses found in their repo's.

