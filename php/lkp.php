<?php
require_once 'load_globals.php';
require_once 'load_configs.php';
require_once 'rs_assert.php';

function lkp(
  $tbl, 
  $val,
  $reverse = false
)
{
  if ( $reverse ) { 
    $tbl = "reverse_" . $tbl; 
  }
  if ( !isset($tbl) ) { 
    rs_assert(null, "Table $tbl not specified"); 
  }
  if ( !isset($val) ) { 
    rs_assert(null, "Value $val not specified"); 
  }
  if ( !isset($GLOBALS[$tbl]) ) {
    load_configs();
    load_globals();
  }
  rs_assert(isset($GLOBALS[$tbl]), "Table [$tbl] not loaded");
  if (!isset($GLOBALS[$tbl][$val]) ) {
    echo("UNDEFINED $tbl, $val\n");
  }
  rs_assert(isset($GLOBALS[$tbl][$val]), 
    "No value for key [$val] in table [$tbl] ");
  $x = $GLOBALS[$tbl][$val];
  // echo "<br> =ZZ= <br> \n"; var_dump($GLOBALS['state']); echo "<br> =WW= <br> \n"; 
  return $x;
}
?>
