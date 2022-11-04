[CmdletBinding(PositionalBinding=$False, DefaultParameterSetName='STL')]
Param (
  [Parameter(Position=0, Mandatory=$False)]
  [String]$Filter,
  [Parameter(Mandatory=$False)]
  [String]$Flavor,
  [Parameter(Mandatory=$False)]
  [String]$BuildDir,
  [Parameter(Mandatory=$False)]
  [Int]$Compiler = -1,
  [Parameter(Mandatory=$False, ParameterSetName='STL')]
  [Switch]$Changed,
  [Parameter(Mandatory=$False, ParameterSetName='STL')]
  [Switch]$STL,
  [Parameter(Mandatory=$True, ParameterSetName='LLVM')]
  [Switch]$LLVM,
  [Parameter(Mandatory=$True, ParameterSetName='TR1')]
  [Switch]$TR1
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

$testSet = 'std'
if ($LLVM) {
  $testSet = 'libcxx'
} elseif ($TR1) {
  $testSet = 'tr1'
}

cmake --build ./out/$Flavor
if (-not $?) {
  Write-Error 'Could not build $Flavor'
  throw
}

if ($Changed) {
  $expectedMainBranches = @('main', 'master', 'trunk', 'dev', 'devel')
  $mainBranch = ([String[]]($expectedMainBranches | % {
    if (-not [String]::IsNullOrEmpty((git branch --list $_))) {
      $_
    }
  }))[0]
  if ($null -eq $mainBranch) {
    throw "Could not find main branch"
  }

  [String[]]$changedTestFiles = git diff --name-only "$mainBranch...HEAD" "tests/$testSet/tests"

  $changedTests = $changedTestFiles | % {
    Split-Path -Parent $_
  } | Sort-Object -Unique | % {
    if ($_ -match '^tests\\std\\tests\\([^\\].*)$') {
      $Matches[1]
    }
  }

  if (-not [String]::IsNullOrEmpty($Filter)) {
    $changedTests = $changedTests + @($Filter)
  }
  $joiner = ')|(' # has a `)`, so we can't just put it in the string
  $Filter = "($($changedTests -join $joiner))"
}

if ($Compiler -ge 0) {
  $compilerFilter = ".*:$($Compiler.ToString('D2'))$"
  if (-not [String]::IsNullOrEmpty($Filter)) {
    $Filter = "($Filter)$compilerFilter"
  } else {
    $Filter = $compilerFilter
  }
}

$filterArg = @()
if (-not [String]::IsNullOrEmpty($Filter)) {
  $filterArg = @('--filter', $Filter)
}

echo "python ./out/$Flavor/tests/utils/stl-lit/stl-lit.py -vv ./tests/$testSet $($filterArg -join ' ')"
python ./out/$Flavor/tests/utils/stl-lit/stl-lit.py -vv ./tests/$testSet @filterArg
