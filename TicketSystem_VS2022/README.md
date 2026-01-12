# 火车订票系统（适配 Visual Studio 2022）

说明
- 这是一个命令行版示例实现，使用 C++17，已做 Windows 控制台 UTF-8 兼容处理。
- 包含文件：
  - main.cpp
  - ticket_system.h
  - ticket_system.cpp
  - create_project_and_zip.ps1（可选，用于自动生成文件并打包成 ZIP）

在 Visual Studio 2022 中运行（手动方式）
1. 打开 Visual Studio 2022，创建一个新的 "Console App" (C++) 项目（空项目也可）。
2. 将 main.cpp、ticket_system.h、ticket_system.cpp 三个文件添加到项目中。
3. 在项目属性中：C/C++ -> Language -> "C++ Language Standard" 选择 "ISO C++17 (/std:c++17)"。
4. 为保证源码与中文字面量正确处理，建议在 C/C++ -> Command Line 的 "Additional Options" 中添加 `/utf-8`。
5. 编译运行（F5）。程序在运行目录会读取/写入 trains.dat、tickets.dat、seq.dat 三个文件。

自动生成并打包（PowerShell 脚本）
- Windows 下打开 PowerShell（最好以普通用户权限即可），进入你希望生成项目的目录，运行：
  .\create_project_and_zip.ps1
- 脚本会创建目录 `TicketSystem_VS2022` 并写入文件，然后生成 `TicketSystem_VS2022.zip`。

注意
- 如果运行后控制台中文仍有乱码，确认 VS 的输出窗口/控制台字体支持 Unicode（例如使用“新宋体”或“MS Gothic”等）并且你已经运行了脚本设置 /utf-8。
- 若需我把完整的 .sln/.vcxproj 也生成为可用的 VS 项目文件，我可以继续帮你生成（但那会包含 VS 项目 XML 配置）；目前建议直接把源文件加入到新建项目中.
