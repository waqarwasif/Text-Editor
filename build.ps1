# build.ps1 - Automated build script for TextEditor
param (
    [switch]$RunTests,
    [switch]$RunApp
)

$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH
$CXX = "C:\msys64\ucrt64\bin\g++.exe"

if (-not (Test-Path $CXX)) {
    $CXX = "g++"
}

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Join-Path $ScriptDir "Text-Editor"
if (-not (Test-Path $ProjectDir)) {
    $ProjectDir = $ScriptDir
}

$BuildDir = Join-Path $ScriptDir "build"
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

$Flags = @(
    "-std=c++17",
    "-Wall",
    "-Wextra",
    "-static",
    "-static-libgcc",
    "-static-libstdc++",
    "-I", "$ProjectDir/include",
    "-I", "$ProjectDir/src"
)

$SrcFiles = @(
    "$ProjectDir/src/DataStructures/MyString.cpp",
    "$ProjectDir/src/DataStructures/Array.cpp",
    "$ProjectDir/src/DataStructures/DoublyLinkedList.cpp",
    "$ProjectDir/src/DataStructures/UndoStack.cpp",
    "$ProjectDir/src/Utils/TextProcessing.cpp",
    "$ProjectDir/src/Core/FileOperations.cpp",
    "$ProjectDir/src/Core/TextEditor.cpp"
)

Write-Host "Compiling TextEditor.exe..." -ForegroundColor Cyan
& $CXX @Flags @SrcFiles "$ProjectDir/src/Utils/Menu.cpp" "$ProjectDir/src/main.cpp" -o "$BuildDir/TextEditor.exe"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile TextEditor.exe"
    exit 1
}

$Tests = @(
    "test_linked_list",
    "test_text_editor",
    "test_text_processing",
    "test_file_io",
    "test_search_replace",
    "test_replace",
    "test_undo_stack"
)

foreach ($t in $Tests) {
    Write-Host "Compiling ${t}.exe..." -ForegroundColor Cyan
    & $CXX @Flags @SrcFiles "$ProjectDir/tests/$t.cpp" -o "$BuildDir/$t.exe"
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to compile ${t}.exe"
        exit 1
    }
}

Write-Host "`nBuild complete! Executables located in $BuildDir" -ForegroundColor Green

if ($RunTests) {
    Write-Host "`n--- Running All Tests ---" -ForegroundColor Yellow
    foreach ($t in $Tests) {
        Write-Host "`n==> Running $t" -ForegroundColor Magenta
        & "$BuildDir/$t.exe"
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Test $t failed"
            exit 1
        }
    }
    Write-Host "`nAll tests completed successfully!" -ForegroundColor Green
}

if ($RunApp) {
    & "$BuildDir/TextEditor.exe"
}
