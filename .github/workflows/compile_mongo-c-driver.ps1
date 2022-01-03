#
# Download and compile mongo-c-driver
#
# Note:
#   1. This script should be saved with an encoding of UTF-8 without BOM!
#        https://stackoverflow.com/a/62530946/4837280
#
# PreInstalled list: https://github.com/actions/virtual-environments/blob/main/images/win/Windows2019-Readme.md
#
# Author: Liangjun Zhu, zlj@lreis.ac.cn
# Date: 2022-01-03
#
$mongoCPath = "$env:SystemDrive\mongo-c-driver"
$version = "1.16.2"
$url = "https://github.com/mongodb/mongo-c-driver/releases/download/$version/mongo-c-driver-$version.tar.gz"
$zipFile = "$mongoCPath\mongo-c-driver.tar.gz"
$unzippedFolderContent ="$mongoCPath\mongo-c-driver-$version"

# Check if mongoCPath existed
if ((Test-Path -path $mongoCPath) -eq $false) {
    mkdir $mongoCPath
    Set-Location $mongoCPath
    Write-Host "Downloading mongo-c-driver……"
    #$webClient = New-Object System.Net.WebClient
    #$webClient.DownloadFile($url,$zipFile)
    #tar -xzf $zipFile
    Write-Host "Compiling mongo-c-driver……"
    Set-Location $unzippedFolderContent
    # Refers to http://mongoc.org/libmongoc/current/installing.html
    # mkdir cmake-build
    Set-Location cmake-build
    cmake -G "Visual Studio 16 2019" -A x64 "-DCMAKE_INSTALL_PREFIX=$mongoCPath" `
    "-DCMAKE_PREFIX_PATH=$mongoCPath" -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
    cmake --build . --config RelWithDebInfo --target install -- /m:4
    Write-Host "Setting environmetal paths of mongo-c-driver……"
    $env:MONGOC_ROOT = $mongoCPath
    $env:MONGOC_LIB_DIR = "$mongoCPath\bin"
    $env:PATH = "$mongoCPath\bin;" + $env:PATH
} else {
    Write-Host "mongo-c-driver already been installed."
}
