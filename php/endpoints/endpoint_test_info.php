<?php
set_include_path(get_include_path() . PATH_SEPARATOR . "../");
set_include_path(get_include_path() . PATH_SEPARATOR . "../db_helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../rts/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/helpers/");
require_once 'db_get_test.php';
require_once 'get_json_element.php';
//-----------------------------------------------------------
if ( ( !empty($_GET) ) && ( isset($_GET['TestID']) ) ) {
  $test_id= $_GET['TestID'];
}
else {
  $body = file_get_contents('php://input');
  if ( ( empty($body) ) || ( trim($body) == "" ) ) {
  $err = '{ "TestInfo" : "ERROR", "Message" : "No payload" }'; 
  header("Error-Message: " . nl2br($err));
  header("Error-Code: 400");
  http_response_code(400);
  exit;
  }
  $X = json_decode($body); 
  if ( !$X ) { 
  $err = '{ "TestInfo" : "ERROR", "Message" : "Bad payload" }'; 
  header("Error-Message: " . nl2br($err));
  header("Error-Code: 400");
  http_response_code(400);
  exit;
  }
  $test_id = get_json_element($X, "TestID");
}
$test_id = intval($test_id);
$rslt =  db_get_test($test_id);
header("TestID: $test_id");
echo json_encode($rslt, JSON_PRETTY_PRINT);
?>
