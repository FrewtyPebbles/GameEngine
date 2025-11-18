# About Render Backends

This engine uses 2 different Vulkan powered rendering backends:

 - **Compatibility** : This backend has the most platform support and uses the MoltenVK translation layer for MacOS and IOS support.

 - **Progressive** : This backend has less platform support (Notably missing IOS and MacOS), but has the best performance and support for rendering features.

## Notice

 > At release we will likely only have the Progressive backend supported.