param([switch]$SkipBuild, [switch]$PackageOnly)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$taskRoot = $PSScriptRoot
$taskImages = Join-Path $taskRoot 'report_screenshots'
$taskArchive = Join-Path $taskRoot 'Graphics_Project_Report_Screenshots.zip'
$taskRequired = @(
 '01_overview.png','02_birds_eye.png','03_day.png','04_night.png','05_rain.png',
 '06_lights_off.png','07_ceiling_light.png','08_bedside_light.png','09_study_light.png','10_all_lights.png',
 '11_flat.png','12_gouraud.png','13_phong.png','14_fan.png','15_fan_speed.png',
 '16_door_open.png','17_curtain_open.png','18_wardrobe_open.png','19_drawer_open.png','20_laptop_open.png',
 '21_keyboard_backlight.png','22_clock.png','23_showcase.png'
)
Push-Location -LiteralPath $taskRoot
try {
 if (-not $PackageOnly) {
  if (-not $SkipBuild) {
   & (Join-Path $taskRoot 'build.bat')
   if ($LASTEXITCODE -ne 0) { throw 'Project build failed.' }
  }
  & (Join-Path $taskRoot 'build\smart_study_bedroom.exe') --capture-report
  if ($LASTEXITCODE -ne 0) { throw 'Report capture failed.' }
 }
 Add-Type -AssemblyName System.Drawing
 Add-Type -AssemblyName System.IO.Compression
 $taskManifest = @(Import-Csv -LiteralPath (Join-Path $taskImages 'manifest.csv'))
 $taskNames = @($taskManifest | ForEach-Object { $_.filename })
 if ($taskNames.Count -ne @($taskNames | Select-Object -Unique).Count) { throw 'Duplicate image names in manifest.' }
 foreach ($taskName in $taskRequired) {
  if ($taskName -cnotin $taskNames) { throw "Missing required image: $taskName" }
 }
 foreach ($taskRow in $taskManifest) {
  if ($taskRow.filename -cnotmatch '^\d{2}_[a-z0-9_]+\.png$') { throw 'Invalid screenshot filename.' }
  $taskPath = Join-Path $taskImages $taskRow.filename
  $taskFile = Get-Item -LiteralPath $taskPath
  if ($taskFile.Length -le 0 -or $taskFile.Length -ne [long]$taskRow.bytes) { throw "Invalid file size: $($taskRow.filename)" }
  $taskImage = [System.Drawing.Image]::FromFile($taskPath)
  try {
   if ($taskImage.Width -ne 1920 -or $taskImage.Height -ne 1080 -or $taskImage.RawFormat.Guid -ne [System.Drawing.Imaging.ImageFormat]::Png.Guid) {
    throw "Invalid PNG format/resolution: $($taskRow.filename)"
   }
   # Decode the complete image rather than relying on its header alone.
   $taskDecoded = New-Object System.Drawing.Bitmap 1920,1080
   $taskDrawing = [System.Drawing.Graphics]::FromImage($taskDecoded)
   try { $taskDrawing.DrawImageUnscaled($taskImage,0,0) } finally { $taskDrawing.Dispose(); $taskDecoded.Dispose() }
  } finally { $taskImage.Dispose() }
  Write-Output "[OK] $($taskRow.filename) | 1920x1080 | $($taskFile.Length) bytes"
 }
 function Get-SceneSignature($taskRow, [string[]]$taskExclude) {
  return (($taskRow.PSObject.Properties | Where-Object { $_.Name -notin $taskExclude } | ForEach-Object { "$($_.Name)=$($_.Value)" }) -join '|')
 }
 function Assert-Group($taskNamesToCompare, $taskExclude, [string]$taskDescription) {
  $taskGroup = @($taskManifest | Where-Object { $_.filename -in $taskNamesToCompare })
  if ($taskGroup.Count -ne $taskNamesToCompare.Count) { throw "Incomplete comparison: $taskDescription" }
  $taskSignatures = @($taskGroup | ForEach-Object { Get-SceneSignature $_ $taskExclude } | Select-Object -Unique)
  if ($taskSignatures.Count -ne 1) { throw "Comparison state mismatch: $taskDescription" }
  $taskHashes = @($taskGroup | ForEach-Object { (Get-FileHash -LiteralPath (Join-Path $taskImages $_.filename) -Algorithm SHA256).Hash } | Select-Object -Unique)
  if ($taskHashes.Count -ne $taskGroup.Count) { throw "Comparison images do not differ: $taskDescription" }
  Write-Output "[OK] $taskDescription"
 }
 Assert-Group @('11_flat.png','12_gouraud.png','13_phong.png') @('filename','bytes','shading') 'Shading comparison: identical camera, simulation, environment and lights; only shading changes.'
 Assert-Group @('06_lights_off.png','07_ceiling_light.png','08_bedside_light.png','09_study_light.png','10_all_lights.png') @('filename','bytes','ceiling','bedside','study') 'Lighting comparison: identical camera, objects and environment; only lamp switches change.'
 Assert-Group @('21_keyboard_backlight.png','28_keyboard_backlight_off.png') @('filename','bytes','keyboard_light','keyboard_brightness') 'Keyboard comparison: identical scene; only backlight changes.'
 function Assert-State([string]$taskName, [string]$taskProperty, [double]$taskExpected) {
  $taskRow = $taskManifest | Where-Object { $_.filename -ceq $taskName }
  if (-not $taskRow -or [math]::Abs([double]$taskRow.$taskProperty - $taskExpected) -gt 0.01) { throw "Unexpected state: $taskName / $taskProperty" }
 }
 Assert-State '14_fan.png' 'fan_speed' 240
 Assert-State '15_fan_speed.png' 'fan_speed' 480
 Assert-State '15_fan_speed.png' 'fan_level' 4
 Assert-State '16_door_open.png' 'door_angle' 100
 Assert-State '17_curtain_open.png' 'curtain_amount' 1
 Assert-State '18_wardrobe_open.png' 'wardrobe_angle' 105
 Assert-State '19_drawer_open.png' 'drawer_amount' 1
 Assert-State '20_laptop_open.png' 'laptop_amount' 1
 Assert-State '21_keyboard_backlight.png' 'keyboard_brightness' 1
 Assert-State '22_clock.png' 'clock_seconds' 36617.25
 Assert-State '23_showcase.png' 'showcase' 1
 Assert-State '24_automatic_day_cycle.png' 'day_cycle' 1
 Assert-State '25_camera_tour.png' 'tour' 1
 Assert-State '27_curtains_closed.png' 'curtain_amount' 0
 Assert-State '28_keyboard_backlight_off.png' 'keyboard_brightness' 0
 Assert-State '29_animation_paused.png' 'paused' 1
 foreach ($taskName in @('03_day.png','04_night.png')) {
  Assert-State $taskName 'x' 2.65; Assert-State $taskName 'y' 2.5; Assert-State $taskName 'z' 2.6
 }
 $taskTempDir = Join-Path $taskRoot '.tools\report-screenshots'
 New-Item -ItemType Directory -Path $taskTempDir -Force | Out-Null
 $taskTempZip = Join-Path $taskTempDir 'package.zip'
 $taskZipStream = [System.IO.File]::Open($taskTempZip,[System.IO.FileMode]::Create)
 try {
  $taskZip = [System.IO.Compression.ZipArchive]::new($taskZipStream,[System.IO.Compression.ZipArchiveMode]::Create,$true)
  try {
   foreach ($taskName in $taskNames) {
    $taskEntry = $taskZip.CreateEntry("report_screenshots/$taskName",[System.IO.Compression.CompressionLevel]::Optimal)
    $taskEntryStream = $taskEntry.Open(); $taskSourceStream = [System.IO.File]::OpenRead((Join-Path $taskImages $taskName))
    try { $taskSourceStream.CopyTo($taskEntryStream) } finally { $taskSourceStream.Dispose(); $taskEntryStream.Dispose() }
   }
  } finally { $taskZip.Dispose() }
 } finally { $taskZipStream.Dispose() }
 $taskZipStream = [System.IO.File]::OpenRead($taskTempZip)
 try {
  $taskZip = [System.IO.Compression.ZipArchive]::new($taskZipStream,[System.IO.Compression.ZipArchiveMode]::Read,$true)
  try {
   if ($taskZip.Entries.Count -ne $taskNames.Count) { throw 'ZIP entry count mismatch.' }
   foreach ($taskEntry in $taskZip.Entries) {
    $taskEntryName = $taskEntry.FullName.Substring('report_screenshots/'.Length)
    if ($taskEntry.FullName -cne "report_screenshots/$taskEntryName" -or $taskEntryName -cnotin $taskNames) { throw 'Unexpected ZIP entry.' }
    $taskEntryStream = $taskEntry.Open(); $taskHasher = [System.Security.Cryptography.SHA256]::Create()
    try { $taskHash = [BitConverter]::ToString($taskHasher.ComputeHash($taskEntryStream)).Replace('-','') } finally { $taskHasher.Dispose(); $taskEntryStream.Dispose() }
    if ($taskHash -ne (Get-FileHash -LiteralPath (Join-Path $taskImages $taskEntryName) -Algorithm SHA256).Hash) { throw 'ZIP entry failed integrity check.' }
   }
  } finally { $taskZip.Dispose() }
 } finally { $taskZipStream.Dispose() }
 Copy-Item -LiteralPath $taskTempZip -Destination $taskArchive -Force
 Write-Output "[OK] ZIP verified: $($taskNames.Count) PNG images; no source code, binaries or manifest included."
 Write-Output "Package: $taskArchive"
} finally { Pop-Location }
