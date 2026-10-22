# Disk Analyzer
## Overview
`da` is a commandline-based tool for quickly scanning a disk image for forensic purposes. Features include determining the filesystem of the image, getting partition info, carving files from those partitions, and *creating a hashsum-checked report of all info found*. 
Although the functionality of the current release is limited, the project was built with extensibility in mind.

## Usage

> [!TODO] TODO
> Update to reflect current usage


```
da [action] [scope]

[action]
    list: scan image/partition, but don't write anything to files
    carve: save partitions on image/files on partition to files
[scope]
    parts: operate on partitions of a full disk image
    files: operate of files on a partition
```

### Examples
To list the partitions detected on an image:
```
da list parts img.dd
```

Carve detected partitions from `img.dd` and save them to the provided output directory, with default names `img.dd<partition_number>`.
```
da carve parts img.dd ./img_partitions/
```

Scan a partition image for files. 
```
da list files img.dd1
```

Output files from a partition image into the provided output directory.
```
da carve files img.dd1 ./img_partitions/
```

## Implementation
%% Outline the program structure? %%
- [ ] User-interfaceable structs
## Results
- [ ] what was successfully implemented
- [ ] what was not implemented
- [ ] relation to class's concepts
