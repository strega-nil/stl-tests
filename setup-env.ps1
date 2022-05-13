[CmdletBinding()]
Param(
    [String]$BuildRoot,
    [String]$Arch
)

if ([String]::IsNullOrEmpty($BuildRoot)) {
    $BuildRoot = (Get-Item "$PSScriptRoot/../stl/build/out").FullName
}
if ([String]::IsNullOrEmpty($Arch)) {
    [String[]]$names = Get-ChildItem "$BuildRoot/lib" | ForEach-Object Name
    if ($names.Length -eq 1) {
        $Arch = $names[0]
    }
    elseif ($names.Length -gt 1) {
        if ('x64' -in $names) {
            $Arch = 'x64'
        }
        elseif ('x86' -in $names) {
            $Arch = 'x86'
        }
        elseif ('arm64' -in $names) {
            $Arch = 'arm64'
        }
        else {
            $Arch = $names[0]
        }
    }
    else {
        # $names.Length -eq 0
        throw "Invalid build root $BuildRoot"
    }
}

$includes = "$BuildRoot/inc"
$libs = "$BuildRoot/lib/$Arch"

Write-Host "Using include dir: $includes"
Write-Host "Using lib dir    : $libs"

$env:INCLUDE = "$includes;$env:INCLUDE"
$env:LIB = "$libs;$env:LIB"
