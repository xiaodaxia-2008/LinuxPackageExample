# Linux程序打包技术



[TOC]

我们在linux上写好了代码，编译完，在本地测试没有问题，可是当把程序发给别人用的时候，常常因为各种依赖而导致运行不起来。对于用户来说最好的体验莫过于在收到程序的时候，直接双击一下，就可以运行了；或者双击安装，然后在桌面或者开始菜单生成一个快捷方式，点击运行。以下的内容就来聊聊怎么达到这样的用户体验。

## 打包Python程序

起始Python程序在linux上比在window下更要方便一点，毕竟没有哪个linux不安装Python的，只是我们写的程序常常都是很多个脚本，而且依赖很多第三方的包，交给用户的时候，需要先把环境配置起来才能使用，不是很方便。如果能够把Python程序直接编译成一个可执行文件，将Python的解释器以及需要的第三方包都嵌入进去，那就不需要用户端配置环境了，甚至用户连Python都不需要安装。庆幸地是，已经有人开发出来了这样的工具——[PyInstaller](https://pyinstaller.org/en/stable/)。它不光可以在linux下使用，还可以在windows、mac下使用，下面主要介绍的是在linux下如何使用。

下载安装 PyInstaller：

```
pip3 install pyinstaller
```

下载完成后，就可以使用了。对于一个简单的Python脚本，直接运行：

```
pyinstaller -F hello.py
```

其中-F是single file的缩写选项，意思是打包成一个单独的文件，将这个单独的文件传给用户就可以了；如果不加-F，打包的结果是一个文件夹，需要在文件夹里面找到可执行文件，分发的时候把整个文件夹分发。

就可以在dist目录下生成一个独立的hello的可执行程序，可以使用 ldd看看这个程序都有哪些库依赖：

> ➜  ldd dist/hello 
>         linux-vdso.so.1 (0x00007ffd4797d000)
>         libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f98e77a0000)
>         libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f98e7783000)
>         libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f98e7762000)
>         libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f98e75a1000)
>         /lib64/ld-linux-x86-64.so.2 (0x00007f98e77e9000)

可以看到都是一些系统库，而且没有libpythonX.X.so，说明该程序的运行不依赖于Python环境了。

可以运行试一下：

```
➜ ./dist/hello 
hello, world!
```

对于复杂一点的项目，比如自己用c++写的Python模块，pyinstaller可能找不到，这个时候，一种方式是把模块所在路径添加到环境变量PYHTONPATH里面，一种是在使用pyinstaller的时候添加命令行参数 -p 后面加上模块路径，这个参数的说明是：

> -p DIR, --paths DIR   A path to search for imports (like using PYTHONPATH).
>                         Multiple paths are allowed, separated by ``':'``, or
>                         use this option multiple times. Equivalent to
>                         supplying the ``pathex`` argument in the spec file.

如果有好几个路径，用`:`分隔。

另外几个常用的参数--splash用来定义一个启动时的画面；-i用来定义一个窗口图标，只是在linux上好像没用。



## 打包可执行的二进制文件

如果是用c++、c编写的程序，在编译好之后，往往会依赖一大堆动态库，在用户的电脑上，这些动态库不一定都很能方便的安装，所以最好是在分发程序的时候将这些动态库一起打包。[AppImage](https://appimage.org/)就是为了解决这个问题而产生的，它的目标就是让linux程序随处运行。

假设我们创建了如下的c++程序

```c++
/**
 * @file ku.h
 */

#include <vector>

std::vector<float> vec_sqrt(const std::vector<float> &vec);
void print_vec(const std::vector<float> &vec);
```

```c++
/**
 * @file ku.cpp
 */

#include "ku.h"
#include <algorithm>
#include <iostream>
#include <cmath>

void print_vec(const std::vector<float> &vec)
{
    std::cout << "{";
    for (const auto &e : vec)
    {
        std::cout << e << ",";
    }
    std::cout << "}\n";
}

std::vector<float> vec_sqrt(const std::vector<float> &vec)
{
    std::vector<float> res(vec.size());
    int i = 0;
    for (const auto &e : vec)
    {
        res[i++] = std::sqrt(e);
    }
    return res;
}
```

```c++
/**
 * @file main.cpp
 */

#include "ku.h"
#include <algorithm>


int main()
{
    int n = 10;
    std::vector<float> vec(n);
    float v = 0.f;
    std::for_each(vec.begin(), vec.end(), [&v](float &e)
                  {
        e = v + 0.5f;
        v = e; });
    print_vec(vec);
    auto res = vec_sqrt(vec);
    print_vec(res);
    return 0;
}
```

cmake如下：

```cmake
cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(Temp)
add_library(ku ku.cpp)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ku)
```

同时准备一个App.desktop文件：

```
[Desktop Entry]
Version=1.0
Name=App
Exec=main
Terminal=false
Type=Application
Icon=Eye
Categories=Graphics;Science;Engineering;
StartupNotify=true
```



编译成功之后，我们看一下它的依赖

> ➜  build ldd bin/main 
>         linux-vdso.so.1 (0x00007ffc2a396000)
>         libku.so => /home/rvbust/Temp/build/lib/libku.so (0x00007f78d16e8000)
>         libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f78d1522000)
>         libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f78d139f000)
>         libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f78d1385000)
>         libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f78d11c4000)
>         /lib64/ld-linux-x86-64.so.2 (0x00007f78d16ef000)

可执行程序main依赖libku.so，而这个库使我们自己写的，所以需要随着main一起发给客户，显然不是很方便。我们使用linuxdeploy来创建一个独立的appimage文件，将所有的依赖打包进去。

