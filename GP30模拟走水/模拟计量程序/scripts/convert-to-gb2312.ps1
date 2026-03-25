<#
PowerShell 批量将文件从 UTF-8/系统默认编码转换为 GB2312
- 默认会先创建 .bak 备份（原始文件）
- 支持按扩展名批量转换（例如 *.c, *.h, *.txt）
- 使用示例：
  PS> .\convert-to-gb2312.ps1 -Path "C:\Users\MDC\Desktop\Simulate\Core" -Include "*.c","*.h" -WhatIf
  加 -WhatIf 先做演练；要真正写入，去掉 -WhatIf
#>
param(
    [Parameter(Mandatory=$true)]
    [string]$Path,

    [string[]]$Include = @("*.c","*.h","*.txt"),

    [switch]$NoBackup,

    [switch]$WhatIf
)

function Convert-FileToGB2312 {
    param(
        [string]$FilePath,
        [switch]$CreateBackup = $true,
        [switch]$WhatIfMode = $false
    )

    $encodingSrc = [System.Text.Encoding]::UTF8
    try {
        $encodingDst = [System.Text.Encoding]::GetEncoding("GB2312")
    } catch {
        Write-Error "系统中无法找到 GB2312 编码: $_"
        return
    }

    if ($WhatIfMode) {
        Write-Output "[WhatIf] 将把 '$FilePath' -> GB2312 $(if ($CreateBackup) { '(保留 .bak 备份)' } )"
        return
    }

    if ($CreateBackup) {
        Copy-Item -LiteralPath $FilePath -Destination ($FilePath + '.bak') -Force
    }

    # 读取文本并以 GB2312 写回
    try {
        $text = [System.IO.File]::ReadAllText($FilePath, $encodingSrc)
    } catch {
        # 如果不是 UTF8，可以尝试用系统默认编码读取再写入
        Write-Warning "无法用 UTF8 读取 '$FilePath'，尝试用系统默认编码读取。错误: $_"
        $text = [System.IO.File]::ReadAllText($FilePath)
    }

    try {
        [System.IO.File]::WriteAllText($FilePath, $text, $encodingDst)
        Write-Output "已转换: $FilePath"
    } catch {
        Write-Error "写入失败: $FilePath -> $_"
    }
}

# 主流程
$files = Get-ChildItem -Path $Path -Recurse -File -Include $Include -ErrorAction SilentlyContinue
if (-not $files) {
    Write-Warning "未找到匹配的文件。检查 -Path 和 -Include 参数。"
    return
}

foreach ($f in $files) {
    Convert-FileToGB2312 -FilePath $f.FullName -CreateBackup:(!$NoBackup) -WhatIfMode:$WhatIf
}

Write-Output "完成。注意：.bak 文件包含转换前的副本。若需回滚，请还原 .bak。" 
