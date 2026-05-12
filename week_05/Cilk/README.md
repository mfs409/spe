# Cilk Demonstration
## Prerequisites
Install Cilk from [here](https://www.opencilk.org/doc/users-guide/install/)

## Compile Program 

Assuming that clang++ is the OpenCilk clang++ compiler 

clang++ {src_prog} -O3 -fopencilk

## Benchmark with Cilkscale

clang++ {src_prog} -fopencilk -fcilktool=cilkscale-benchmakr -O3 -o {src_prog}_bench

## Analyze Work/Span

Work-span snapshots can be created around regions of interest using wsp_getworkspan(), and stored with wsp_dump

clang++ {src_prog} -fopencilk -fcilktool=cilkscale -O3 -o {src_prog}_cs

## Display Performance Results

python3 /opt/opencilk/share/Cilkscale_vis/cilkscale.py -c ./{src_prog}_cs -b ./{src_prog}_bench -ocsv {output table name}.csv -oplot {plotname}.pdf --args {arguments to program}