首先在[github的release页面去下载linuxdeploy的appimage](https://github.com/linuxdeploy)，下载后给它可执行权限，假设放在了HOME目录。

生成AppImage一般输入下面几个参数就够了，分别如下：

>      --appdir=[appdir]                 Path to target AppDir
>     
>      -e[executable...],
>       --executable=[executable...]      Executable to deploy
>      
>       -d[desktop file...],
>       --desktop-file=[desktop file...]  Desktop file to deploy
>      
>       -i[icon file...],
>       --icon-file=[icon file...]        Icon to deploy

所以对于我们的场景，使用下面的命令生成AppImage

```
~/linuxdeploy-x86_64.AppImage --appdir=AppDir -e build/bin/main -d ./App.desktop -i Eye.svg --output appimage
```

成功后，就在当前目录生成了 App-x86_64.AppImage 这个文件。在生成过程中，会产生一个AppDir的文件夹，App-x86_64.AppImage 里面包含了文件夹里的内容。

> ➜  tree AppDir 
> AppDir
> ├── App.desktop -> usr/share/applications/App.desktop
> ├── AppRun -> usr/bin/main
> ├── Eye.svg -> usr/share/icons/hicolor/scalable/apps/Eye.svg
> └── usr
>     ├── bin
>     │   └── main
>     ├── lib
>     │   └── libku.so
>     └── share
>         ├── applications
>         │   └── App.desktop
>         ├── doc
>         │   └── papirus-icon-theme
>         │       └── copyright
>         └── icons
>             └── hicolor
>                 ├── 128x128
>                 │   └── apps
>                 ├── 16x16
>                 │   └── apps
>                 ├── 256x256
>                 │   └── apps
>                 ├── 32x32
>                 │   └── apps
>                 ├── 64x64
>                 │   └── apps
>                 └── scalable
>                     └── apps
>                         └── Eye.svg

可以看下文件夹里的内容就是我们通过参数加入进去的，以及它自己通过ldd等工具查找的依赖。这样我们在分发程序的时候只需要把App-x86_64.AppImage这一个文件发给用户就可以了。



## Deb包的生成

下面介绍下Debian系列系统怎么生成deb包，上面两种方式比较适合打包可执行文件，如果是库文件的话，别人要依赖于我们的库做二次开发，是不能用上面两种方式的，这种情况下可以用Deb包，当然了Deb包也能用来打包可执行文件。

还是用上面的例子吧，我们生成了libku.so，如果我们要将这个库以及头文件分发给别人去使用，一种是直接拷贝多个文件过去，不方便。我们可以用cpack工具将这些库、头文件打包成一个deb包，用户端只需要使用apt工具安装即可。

打包deb包需要用到cpack工具，都是通过修改CMakeLists.txt配置的，修改后的内容如下：

```cmake
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

project(LinuxPackageExample VERSION 0.0.1)

add_library(ku ku.cpp)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ku)

include(GNUInstallDirs)
install(
  TARGETS ku main
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ku.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "example@qq.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "example@qq.com")

# 下面可以设置这个deb包的系统依赖，安装时自动通过apt查找安装
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "qt5-default, qttools5-dev, qtdeclarative5-dev")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_SOURCE_DIR}/postinst)
# 包的安装路径前缀，默认是/usr
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
include(CPack)

# 下面两个命令用来生成AppImage和通过pyinstaller打包Python脚本
# 如果是qt程序的话，在--output前面加上 --plugin qt
# 还需要先从github上下载linuxdeploy-plugin-qt-x86_64.AppImage，放置到和linuxdeploy-x86_64.AppImage相同的目录。
# 如果系统安装了多个qt版本，在终端中export QMAKE=path/to/your/qt/qmake，来确定选择哪一个
add_custom_target(
  BuildAppImage
  COMMAND
    $ENV{HOME}/linuxdeploy-x86_64.AppImage -d ${CMAKE_SOURCE_DIR}/App.desktop -i
    ${CMAKE_SOURCE_DIR}/Eye.svg -e ${CMAKE_BINARY_DIR}/bin/main --appdir=AppDir
    --output appimage
  DEPENDS main)

add_custom_target(
  BuildPythonApp
  COMMAND pyinstaller -p ${CMAKE_BINARY_DIR}/lib -i
          ${CMAKE_SOURCE_DIR}/WindowIcon.ico -F ${CMAKE_SOURCE_DIR}/hello.py)

```

在build文件夹里面：

```
cmake ..
make 
cpack
```

> CPack: Create package using DEB
> CPack: Install projects
> CPack: - Run preinstall target for: LinuxPackageExample
> CPack: - Install project: LinuxPackageExample []
> CPack: Create package
> CPack: - package: /home/rvbust/LinuxPackageExample/build/LinuxPackageExample-0.0.1-Linux.deb generated.

成功后在build文件夹里面生成

> LinuxPackageExample-0.0.1-Linux.deb

这个文件可以直接使用下面的命令安装：

```
sudo apt install ./LinuxPackageExample-0.0.1-Linux.deb
```

如果使用默认安装路径，安装后在终端直接输入main，就可以运行程序了。

卸载的话使用下面的命令：

```
sudo apt remove linuxpackageexample
```



## 总结

上面介绍了几种linux程序的打包技术，总的来说：

- 如果是Python程序，使用pyinstaller打包成一个不需要Python环境的单个可执行文件，该文件一般会比较大
- 如果是c、c++可执行程序，使用linuxdeploy工具打包成AppImage，可以免去客户机上配置各种依赖
- 如果是库文件，本身应该是面向开发者的，可以利用cpack打包成deb安装包，相关的依赖也可以通过apt自动下载