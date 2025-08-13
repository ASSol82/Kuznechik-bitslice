# Kuznechik bitslice

Bitslice (bit-parallel) implementation of the Kuznechik (Kuznyechik, Grasshopper) algorithm from GOST 34.12-2018

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

The test bench is represented by a computer with an Intel Core i5 8400 central processor. Running on a single core, without parallelization.

**./Kuznechik_bitslice_test**    ->  The encryption rate is 395 MBytes per second or, what is the same, 395*8=3160 Mbits per second.

## Author

Solovyev Anatoly Sergeevich

email: soloviov-anatoly@mail.ru

## LICENSE

The license is described in the file https://github.com/ASSol82/Kuznechik-bitslice/blob/main/LICENSE.md

## Comment

The link to this repository is located in a paper

**Anatoly Sergeevich Solovyev and Denis Bonislavovich Fomin
Bitsliced implementations of the «Streebog» and «Kuznechik» algorithms**

published at the CTCRYPT 2025 conference, https://ctcrypt.ru/files/files/2025/06/solovyev-fomim.pdf

