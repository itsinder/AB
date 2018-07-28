<?php
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/db_helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/rts/");
require_once 'dbconn.php';
require_once 'insert_row.php'; // NO PROBLEM
require_once 'get_time_usec.php'; // NO PROBLEM
require_once 'lkp.php';
require_once 'get_json_element.php';
require_once 'mod_row.php';
require_once 'mod_cell.php';
require_once 'inform_rts.php';

function set_device_specific_variant(
  $str_inJ
)
{
  //--- Logging 
  $created_at =  $updated_at = get_date(); 
  $t_create =  $t_update = get_time_usec(); 
  $api_id   = lkp("api", "set_device_specific_variant");
  $X0['created_at'] = $created_at;
  $X0['t_create'] = $t_create;
  $X0['payload']  = $str_inJ;
  $X0['api_id']   = $api_id;
  $request_webapp_id = insert_row("request_webapp", $X0);
  $_SESSION['REQUEST_WEBAPP_ID'] = $request_webapp_id;

  // START Check inputs
  rs_assert(!empty($str_inJ));
  rs_assert(is_string($str_inJ), "input not string");
  $inJ = json_decode($str_inJ); rs_assert($inJ, "invalid JSON");
  $tid = get_json_element($inJ, 'id'); 
  $is_dev_specific = intval(get_json_element($inJ, 'is_dev_specific'));
  $T = db_get_row("test", "id", $tid);
  rs_assert($T);
  $test_name = $T['name'];
  $state_id = $T['state_id'];
  $state = lkp("state", $state_id, "reverse");
  $dxv = get_json_element($inJ, 'DeviceCrossVariant'); 
  rs_assert($dxv);
  if ( !$is_dev_specific ) { // Nothing to do. Quit early
    $outJ["rts_code"] = 0;
    $outJ["msg_stdout"] = "No change to device specific  info for $tid";
    $outJ["TestID"] = $tid; 
    return $outJ;
  }
  // TODO P1 Put updates in transaction
  mod_cell("test", "is_dev_specific", $is_dev_specific, " id = $tid ");
  foreach ( $dxv as $d => $v ) { 
    $device_id   = lkp("device", $d);
    rs_assert(is_array($v));
    $sum = 0;
    foreach ( $v as $vv ) { 
      $p = $vv->{'percentage'};
      rs_assert(is_numeric($p), "percentage must be a number");
      rs_assert($p >=   0, "percentage must not be negative");
      rs_assert($p <= 100, "percentage cannot exceed 100");
      $sum += $p;
    }
    rs_assert(( $sum < 100 + 0.0001 ) && ( $sum > 100 - 0.0001 ) );
    foreach ( $v as $vv ) { 
      $percentage = $vv->{'percentage'};
      $variant_id = $vv->{'variant_id'};
      $device_id  = $vv->{'device_id'};
      mod_cell("device_x_variant", "percentage", $percentage, 
        " device_id = $device_id and variant_id = $variant_id ");
    }
  }
  //------------------------------------------
  $http_code = 0;
  if ( $state == "started" ) {
    $http_code = 200; 
    $rts_error_msg = "";
    $status = inform_rts($test_id, $rts_err_msg);
    if ( !$status ) {$http_code = 400; $outJ['msg_stderr'] = $rts_err_msg;}
  }
  $outJ["rts_code"] = $http_code;
  $outJ["msg_stdout"] = "Set Device Specific Variants for [$test_name] ";
  $outJ["TestID"] = $tid; 
  return $outJ;
}
