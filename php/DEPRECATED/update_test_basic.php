<?php
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/");
require_once 'lkp.php';
require_once 'make_seed.php';
require_once 'get_time_usec.php';
require_once 'aux_chk_name.php';
require_once 'aux.php';
require_once 'get_json_element.php';

function update_test_basic(
  $str_inJ
)
{
  $outJ['status'] = "error";
  $outJ['stdout'] = "";
  $outJ['stderr'] = "";
  // START Check inputs
  assert(isset($str_inJ));
  assert(is_string($str_inJ));
  $inJ = json_decode($str_inJ); assert(!is_null($inJ));
  $test_id   = get_json_element($inJ, 'TestID'); 
  $test_name = get_json_element($inJ, 'TestName'); 
  $test_type = get_json_element($inJ, 'TestType'); 
  $test_dscr = get_json_element($inJ, 'TestDescription'); 
  $updater   = get_json_element($inJ, 'Updater');
  $variants  = get_json_element($inJ, 'Variants');
  assert(is_array($variants));
  $nV = count($variants);
  assert($nV > 0 );
  assert($nV >= lkp('configs', "min_num_variants"));
  assert($nV <= lkp('configs', "max_num_variants"));

  if ( isset($test_dscr) ) {
    assert(is_string($test_dscr));
    assert(strlen($test_dscr) <= lkp("configs", "max_len_test_dscr"));
  }
  assert(aux_chk_name($test_name), "test name is invalid");
  assert(strlen($test_name) <= lkp("configs", "max_len_test_name"));
  $test_type_id = lkp("test_type", $test_type);
  $updater_id   = lkp("admin", $updater);

  $variant_ids   = array($nV);
  $variant_names = array($nV);
  $variant_percs = array($nV);
  $variant_urls  = array($nV);
  $vidx = 0;
  foreach ( $variants as $v ) { 
    $vname = $v->{'Name'};
    assert(isset($vname));
    assert(is_string($vname));

    $vid = $v->{'ID'};
    assert(isset($vid));
    assert(is_string($vid));
    $vid = intval($vid);
    assert($vid > 0);

    $perc = $v->{'Percentage'};
    assert(isset($perc));
    assert(is_string($perc));
    $perc = floatval($perc);
    
    $url = $v->{'URL'};
    assert(isset($url));
    assert(is_string($url));

    $variant_names[$vidx] = $vname;
    $variant_ids[$vidx]   = $vid;
    $variant_percs[$vidx] = $perc;
    $variant_urls[$vidx]  = $url;

    $vidx++;
  }
  assert(is_good_variants($variant_names));
  assert(is_good_urls($variant_urls));
  assert(is_good_percs($variant_percs));
  assert(is_good_test_name($test_name, $test_type, $test_id));
  // STOP Check inputs
  //----------------------------------------------------
  $t = db_get_test($test_id);
  assert($t, "No test [$test_name] of type [$test_type]");
  $state = lkp("state", $t['state_id'], true); //reverse lkp
  assert(( $state != "terminated" ) && ( $state != "archived" ) );

  $d_update = get_date(); 
  $t_update = get_time_usec(); 
  $X1['description']  = $test_dscr;
  $X1['d_update']     = $d_update;
  $X1['t_update']     = $t_update;
  $X1['updater_id']   = $updater_id;
  $X1['name']         = $test_name;
  //-----------------------------------------------
  $dbh = dbconn(); assert(isset($dbh)); 
  try {
    $dbh->beginTransaction();
    mod_row("test", $X1, "where id = $test_id ");
    $X2['t_update'] = $t_update;
    $X2['d_update'] = $d_update;
    //-------------------------------------------------------
    for ( $i = 0; $i < $nV; $i++ ) { 
      $X2['percentage']  = $variant_percs[$i];
      // Can change some stuff only when draft 
      if ( ( $state == "draft" ) || ( $state == "dormant" ) ) { 
        $X2['name']        = $variant_names[$i];
        if ( $test_type == "XYTest" ) {
          $X2['url']        = $variant_urls[$i];
        }
      }
      mod_row("variant", $X2, "where id = " . $variant_ids[$i]);
    }
    //------------------------------------------
    $dbh->commit();
  } catch ( PDOException $ex ) {
    $dbh->rollBack();
    $GLOBALS["err"] .= "ERROR: Transaction aborted\n";
    $GLOBALS["err"] .= "FILE: " . __FILE__ . " :LINE: " . __LINE__ . "\n";
    return false;
  }
  //------------------------------------------
  $outJ["status"] = "ok";
  $outJ["test_id"] = $test_id;
  return $outJ;
}
