<?php
set_include_path(get_include_path() . PATH_SEPARATOR . "../");
set_include_path(get_include_path() . PATH_SEPARATOR . "../db_helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../rts/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/helpers/");
require_once 'set_follow_on.php';
//-----------------------------------------------------------
ob_start();
$body = file_get_contents('php://input');
if ( ( empty($body) ) || ( trim($body) == "" ) ) {
  echo '{ "InsertTest" : "ERROR", "Message" : "No payload" }'; exit;
}
$rslt =  set_follow_on($body);
header("TestID: ".$rslt["TestID"]);
ob_clean();
?>