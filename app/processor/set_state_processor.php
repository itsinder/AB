<?php session_start();
error_reporting( E_ALL );
# -- PARSE SESSION VARIABLES
// -- PARSE SESSION VARIABLES
if (isset($_SESSION['User']))
	{
		$User = $_SESSION['User'];
	}
else
	{
		header('Location: index.php?error=User name not set FILE: ' . __FILE__ . ' :LINE: ' . __LINE__ . '');
		return false;
	}
set_include_path(get_include_path() . PATH_SEPARATOR . "../../php/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../../php/db_helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../../php/helpers/");
set_include_path(get_include_path() . PATH_SEPARATOR . "../../php/rts/");

require_once 'set_state.php';
require_once 'db_get_test.php';
//require_once 'cli_del_test.php';
//require_once '../create_good_json_test.php';
//-----------------------------------------------------------
//-------- ACCESS POST parameters

function action_state($state_id) {
  switch ($state_id) {
    case '1':
      return 'dormant';
    break;

    case '2':
      return 'started';
    break;

    case '3':
      return 'terminated';
    break;

    case '4':
      return 'archived';
    break;
  }
}


// GET DATA 
if (isset($_GET['TestID']))
	{
		$tid = $_GET['TestID'];
	}
else
	{
		header('Location: home.php?error=Test ID not set FILE: ' . __FILE__ . ' :LINE: ' . __LINE__ . '');
		return false;
	}
$X = db_get_test($tid);

// CASE: FIX TO A WINNER
$X['NewState'] = action_state($_GET['state_id']);
if ((isset($_GET['Winner'])) && ($_GET['state_id'] == "2")) {
  $X['Winner'] = $_GET['Winner'];
}
// CASE: RESURRECT
if( $_GET['state_id'] == "4" && (isset($_GET['action']) == "resurrect")) {
  $X['NewState'] = "started";
}
// CASE: DELETE
if( $_GET['state_id'] == "1" && (isset($_GET['action']) == "delete")) {
  $X['NewState'] = "archived";
}

$X['Updater'] = $User;
$str_inJ = json_encode($X);
//-------------------------------------
// Call to set state
$outJ = set_state($str_inJ);

header('Location: ../home.php?TestID='.$tid);

?>
