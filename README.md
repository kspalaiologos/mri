# mri
minecraft region interchange - compressing minecraft savefiles with bzip3.

# usage
in the directory with server jars, to compress `world/` to `world.mri.bz3`:

```bash
~/minecraft$ ./mri-compress world >/dev/null
```

decompressing `world.mri.bz3` to the `world/` directory (assuming it does not exist yet):
```bash
~/minecraft$ ./mri-decompress world >/dev/null
```

# algorithm

mri will decompress all gzip-compressed files in the world directory (as reported by `file`) and decompress all zlib hunks inside `.mca` files using `regiond`. then, all the files will be tarballed and compressed with bzip3 using the largest available block size.

# benchmarks

the question whether bzip3 is the approperiate compressor for minecraft save files might be lingering already. having downloaded a few maps off planetminecraft, in particular focusing on [imperial city](https://www.planetminecraft.com/project/monumental-imperial-city/), i arrive at the following benchmarks (**no** mri, best compression):

```
210974720 imperial.tar
127757638 imperial.tar.bz2
123932783 imperial.tar.bz3
128621738 imperial.tar.lz4
124502377 imperial.tar.lzma
125018258 imperial.tar.zst
125955225 imperial.ppmd
```

bzip3 is the best compressor in this benchmark, however, the unpacking pass that mri adds is necessary to accomplish a respectable good compression ratio:

```
1858068480 imperial.mri
 110721907 imperial.mri.bz2
  89481940 imperial.mri.bz3
 129550691 imperial.mri.gz
 151343785 imperial.mri.lz4
  96153716 imperial.mri.lzma
  97547776 imperial.mri.ppmd
```
