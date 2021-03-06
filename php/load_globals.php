<?php
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/db_helpers/");
require_once 'db_get_rows.php';
require_once 'rs_assert.php';
function make_lkp($tbl)
{

  $R = db_get_rows($tbl);
  rs_assert(!is_null($R));
  $cnt = 0;
  foreach ( $R as $r ) {
    $id   = $r['id'];
    $name = $r['name'];
    if ( ( isset($r['is_del']) ) && ( $r['is_del'] == 0 ) ) { 
      $X[$name] = $id;
      $reverse_X[$id] = $name;
      $cnt++;
    }
  }
  rs_assert($cnt > 0);
  $GLOBALS[$tbl] = $X;
  $GLOBALS["reverse_" . $tbl] = $reverse_X;
  // print("<br> Created $tbl with " . count($R) . " rows <br\n");
}
//------------------------------------------
function load_globals()
{
  make_lkp("test_type");
  make_lkp("state"); 
  make_lkp("bin_type");
  make_lkp("admin");
  make_lkp("api");
  make_lkp("channel");
  make_lkp("device");
  make_lkp("attr");
  make_lkp("attr_type");
  make_lkp("cat_attr_val");
}
/*
load_globals();
var_dump($GLOBALS['cat_attr_val']);
 */
?>
