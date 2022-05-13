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
        if ('amd64' -in $names) {
            $Arch = 'amd64'
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

$include = "$BuildRoot/inc"
$lib = "$BuildRoot/lib/$Arch"
$bin = "$BuildRoot/bin/$Arch"

Write-Host "Using include dir: $include"
Write-Host "Using lib dir    : $lib"
Write-Host "Using bin dir    : $bin"

$env:INCLUDE = "$include;$env:INCLUDE"
$env:LIB = "$lib;$env:LIB"
$env:PATH = "$bin;$env:PATH"
