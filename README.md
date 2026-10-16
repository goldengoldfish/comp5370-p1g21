# Disk Forensics Tool
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
