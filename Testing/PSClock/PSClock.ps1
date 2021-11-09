#
# tty2oled PSClock
# See https://www.msxfaq.de/code/powershell/ps_serial.htm#lesen_und_schreiben
# 
#

$myhrs=24
$mymin=61
$mysec=61
$myhrsbuf=00
$myminbuf=00
$mysecbuf=00
$secbit=0
$seread=""
#$contrast=100
$contrast=50

$serial = New-Object System.IO.Ports.SerialPort
$serial.PortName = "COM5"
#$serial.PortName = "COM6"
#$serial.PortName = "COM22"
$serial.BaudRate = "115200"
$serial.Parity = "None"
$serial.DataBits = 8
$serial.StopBits = 1
#$serial.ReadTimeout = 5000
$serial.ReadTimeout = 500
$serial.DiscardInBuffer
$serial.DiscardOutBuffer

$serial.Open()

function waitforack {
  #Write-Host -NoNewline "Wait for tty2oled..."
  while ($seread -ne "ttyack") {
    $seread=$serial.ReadTo(";")
  }
  #Write-Host "ttyack"
  #$serial.ReadTo(";")
}

$serial.WriteLine("CMDCON,"+$contrast)
waitforack
$serial.WriteLine("CMDCLS")
waitforack
$serial.WriteLine("CMDTXT,0,15,0,0,8,tty2oled PSClock")
waitforack
$serial.WriteLine("CMDTXT,8,15,0,15,58,00:")
waitforack
$serial.WriteLine("CMDTXT,8,15,0,97,58,00:")
waitforack
$serial.WriteLine("CMDTXT,8,15,0,179,58,00")
waitforack
$serial.WriteLine("CMDDUPD")
waitforack
Start-Sleep -s 2.00

while (1) {
  $myhrs=Get-Date -Format HH
  $mymin=Get-Date -Format mm
  $mysec=Get-Date -Format ss
  
  if ($myhrs -ne $myhrsbuf) {
    $serial.WriteLine("CMDTXT,108,0,0,15,58,"+$myhrsbuf+":")
    waitforack
    $serial.WriteLine("CMDTXT,108,15,0,15,58,"+$myhrs+":")
    waitforack
    $myhrsbuf=$myhrs
  }

  if ($mymin -ne $myminbuf) {
    $serial.WriteLine("CMDTXT,108,0,0,97,58,"+$myminbuf+":")
    waitforack
    $serial.WriteLine("CMDTXT,108,15,0,97,58,"+$mymin+":")
    waitforack
    $serial.WriteLine("CMDTXT,0,10,0,220,8,TEP184")
    waitforack
    $myminbuf=$mymin
  }

  if ($mysec -ne $mysecbuf) {
    $serial.WriteLine("CMDTXT,108,0,0,179,58,"+$mysecbuf)
    waitforack
    $serial.WriteLine("CMDTXT,108,15,0,179,58,"+$mysec)
    waitforack
    $serial.WriteLine("CMDDUPD")
    waitforack
    $mysecbuf=$mysec
    Start-Sleep -s 0.85 # Without "sleep" = 5.5% CPU, with sleep 0.2%
  }
}
