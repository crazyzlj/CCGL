$mongoCPath = "$env:SystemDrive\mongo-c-driver"
$version = "1.16.2"
$url = "https://raw.githubusercontent.com/crazyzlj/Github_Actions_Precompiled_Packages/release/releases/mongo-c-driver-$version-vs2019x64.zip"
$zipFile = "$mongoCPath\mongo-c-driver.zip"

# Check if mongoCPath existed
if ((Test-Path -path $mongoCPath) -eq $false) {
    mkdir $mongoCPath
}
Set-Location $mongoCPath
Write-Host "Downloading mongo-c-driver-$version……"
$webClient = New-Object System.Net.WebClient
$webClient.DownloadFile($url,$zipFile)
Expand-Archive -Path $zipFile -DestinationPath "$env:SystemDrive\"
Write-Host "Setting environmetal paths of mongo-c-driver……"
$env:MONGOC_ROOT = $mongoCPath
$env:MONGOC_LIB_DIR = "$mongoCPath\bin"
$env:PATH = "$mongoCPath\bin;" + $env:PATH
