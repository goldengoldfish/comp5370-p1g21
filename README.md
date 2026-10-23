# Disk Analyzer
**COMP 5370/6370 Computer and Network Security**
*Group 21*
Brandon Buckley, Jarod Jernigan, Izayah Tubigon
https://github.com/goldengoldfish/comp5370-p1g21
## Overview
`da` is a commandline-based tool for quickly scanning a disk image for forensic purposes. It is able to parse images of disks with an MBR and FAT16 partitions. Although the functionality of the current release is limited, the project was built with extensibility in mind.

## Usage
da finds partitions on the image and outputs information from them, as well as carving the partitions into the output directory.
```
da img.dd output/
```
### Sample Output
```
=== Partitions ===
Partition 0:
        Partition Type: FAT16
        Start Offset: 1048576 (2048 secotrs)
        Length: 104857600 (204800 sectors)
Partition 1:
        Partition Type: FAT16
        Start Offset: 105906176 (206848 secotrs)
        Length: 209715200 (409600 sectors)
Partition 2:
        Partition Type: FAT16
        Start Offset: 315621376 (616448 secotrs)
        Length: 524288000 (1024000 sectors)
Partition 3:
        Partition Type: FAT16
[]()        Start Offset: 839909376 (1640448 secotrs)
        Length: 1073741824 (2097152 sectors)
Carved 4 partitions
== Files on 0 ===
100M/
        System Volume Information/
                WPSettings.dat [692772596aa152d9909678b98fddfe72d8da87be8f21cfa08c11de696afeaa93]
                IndexerVolumeGuid [bd7b381c58de969b36b20a97025672389f9308c2288c424423034424d705f942]
        Groot.jpg [63e9b30b1d4d3ffe495ebfd91e2a9877dfa7dda10526abe5ca02b86996b17682]
        BYoda.jpeg [de4c83846dd2e183aa506d28eae188b15a217e5fa13b5a8f3b13a08fe8e2d627]
        Babu.jpg [92f38bc88c1377bf2dcc4dea25da1b79a2dd702d9830d0aac9aa72134fabb8b6]
== Files on 1 ===
200M/
        System Volume Information/
                WPSettings.dat [68ca05112c7df200eb378a63e90e67d3815a25313db4ac38e01f1ce6f97465df]
                IndexerVolumeGuid [f385d833863d304d866f00125023abc703d20e7295d4ac98fcfe0c13416b3fd1]
== Files on 2 ===
...
```

## Results
Although we were able to accomplish a lot of what we set out to do, much was left unfinished. For one, limited filesystem support severely limits the tools use in realistic environments. We had issues tying the arguments to the commands many functions, so we had to improvise and use all dump all functionality at once.

The feature most related to the coursework is how the program outputs the file hashes of each of the found files. We had plans to implement a feature to sign the output of the command with a private key, but time ran short on testing.