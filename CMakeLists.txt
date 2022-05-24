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
