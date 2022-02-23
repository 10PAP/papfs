Our project consists of 3 main modules:

* FUSE-interface, provides usual interfaces to work with files.

* Disk-manager: stores compressed representation and performs fast random access without decompressing large files.

* Compressor: approximates probability distribution to exploit repetitions and compress efficiently.

1. FUSE-interface

The "frontend" part of our filesystem: provides interfaces for working with files: open/write/read/close. Other applications will communicate with our system through this module using FUSE interface. It allows applications to work with out filesystem transparently.

2. Disk-manager

Uses an actual, "usual" filesystem under the hood. Processes queries from FUSE module, using compressed information and some additional data stored in a separate file. Works as database engine you can say.

3. Compressor

Searches for similarities, repetitions and other interesting things in data to perform high-quality compression. Creates a model to approximate real probability distribution of initial data to compress. It provides disk-manager with compressed representation of initial data together with additional information. This additional info can be used by disk-manager to provide fast random and sequential access.
