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
  [Switch]$LLVM
)

if ([String]::IsNullOrEmpty($Flavor)) {
	if ($env:PROCESSOR_ARCHITECTURE -eq 'ARM64' -or $env:PROCESSOR_IDENTIFIER -match "ARMv[8,9] \(64-bit\)") {
		$Flavor = 'arm64'
	} else {
		$Flavor = 'x64'
	}
}

$testSet = 'std'
if ($LLVM) {
  $testSet = 'libcxx'
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
