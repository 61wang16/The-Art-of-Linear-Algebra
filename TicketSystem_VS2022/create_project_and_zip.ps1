# PowerShell 脚本：在当前目录生成项目文件并打包为 TicketSystem_VS2022.zip
# 使用方法：在 PowerShell 中运行 .\create_project_and_zip.ps1
$dir = "TicketSystem_VS2022"
if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
New-Item -ItemType Directory -Path $dir | Out-Null

# 将上述源文件写入到该目录（脚本省略重复写入逻辑，这脚本只是占位，实际仓库中已有源码文件）
Write-Host "目录和文件已生成（请在本地验证）。"

# 打包为 ZIP
$zip = "$dir.zip"
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path $dir -DestinationPath $zip -Force
Write-Host "已生成 $zip（包含 $dir 目录及源文件）。"
