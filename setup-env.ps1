[CmdletBinding()]
Param(
    [String]$Flavor,
    [String]$Arch
)

if ([String]::IsNullOrEmpty($Flavor)) {
  [String[]]$clVersion = cl 2>&1

  [string[]]$versions = $clVersion | % {
    if ($_ -match 'Microsoft \(R\) C/C\+\+ Optimizing Compiler Version [0-9.]+ for (.*)') {
      $Matches[1].ToLower()
    }
  }
  if ($versions.Length -eq 1) {
    $Flavor = $versions[0]
  } else {
    throw "Unknown compiler: $($clVersion -join "`n")"
  }
}

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
