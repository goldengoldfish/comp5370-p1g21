# Disk Analyzer
**COMP 5370/6370 Computer and Network Security**
*Group 21*
Brandon Buckley, Jarod Jernigan, Izayah Tubigon
## Overview
`da` is a commandline-based tool for quickly scanning a disk image for forensic purposes. Features include determining the filesystem of the image, getting partition info, carving files from those partitions, and creating a hashsum-checked report of all info found. 
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
### Argument Handling

#### Structures & Enums
- **Action** (enum): Defines the operation to be performed (e.g., to display info, to extract data).
- **Scope** (enum): Defines the context of the operation (e.g., for partitions, for file contents).
- **Arguments** (struct): Container holding the parsed command's state, including the , , and .

#### Functions
- `: Reads the command line arguments, validates them, and populates the structure. Returns 0 on success.- `: Prints the tool's usage guide and available commands to standard output.

---

### Media Access and Drive Handling

Provides the primary public interface for interacting with physical or filesystem devices (Drives).

#### Structures
- ****: The main structure representing the physical medium being analyzed.

#### Functions
- `: Constructor. Initializes a object from a given device path (e.g., ).- `: Destructor. Cleans up resources associated with the object.- `: Utility function to determine the number of partitions present on the drive.- `: Retrieves a specific object from the drive using its index.

---

### Data Streaming

Abstracts the input source, allowing the tool to treat files, memory buffers, or nested structures uniformly.

#### Structures
- ****: The base data stream object used for reading sequential data.

#### Functions
- **Constructors (Initialization)**:
 - : Creates a stream reading from a specified file path.
 - : Used for cascading streams (e.g., capturing a file system within a raw disk image). This overload handles nested stream contexts.
 - : Creates a stream from an existing memory buffer, allowing control.
 - : Used when processing multiple input streams simultaneously.- : Destructor. Frees the object and any resources it owns.- **Core Operations**:
 - : The primary interface for reading data from the stream into a target buffer.
 - : Allows seeking to a specific offset within the stream.
 - : Reports the current read offset within the stream.

---

### Partition Analysis

Handles structures and file systems residing within a partition.

#### Structures & Types
- ****: Represents a specific partition found on a drive.- **** (enum): Categorizes the type of partition found (e.g., ).- ****: Represents a file or directory within the file system structure.

#### Functions
- **Constructors/Destructors**:
 - Constructor to analyze and establish a FAT16 partition object from a stream segment.
 - Destructor for the object.- **Accessors**:
 - Determines the file system type of the partition.
 - Retrieves the starting byte offset of the partition on the physical medium.
 - Retrieves the total size (in bytes) of the partition.

## Results
- [ ] what was successfully implemented
- [ ] what was not implemented
- [ ] relation to class's concepts

Although we were able to accomplish most of what we set out to 