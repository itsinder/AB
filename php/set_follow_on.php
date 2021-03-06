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
require_once 'mod_cell.php'; // UTPAL: Added this line
require_once 'find_tests_to_follow.php';
require_once 'start_log.php';

function set_follow_on(
  $str_inJ
)
{
  //-- START: For logging
  $ret_val = start_log($str_inJ, "set_follow_on");
  $created_at = $ret_val['created_at'];
  $updated_at = $ret_val['updated_at'];
  $t_create   = $ret_val['t_create'];
  $t_update   = $ret_val['t_update'];
  $api_id     = $ret_val['api_id'];
  $request_webapp_id = $ret_val['request_webapp_id'];
  //-- STOP: For logging

  // START Check inputs
  rs_assert(!empty($str_inJ));
  rs_assert(is_string($str_inJ), "input not string");
  $inJ = json_decode($str_inJ); 
  rs_assert(gettype($inJ) != "string");
  rs_assert($inJ, "invalid JSON");
  $tid = get_json_element($inJ, 'tid'); 
  $tid_to_follow = get_json_element($inJ, 'tid_to_follow'); 
  //-----------------------------------------------
  // START Check inputs
  rs_assert(is_numeric($tid));
  rs_assert(is_numeric($tid_to_follow));
  $T1 = db_get_row("test", "id", $tid);
  rs_assert($T1);
  $test_name = $T1['name'];
  $external_id = $T1['external_id'];

  $state_id = $T1['state_id'];
  $state = lkp("state", $state_id, "reverse");
  rs_assert($state == "draft", "Can set follow on only in draft state");

  if ( $tid_to_follow == 0 ) { 
    mod_cell("test", "pred_id", NULL, "id = $tid ");
  }
  else {
    $T2 = db_get_row("test", "id", $tid_to_follow);
    rs_assert($T2);
    $name_to_follow = $T2['name'];
    $external_id_to_follow = $T2['external_id'];

    $channel_id = $T1['channel_id'];
    rs_assert($channel_id, "Can set follow on only for test in channel");
    $channel = lkp("channel", $channel_id, "reverse");
    rs_assert($channel);
    $F = find_tests_to_follow($channel);
    $found = false;
    foreach ( $F as $f ) { 
      if ( $f['id'] == $tid_to_follow ) { $found = true; break; }
    }
    rs_assert($found, "Not a valid test to follow");

    $X['pred_id'] = $tid_to_follow;
    $X['external_id'] = $external_id_to_follow;
    mod_row("test", $X, " where id = $tid");
  }
  //------------------------------------------
  $outJ["rts_code"] = 0; 
  $outJ["msg_stdout"] = 
    "Test [$test_name:$tid] will follow [$name_to_follow:$tid_to_follow]";
  $outJ["TestID"] = $tid; 
  return $outJ;
}
