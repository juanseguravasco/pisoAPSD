<?php
  ini_set("display_errors", 1);
  $port="/dev/ttyUSB0";
//    $port="/dev/ttyACM0";
  $fp=fopen($port,"c+");

  $arduino=$_POST['arduino'];
  $orden=escapeshellcmd($_POST['arduino']."-".$_POST['sensor']."-".$_POST['valor'].";");
//  $orden=escapeshellcmd("9-9-1;");
// Set timeout to 500 ms
$timeout=microtime(true)+0.5;
//#########################$port="/dev/ttyACM0";
// Set device controle options (See man page for stty)
//exec("/bin/stty -F ".$port." 9600 sane raw cs8 hupcl cread clocal -echo -onlcr ");
    
// Open serial port
//###############$fp=fopen($port,"c+");
if(!$fp) die("Can't open device");

if ($arduino=="A" || $arduino=="B" || $arduino=="C")
  sleep(0.1);//Retardo 3 s. para probar arduino de teclado
//else
//  sleep(0.5);//Retardo 3 s. para probar arduino de teclado

// Set blocking mode for writing
stream_set_blocking($fp,1);
fwrite($fp,$orden);

// Esperamos a leer
//usleep(50000);
usleep(50000);


// Set non blocking mode for reading
stream_set_blocking($fp,0);
$line="";
do{
  // Try to read one character from the device
  $c=fgets($fp);

  // Wait for data to arive 
  if($c == false){
//      usleep(50000);
//      continue;
  }  
  else
//  if ($c!="\n")
	  $line.=$c;
    
}while($c!="\n" && $c!=";" && microtime(true)<$timeout); 
  
echo "$line";  
?>
