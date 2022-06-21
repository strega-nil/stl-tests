[CmdletBinding()]
Param(
    [Parameter(Mandatory)]
    [String]$Flavor,
    [String]$Arch
)

$buildRoot = (Get-Item "$PSScriptRoot/../stl/out/$Flavor/out" -ErrorAction Stop).FullName
if ([String]::IsNullOrEmpty($Arch)) {
    [String[]]$names = Get-ChildItem "$buildRoot/lib" | ForEach-Object Name
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
        throw "Invalid build root $buildRoot"
    }
}

$include = "$buildRoot/inc"
$lib = "$buildRoot/lib/$Arch"
$bin = "$buildRoot/bin/$Arch"

Write-Host "Using include dir: $include"
Write-Host "Using lib dir    : $lib"
Write-Host "Using bin dir    : $bin"

$env:INCLUDE = "$include;$env:INCLUDE"
$env:LIB = "$lib;$env:LIB"
$env:PATH = "$bin;$env:PATH"
