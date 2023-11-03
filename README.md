# FUSE File System from JSON Files
A program written in C to mount a FUSE file system generated from input JSON files under Linux environment.

## General Information
This program reads JSON files with file-folder structure data as strings, and mounts a FUSE file system in the current Linux OS according to the obtained JSON data.

The program is derived from the **hello.c** example of the FUSE Project with FUSE USE VERSION 30 which is also included here.

## Setup & Run
1. Install libfuse-dev:
```
sudo apt-get install libfuse-dev
```
2. Compile **fuse.c** file by either executing **compile.sh** or running this command in terminal:
```
gcc fuse.c -o fuse -D_FILE_OFFSET_BITS=64 -lfuse
```
3. Run the generated output file **fuse** with the given empty **mounted** folder by either executing **mount.sh** or running this command in terminal:
```
./fuse ./mounted
```
* The file system can be seen on file explorer now. Tree structure of the file system can also be observed on terminal by using **tree**:
```
sudo apt-get install tree
```
```
tree ./mounted
```
4. Unmount the file system by either executing **unmount.sh** or running this command in terminal:
```
fusermount -u ./mounted
```

* **Compiling, Mounting and Unmounting**

<img title="Compiling, Mounting and Unmounting" src="https://github.com/firatkorkmaz/FUSE-from-JSON-Files/blob/main/images/Compile_Mount_Unmount.png">

* **Mounted File Content**

<img title="Mounted File Content" src="https://github.com/firatkorkmaz/FUSE-from-JSON-Files/blob/main/images/Mounted_File_Content.png">
