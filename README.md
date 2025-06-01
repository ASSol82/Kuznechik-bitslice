# Kuznechik_bitslice

Bitslice (bit-parallel) implementation of the Grasshopper algorithm from GOST 34.12—2015

## Compilation

### Linux

Download the repository to your local disk and go to the **Streebog-bitslice** directory.

At the command prompt from **Kuznechik-bitslice** directory, type

**make**

After running make, a file named **Kuznechik_bitslice_test** should be created in the current directory.

### In other operating systems

Compile files from the repository in a convenient way.

## Run

### Linux

At the command prompt, type for example

**./Kuznechik_bitslice_test**

This example implements encryption of 2^24 plaintext blocks in ECB mode.

