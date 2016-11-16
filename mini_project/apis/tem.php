<?php
  ini_set('display_errors',0);
  ini_set('display_startup_errors',0);

  $ret_data = file_get_contents("http://192.168.0.201:8080/tem");
  echo $ret_data;
?>
