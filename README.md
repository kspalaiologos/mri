# mri
minecraft region interchange - compressing minecraft savefiles with bzip3.

# usage
in the directory with server jars, to compress `world/` to `world.tar.bz3`:

```bash
~/minecraft$ ./mri-compress world >/dev/null
```

decompressing `world.tar.bz3` to the `world/` directory (assuming it does not exist yet):
```bash
~/minecraft$ ./mri-decompress world >/dev/null
```

# algorithm

mri will decompress all gzip-compressed files in the world directory (as reported by `file`) and decompress all zlib hunks inside `.mca` files using `regiond`. then, all the files will be tarballed and compressed with bzip3 using the largest available block size.
