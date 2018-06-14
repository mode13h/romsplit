# romsplit
A quick and dirty tool written in C to split UEFI/BIOS files into two or more binary chunks, which can then be flashed onto a chip using flashrom or something similar.  This was born out of the necessity of needing a UEFI and an EC (embedded controller) image to be reflashed manually on a laptop where the UEFI was 2MB and the EC was 128K, but seperate images were not available.

# Usage:

romsplit <filename> [size of chunk 1] ... [size of chunk n]

Chunk sizes can be specified in megabytes or kilobytes, e.g. 2048M or 128K will produce two chunks of 2 megabytes and 128 kilobytes, respectively.  If no unit is specified, then bytes are assumed.

This tool comes with absolutely no warranty; use entirely at your own risk!
