# hc32f4a0-bootloader
 - 使用cmake编写的bootloader程序.
 - 升级方式为SD卡.
## 编译方法
下载源码  
`git clone https://github.com/nczyw/hc32f4a0-bootloader.git`  
进入目录  
`cd hc32f4a0-bootloader`
初始化子模块  
`git submodule init`  
拉取子模块内容  
`git submodule update --init --recursive`  
创建编译文件夹  
`mkdir build`  
`cd build`  
编译(liunux)  
`cmake -DCMAKE_BUILD_TYPE=Release ..`  
`make`  
编译(windows)MinGW  
`cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" ..`  
`mingw32-make`