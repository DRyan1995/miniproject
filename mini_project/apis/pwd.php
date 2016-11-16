<?php
  ini_set('display_errors',0);
  ini_set('display_startup_errors',0);
  $pwd = @$_GET["pwd"]?$_GET["pwd"]:"";
  if ($pwd == "") {
    echo "nothing";
    exit();
  }
  $ret_data = file_get_contents("http://localhost:8080/pwd/".$pwd);
  echo $ret_data;
?>
