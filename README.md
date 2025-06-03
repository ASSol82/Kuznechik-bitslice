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

## Testbench

The test bench is represented by a computer with an Intel Core i5 8400 central processor.

**./Kuznechik_bitslice_test**    ->  The encryption rate is 395 MBytes per second or, what is the same, 395*8=3160 Mbits per second.

## Author

Solovyev Anatoly Sergeevich

## Licence

NonCommercial 

Only non commercial use.

## Comment

The link to this repository is located in a paper
**Anatoly Sergeevich Solovyev and Denis Bonislavovich Fomin
Bitsliced implementations of the «Streebog» and «Kuznechik» algorithms**
published at the CTCRYPT 2025 conference.


