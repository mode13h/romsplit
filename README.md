# romsplit
A quick and dirty tool written in C to split UEFI/BIOS files into two or more binary chunks, which can then be flashed onto a chip using flashrom or something similar.  This was born out of the necessity of needing a UEFI and an EC (embedded controller) image to be reflashed manually on a laptop where the UEFI was 2MB and the EC was 128K, but seperate images were not available.
