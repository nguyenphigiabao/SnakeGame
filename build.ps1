param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    $portableCMake = Join-Path $PSScriptRoot ".tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe"
    if (Test-Path -LiteralPath $portableCMake) {
        $cmake = $portableCMake
    } else {
        throw "CMake was not found. Install CMake 3.20 or newer and add it to PATH."
    }
} else {
    $cmake = (Get-Command cmake).Source
}

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    throw "g++ was not found. Install MinGW-w64 and add its bin directory to PATH."
}

& $cmake -S $PSScriptRoot -B (Join-Path $PSScriptRoot "build") -G "MinGW Makefiles" "-DCMAKE_BUILD_TYPE=$Configuration"
& $cmake --build (Join-Path $PSScriptRoot "build") --config $Configuration -j 4

$executable = Join-Path $PSScriptRoot "build\bin\SnakeGame.exe"
if (-not (Test-Path -LiteralPath $executable)) {
    throw "Build completed without creating $executable"
}

Write-Host "Build complete: $executable" -ForegroundColor Green
Write-Host "Run it with: .\build\bin\SnakeGame.exe"
