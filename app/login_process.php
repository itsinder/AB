<?php
// -- SET PATH
require_once "set_path.php";
// -- CALL REQUIRED FILES
require_once "db_get_row.php";

function is_admin(
  $admin
)
{
  $rslt = db_get_row("admin", "name", $admin); 
  if ( !$rslt ) { header('Location: index.php?error=User does not exist FILE: ' . __FILE__ . ' :LINE: ' . __LINE__ . ''); return false; } else { return true; }
}

$admin_name = "";
if (isset($_GET['User'])) { $admin_name = $_GET['User']; } 
else { header('Location: index.php?error=User name is not set FILE: ' . __FILE__ . ' :LINE: ' . __LINE__ . ''); return false;};
 
if ( is_admin($admin_name) ) {
  session_start();
  $_SESSION['User'] = $admin_name;
if (isset($_GET['TestType'])) { $_SESSION['TestType'] = $_GET['TestType']; }  
else { header('Location: index.php?error=Test Type is not set FILE: ' . __FILE__ . ' :LINE: ' . __LINE__ . ''); return false;}
    header('Location: home.php');
}
else {
  header('Location: index.php?error=User does not exist');
}
?>
