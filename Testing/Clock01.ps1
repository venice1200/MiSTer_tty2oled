#
# PowerShell tty2oled Test
# See https://www.msxfaq.de/code/powershell/ps_serial.htm#lesen_und_schreiben
# 
#

$myhrs=0
$mymin=0
$mysec=0
$mysecbuf=0
$secbit=0
$seread=""

$serial = New-Object System.IO.Ports.SerialPort
$serial.PortName = "COM22"
$serial.BaudRate = "115200"
$serial.Parity = "None"
$serial.DataBits = 8
$serial.StopBits = 1
$serial.ReadTimeout = 5000

$serial.Open()

function waitforack {
  Write-Host -NoNewline "Wait for tty2oled..."
  while ($seread -ne "ttyack") {
    $seread=$serial.ReadTo(";")
  }
  Write-Host "ttyack"
  #$serial.ReadTo(";")
}

$serial.WriteLine("CMDCON,200")
waitforack
$serial.WriteLine("CMDCLS")
waitforack
$serial.WriteLine("CMDGEO,10,1,0,0,256,18,4")
waitforack
$serial.WriteLine("CMDGEO,9,1,0,18,256,46,4")
waitforack
$serial.WriteLine("CMDGEO,6,0,10,9,5,0,0")
waitforack
$serial.WriteLine("CMDTXT,1,0,45,14,tty2oled & PowerShell")
waitforack

while (1) {
  $myhrs=Get-Date -Format HH
  $mymin=Get-Date -Format mm
  $mysec=Get-Date -Format ss
  if ($mysec -ne $mysecbuf) {
	$mytime=$myhrs+":"+$mymin+":"+$mysec
    $serial.WriteLine("CMDTXT,8,1,60,50,$mytime")
    waitforack
    if ($secbit -eq 1 ) {
      $serial.WriteLine("CMDGEO,6,1,10,9,5,0,0")
	  waitforack
      $serial.WriteLine("CMDGEO,6,0,245,9,5,0,0")
	  waitforack
      $secbit=0
	}
    else {
      $serial.WriteLine("CMDGEO,6,1,245,9,5,0,0")
	  waitforack
      $serial.WriteLine("CMDGEO,6,0,10,9,5,0,0")
	  waitforack
      $secbit=1
    }
	$mysecbuf=$mysec
  }
  Start-Sleep -s 0.8 # Without this "sleep" my system has 5.5% CPU Load. With this sleep 0.1/0,2%.
}
