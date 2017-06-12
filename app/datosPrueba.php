<?php
	ini_set("display_errors", 1);
  $arduino=$_POST['arduino'];
  $sensor=$_POST['sensor'];
  $valor=$_POST['valor'];
  $valores=["1235","7890","1235","7890","1235","7890","1235","7890","1235","7890","1235","7890","174","149","247","12","76","231","53","109","206","121","97","241"];

  $puerta=rand(1,3);
  if ($sensor==0) {
    sleep(0.5);
    switch ($arduino) {
      case 'A':
        if ($puerta==1)
          echo $arduino.":".$valores[rand(0,count($valores)-1)];
        else
          echo $arduino.":no";
        break;
      case 'B':
        if ($puerta==2)
          echo $arduino.":".rand(0,5);
        else
          echo $arduino.":no";
        break;
      case 'C':
        if ($puerta==3)
          echo $arduino.":".$valores[rand(0,count($valores)-1)];
        else
          echo $arduino.":no";
        break;
      
      default:
        echo $arduino.":1-".rand(12,32).":2-".str_replace("0","X",rand(0,1)).":3-".rand(12,32).":4-".rand(12,32);
        break;
      }
    } else {
//      sleep(1);
      if ($arduino==8)
        echo $valor;
      else if ($valor==1)
        echo "On";
      else
        echo "Off";
    }
?>