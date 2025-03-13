# hc32f4a0-bootloader
 - 使用cmake编写的bootloader程序.
 - 升级方式为SD卡.
## 编译方法
下载源码  
```bash
git clone https://github.com/nczyw/hc32f4a0-bootloader.git
```
进入目录  
```bash 
cd hc32f4a0-bootloader
```
初始化子模块  
```bash
git submodule init
```  
拉取子模块内容  
```bash 
git submodule update --init --recursive
```  
创建编译文件夹  
```bash
mkdir build 
cd build
```   
编译(liunux)  
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```  
编译(windows)MinGW  
```bash
cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" ..
mingw32-make
```  