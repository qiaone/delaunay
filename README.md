Delaunay Triangulation Demo
========

Computational Geometry Class (2014, [邓俊辉](http://dsa.cs.tsinghua.edu.cn/~deng/index.htm)) Project @ Tsinghua University:

Delaunay Incremental Algorithm with Geometric Transformation 3D Demo
*The Project is licensed under the GNU GPL v2*

[Click Here to Download Excutables for Windows x64](https://raw.githubusercontent.com/qiaone/delaunay/master/Bin/delaunay.7z)

![](https://raw.githubusercontent.com/qiaone/delaunay/master/Img/gate_1.png)

![](https://raw.githubusercontent.com/qiaone/delaunay/master/Img/gate_2.png)

# Lab Report #

[Click Here to Download](https://raw.githubusercontent.com/qiaone/delaunay/master/Docs/report.pdf)

[![](https://raw.githubusercontent.com/qiaone/delaunay/master/Img/lab_report.png)](https://raw.githubusercontent.com/qiaone/delaunay/master/Docs/report.pdf)


# User Manual #

[Click Here to Download](https://raw.githubusercontent.com/qiaone/delaunay/master/Docs/user_manual.pdf)

[![](https://raw.githubusercontent.com/qiaone/delaunay/master/Img/user_manual.png)](https://raw.githubusercontent.com/qiaone/delaunay/master/Docs/user_manual.pdf)

# Performance #

- 1000 points, 0.005s
- 5000 points, 0.048s
- 10000 points, 0.073s
- 50000	points, 0.373s
- 100000 points, 0.773s

![](https://raw.githubusercontent.com/qiaone/delaunay/master/Img/performance.png)

  *With Intel Core i7-3770 3.4GHz * 4, 16GB RAM, Windows 7 Enterprise Edition, Visual Studio 2012 + Qt 5.1.1*

# Configuration #

* 程序入口：
    * 在 main.cpp 文件开头定义不同的宏转入相应的 entry point
        * 目前可定义 DTV / UNITEST / WPS / MW

* 风格约定：
    * 类名中各单词首字母大写，如 class EnsureException
    * 所有源码文件名小写，可用下划线分割单词，如 ensure_exception.cpp
        * 测试源码文件名使用类名+Test，如 OpenMeshTest.cpp
    * 类的public/private下先放置methods，然后放置varibles
    * 缩进为4个空格（而非tab）

* 版本控制：
    * 项目使用 git 进行版本控制，并使用 github 作为中心平台
    * 建议使用 SourceTree 作为 Windows 下客户端

* 开发和编译环境：
    * Visual Studio 2012 (vc11)
    * Qt 5.x.x for Windows 64-bit (VS 2012, OpenGL)
    * 可以使用 Qt Creator 或 Visual Studio 2012 作为 IDE
        * Qt Creator 的 VS 2012 版默认不带调试器，可以：
            * 安装 Debugging Tools for Windows
                http://msdn.microsoft.com/en-us/library/indows/hardware/ff551063(v=vs.85).aspx
            * 由于Qt Creator 调试器反映迟钝，建议使用 VS 进行调试
                * 在 Visual Studio 中使用插件 Visual Studio Add-in for Qt5 打开 pro 项目文件，并将生成的vs解决方案sln文件保存
    * 项目主要使用 Qt 工程文件 .pro 作配置，如果需要在 VS 中进行开发，则需要使用上述 VS Add-in for Qt5 打开
      此时项目会自动转化为 .vcxproj，出现以下错误的解决方法：
        * 问题：error LNK2019: 无法解析的外部符号 WinMain，该符号在函数 __tmainCRTStartup 中被引用
        * 解决：项目属性->配置属性->链接器->输入->附加依赖项 中加入
            $(QTDIR)\lib\qtmaind.lib (debug模式时)
            $(QTDIR)\lib\qtmain.lib (release模式时)
        * 问题：调试运行时弹出 无法启动此程序，因为计算机中丢失 OpenMeshCored.dll ...
        * 解决1：请将 OpenMeshCored.dll 等提示缺少的文件从 Libs 目录拷入 x64\Debug 或其他生成可执行文件的所在目录
        * 解决2：项目属性->(左上角配置)所有配置->配置属性->调试->环境->加入 %3b$(ProjectDir)\Libs
    * VS 相应的项目文件已经加入 .gitignore ，不被版本控制，即每人有独立拷贝
        * 如上所述，统一配置由受版本控制的 .pro 文件记录，因而项目配置方面的改动（如引入新的第三方库）应首先通过 .pro 完成，如项目配置有较大变化则需通过 Qt 插件重新生成vs项目

* 依赖库：
    * Google C++ Testing Framework 1.7.0 (x64 静态链接 从自带msvc工程编译)
        * 入门文章 http://www.cnblogs.com/coderzh/archive/2009/03/31/1426758.html
        * 也可参考 Tests/Smples 目录下的例子
    * OpenMesh 3.1 (x64 静态链接 从源码编译)
    * libQGLViewer 2.5.2 (x64 动态链接 从源码编译)
