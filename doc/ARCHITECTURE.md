Our project consists of 3 main modules:

* compressor/decompressor
* memory expansion manager
* address translator

1. Compressor/decompressor

The most valuable parts of the project. We plan to provide only those parts in MVP: that's it, hardware compressor/decompressor with random-access capabilities.

Memory content is compressed at the page granularity. When the page is compressed, metadata is created to locate data blocks. A compressed memory page will not occupy an entire physical page frame. The free space will be managed by memory expansion manager.

We will try to exploit statistical coding, basically Huffman code. For random access there are some options too; we will choose/create the right one.

There is an open question about when it's better to do compression, possible options include compressing in background always or providing sth like a swap: compression will be enabled only when there is a memory pressure.

2. Memory expansion manager
The main purpose of the module: provide additional virtual memory pages to operating system. Additional pages come from compressor: we store unused blocks somehow, then they form virtual pages.

For now we are thinking about software-driver that will provide virtual pages to os; this topic is quite huge so we don't include it into our MVP.

3. Address translator
Hopefully, the simplest part of the show.

Its main purpose: when a write request hits, it must translate provided address into address in compressed memory; then block located in the address is passed to decompressor, which provides real data, which gets overwritten; at the end data is compressed back.
When a read request hits, situation is more interesting. We plan to provide an opportunity to perform random-access reading of compressed memory, thus we can read data from the addressed block, without decompressing the whole page.
