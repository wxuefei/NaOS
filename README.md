# NaOS  

## Nano Operating system
A 64bit (arch x86-64) Simple Operating System Writing By C++.  (:sunglasses:)  
NaOS runs on Intel / AMD modern processors.  

![BSD-3-Clause](https://img.shields.io/badge/License-BSD--3--Clause-green) ![development on linux](https://img.shields.io/badge/build--platform-linux-lightgrey)  


## Features  
View [Features](./FEATURES.MD) .

## Preview
![preview1](https://github.com/kadds/images/raw/master/naos/view1.apng)

## Quick Start  

### **Requirement**  
* **GNU Binutils** *(version 2.33 tested)*
* **GCC** or **Clang** supports *C++17* version  *(GCC 9.2.0 & Clang 9.0.0 tested on [Manjaro](https://manjaro.org/) 18.1.0)*
* **CMake 3.3** or later
* **Python 3** *(For running utility)*
* An emulator, virtual machine such as **Bochs**, **QEMU**, **Virtual Box** and **VMware Workstation** *(For running OS)*
* **Grub2**, **fdisk / gdisk**, **udisks2** *(For making runnable raw disk file and mounting disk without root privilege)*
* *OVMF* UEFI firmware for QEMU
  
### **Recommend**  

* **clang-format**: Code formatter 
* **cpp-check**: Code static analyzer 
* [**VSCode**](https://code.visualstudio.com/): Development environment.

### 1. Download source code
Clone this repo ```git clone https://url/path/to/repo``` 

### 2. Compile
```Bash
cd /path/to/repo

mkdir build
cd build
# CMAKE_BUILD_TYPE: Debug\Release
# USE_CLANG: OFF (GCC); ON (Clang)
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_CLANG=OFF ..
# Then make
make -j
```

### 3. Make a raw disk
[Arch wiki](https://wiki.archlinux.org/index.php/Fdisk) Make disk  
[Arch wiki](https://wiki.archlinux.org/index.php/GRUB) Install grub   
  
NaOS supports GPT disk with UEFI boot or MBR disk with BIOS boot. See grub [multiboot2 (spec)](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)   
For startup QEMU with UEFI boot, install [OVMF](https://sourceforge.net/projects/tianocore/) and config OVMF_CODE.fd path at *util/run.py*.  

Example of disk partition (UEFI):  
| Partition number | Type  | (Gdisk) Code |  FS   |      Content       |  Size  |
| :--------------: | :---: | :----------: | :---: | :----------------: | :----: |
|        1         |  ESP  |     EF00     | FAT32 |  Grub EFI loader   | 50Mib  |
|        2         | Root  |     8300     | FAT32 | Grub and NaOS data | 100Mib |

Example of disk partition (MBR):  
| Partition number | Type  | (Gdisk) Code |  FS   |  Content  |  Size  |
| :--------------: | :---: | :----------: | :---: | :-------: | :----: |
|        1         | Grub  |     8300     | FAT32 | Grub data | 50Mib  |
|        2         | Root  |     8300     | FAT32 | NaOS data | 100Mib |

Example of grub install (UEFI):
```bash
sudo grub-install --boot-directory=/mnt/Root/boot  --efi-directory=/mnt/ESP --targe=x86_64-efi run/image/disk.img
```

Example of grub install (MBR):
```bash
sudo grub-install --boot-directory=/mnt/Root/boot  --targe=i386-pc run/image/disk.img
```

Example *grub.cfg* (UEFI):
```
root=(hd0,gpt2)
set default=0
set timeout=1
menuentry "NaOS multiboot2" {
    insmod efi_gop
    insmod efi_uga
    insmod ieee1275_fb
    insmod part_gpt
    insmod part_msdos
    insmod fat
    insmod ext2
    multiboot2 /boot/kernel
    module2 /boot/rfsimg rfsimg
    boot
}
```

Example *grub.cfg* (MBR):
```
root=(hd0,msdos1)
set default=0
set timeout=1
menuentry "NaOS multiboot2" {
    insmod part_msdos
    insmod fat
    multiboot2 /boot/kernel
    module2 /boot/rfsimg rfsimg
    boot
}
```

Then move the raw disk to *run/image/disk.img*   


### 4. Run
After ```make``` success, you will get these files
```
build
├── bin # Binary executable files without debug info
│   ├── rfsroot # root file system image files (root folder)
│   └── system
│       ├── rfsimg # root file system image
│       └── kernel # kernel binary file
└── debug # Binary executable files with debug info which can be used by debugger gdb/lldb ...
```

**make sure you mount disk by ``` python util/disk.py mount``` before run emulator**. then
```Bash
# Run emulator
python util/run.py q
```

The kernel log will be generated in *run/kernel_out.log*, just ```cat run/kernel_out.log```.

### 5. Debug
Use command ```python util/gen_debug_asm.py kernel``` to generate kernel disassembly if needed. (e.g. Debug in bochs)

Easily debugging with **VSCode** when running NaOS on QEMU. Just configure in *.vscode/launch.json*
```Json
"MIMode": "gdb", # your debugger: gdb or lldb
"miDebuggerServerAddress": "localhost:1234", # your host:port
```
, start QEMU and press F5.


## Repo Tree
```
NaOS
├── build # build directory
├── naos
│   ├── includes
│   │   └── kernel
│   └── src
│       ├── kernel
│       │   ├── arch # arch x86_64 specification source code
│       │   ├── common # kernel data
│       │   ├── dev # driver interface & drivers
│       │   ├── fs # file subsystem 
│       │   ├── io # io subsystem
│       │   ├── mm # memory subsystem
│       │   ├── module # module support code
│       │   ├── schedulers # round robin scheduler and completely fair scheduler
│       │   ├── syscall # syscall entries
│       │   ├── task # ELF loader and built-in task
│       │   └── util # util functions: memcpy, strcpy, cxxlib, formatter, containers
│       └── usr
│           ├── init # the userland init program 
│           └── syscall_test # kernel test program 
├── run
│   ├── cfg # include emulator config file: bochsrc
│   └── image
│       └── disk.img # raw disk image to startup OS
└── util # Python tools
```

## Maintainers 
[@Kadds](https://github.com/Kadds).

## Reference 
* [OSDev](https://forum.osdev.org/)
* [Linux](https://www.kernel.org/)
* [Intel SDM](https://software.intel.com/en-us/articles/intel-sdm)
* [Minix3](http://www.minix3.org/)
* [VX6](https://github.com/mit-pdos/xv6-public)

## License
[BSD-3-Clause](./LICENSE) © Kadds

----

```
          _____                    _____                   _______                   _____          
         /\    \                  /\    \                 /::\    \                 /\    \         
        /::\____\                /::\    \               /::::\    \               /::\    \        
       /::::|   |               /::::\    \             /::::::\    \             /::::\    \       
      /:::::|   |              /::::::\    \           /::::::::\    \           /::::::\    \      
     /::::::|   |             /:::/\:::\    \         /:::/~~\:::\    \         /:::/\:::\    \     
    /:::/|::|   |            /:::/__\:::\    \       /:::/    \:::\    \       /:::/__\:::\    \    
   /:::/ |::|   |           /::::\   \:::\    \     /:::/    / \:::\    \      \:::\   \:::\    \   
  /:::/  |::|   | _____    /::::::\   \:::\    \   /:::/____/   \:::\____\   ___\:::\   \:::\    \  
 /:::/   |::|   |/\    \  /:::/\:::\   \:::\    \ |:::|    |     |:::|    | /\   \:::\   \:::\    \ 
/:: /    |::|   /::\____\/:::/  \:::\   \:::\____\|:::|____|     |:::|    |/::\   \:::\   \:::\____\
\::/    /|::|  /:::/    /\::/    \:::\  /:::/    / \:::\    \   /:::/    / \:::\   \:::\   \::/    /
 \/____/ |::| /:::/    /  \/____/ \:::\/:::/    /   \:::\    \ /:::/    /   \:::\   \:::\   \/____/ 
         |::|/:::/    /            \::::::/    /     \:::\    /:::/    /     \:::\   \:::\    \     
         |::::::/    /              \::::/    /       \:::\__/:::/    /       \:::\   \:::\____\    
         |:::::/    /               /:::/    /         \::::::::/    /         \:::\  /:::/    /    
         |::::/    /               /:::/    /           \::::::/    /           \:::\/:::/    /     
         /:::/    /               /:::/    /             \::::/    /             \::::::/    /      
        /:::/    /               /:::/    /               \::/____/               \::::/    /       
        \::/    /                \::/    /                 ~~                      \::/    /        
         \/____/                  \/____/                                           \/____/         
                                                                                                    
```