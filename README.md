# Disk Forensics Tool
## Overview
The product will be a commandline-based tool for quickly scanning a disk image for forensic purposes. Features would include scanning the determining the filesystem of the image, getting partitions, carving files from those partitions, and creating a hashsum-checked report of all info found. The tool would overlap with tools such as dd and fdisk, but with our own implementations. 

Because of how modular the featureset would be, this idea would be easy to scale depending on difficulty. Initial filesystem support would include only FAT16 and/or FAT32, but support for NTFS or other filesystems could be added later. Additionally, additional options  could be added

The main course-relevant feature would be the hashsums, which seems like something tacked on the end, rather than a core part of the product.

For example, the result of running the tool may create a directory of found data, like so:

```
disk.img
├── fat16.p1
│   ├── image1.jpg
│   ├── image2.jpeg
│   ├── image3.jpg
│   └── System Volume Information
│       ├── IndexerVolumeGuid
│       └── WPSettings.dat
├── fat16.p2
│   └── System Volume Information
│       ├── IndexerVolumeGuid
│       └── WPSettings.dat
├── fat16.p3
│   ├── image2.jpeg
│   └── System Volume Information
│       ├── IndexerVolumeGuid
│       └── WPSettings.dat
└── fat16.p4
    ├── image1.jpg
    ├── image3.jpg
    └── System Volume Information
        ├── IndexerVolumeGuid
        └── WPSettings.dat
```

## Usage
```
da [action] [scope]

[action]
    list: scan image/partition, but don't write anything to files
    carve: save partitions on image/files on partition to files
    ?mount: stretch goal. like carve, but mount image/partitions on image
            (default read-only) instead of copying. Would be useful for large images.
            Perhaps implement with fusermount?
[scope]
    parts: operate on partitions of a full disk image
    files: operate of files on a partition
```

### Examples:

da carve parts img.dd <output-dir>
    given img.dd has 2 partitions, carve out each partition on img.dd to ./img.dd{1,2}

da list parts <image>
    lists partitions on image

da list files img.dd1
    given img.dd is a partition, list files on img.dd1

da carve files img.dd1 <output-dir>
    save found files from img.dd1 to output-dir
