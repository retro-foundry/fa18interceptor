param([string]$Capture = 'build/menu_trace02', [string]$Project = 'Fa18Menu')
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$capturePath = (Resolve-Path (Join-Path $repoRoot $Capture)).Path
$ghidra = Join-Path $repoRoot '../ghidra/build/dist/ghidra_12.0.4_DEV/support/analyzeHeadless.bat'
python (Join-Path $PSScriptRoot 'prepare_ghidra.py') $capturePath
if ($LASTEXITCODE -ne 0) { throw 'Trace/snapshot byte validation failed' }
New-Item -ItemType Directory -Path (Join-Path $repoRoot 'runtime_project') -Force | Out-Null
$ErrorActionPreference = 'Continue' # Windows PowerShell treats Java stderr warnings as errors.
& $ghidra (Join-Path $repoRoot 'runtime_project') $Project `
    -import (Join-Path $capturePath 'chip.bin') -loader BinaryLoader -loader-baseAddr 0 `
    -processor '68000:BE:32:default' -noanalysis `
    -scriptPath (Join-Path $PSScriptRoot 'ghidra') `
    -postScript ImportFa18.java $capturePath `
    -postScript ExportFa18Pcode.java (Join-Path $capturePath 'pcode')
if ($LASTEXITCODE -ne 0 -or -not (Test-Path (Join-Path $capturePath 'pcode/summary.json'))) {
    throw 'Ghidra import/export failed; inspect headless output'
}
