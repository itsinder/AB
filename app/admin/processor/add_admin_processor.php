<?php
error_reporting( E_ALL );
set_include_path(get_include_path() . PATH_SEPARATOR . "../../../php/db_helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../../../php/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../../../php/helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../../../php/db_helpers/");
require_once "add_admin_channel.php";

//-----------------------------------------------------------
//-------- ACCESS POST parameters

ob_start();
if ( !$_POST ) {
  echo '{ "Add Admin" : "ERROR", "Message" : "No paylaod" }'; exit;
}
$admin_name = $_POST['admin_name'];
$X = array();
$X['Table'] = 'admin';
$X['Name'] = $admin_name;
$str_inJ = json_encode($X);
if ( !$str_inJ ) {
  echo '{ "Add Admin" : "ERROR", "Message" : "Bad JSOn" }'; exit;
}
//-------------------------------------
// Call to add admin
$rslt =  add_admin_channel($str_inJ);
ob_clean();
?>
