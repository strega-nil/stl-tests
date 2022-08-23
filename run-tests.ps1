[CmdletBinding(PositionalBinding=$False, DefaultParameterSetName='STL')]
Param (
  [Parameter(Position=0, Mandatory=$False)]
  [String]$Filter,
  [Parameter(Mandatory=$False)]
  [String]$Flavor,
  [Parameter(Mandatory=$False)]
  [String]$BuildDir,
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

$filterArg = @()
if (-not [String]::IsNullOrEmpty($Filter)) {
  $filterArg = @('--filter', $Filter)
}

python ./out/$Flavor/tests/utils/stl-lit/stl-lit.py -vv ./tests/$testSet @filterArg
