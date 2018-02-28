<?php
set_include_path(get_include_path() . PATH_SEPARATOR . "../");
set_include_path(get_include_path() . PATH_SEPARATOR . "../db_helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../helpers/");

require_once 'rs_assert.php';
require_once 'list_rts.php';
require_once 'post_url.php';
require_once 'db_get_test.php';

function inform_rts(
  $test_id,
  &$err_msg
)
{
  //TODO P0 Needs to be fixed ASAP
  return true;

  $err_msg = "";
  $is_ok = true;
  $SP = list_rts();
  rs_assert($SP);
  if ( !is_array($SP) ) { // NOTHING MORE TO DO 
    $err_msg = "NOTICE: Not talking to RTS\n";
    return true;  
  } 
  foreach ( $SP  as $sp ) {
    $rslt = ""; $http_code = 0;
    $server = $sp['server']; $port   = $sp['port'];
    post_url($server, $port, "AddTest", $body, $http_code, $rslt);
    if ( $http_code != 200 ) { $err_msg = $rslt; $is_ok = false; }
  }
  return $is_ok;
}
?>