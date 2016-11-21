<?php
  ini_set('display_errors',0);
  ini_set('display_startup_errors',0);
  $delay_time = @$_GET["delay_time"]?$_GET["delay_time"]:"";
  if ($delay_time == "") {
    echo "you send nothing";
    exit();
  }
  $ret_data = file_get_contents("http://192.168.0.201:8080/delay_time/".$delay_time);
  echo $ret_data;
?>
