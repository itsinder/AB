<?php
// -- SET PATH
require_once "set_path.php";
// -- STANDARD HEADER INFORMATION
require_once "header.php";
require_once "lkp.php";
$user_id = lkp("admin", $User);
// -- GET TESTS
require_once "dbconn.php";
require_once "db_get_row.php";
require_once "db_get_rows.php";
require_once "utility_functions.php";
// Default
$db_state = 'started';
if (isset($TestType))
	{
		if ($TestType == "ABTest")
			{
				$test_type_id = 1;
			}

		if ($TestType == "XYTest")
			{
				$test_type_id = 2;
			}

if ((isset($_GET['TestID'])) && ($_GET['TestID'] != "")){
  $id = $_GET['TestID'];
  $rslt = db_get_row("test", "id", $id);
  $db_state = lkp("state", $rslt["state_id"], "reverse");
  $user_id = $rslt["creator_id"];
  $User = lkp("admin", $user_id, "reverse");
}
switch ($db_state) {
    case "draft":
        $state_1 = 'draft';
				$state_2 = 'dormant';
        break;
    case "dormant":
        $state_1 = 'draft';
				$state_2 = 'dormant';
        break;
    case "started":
        $state_1 = 'started';
				$state_2 = 'terminated';
        break;
    case "terminated":
        $state_1 = 'started';
				$state_2 = 'terminated';
        break;
    case "archived":
        $state_1 = 'archived';
        break;
    default:
        $state_1 = 'started';
				$state_2 = 'terminated';
}
$state_id_1 = lkp("state", $state_1);
$result = db_get_rows("test", "test_type_id = " . $test_type_id . " and (state_id = $state_id_1 ) and creator_id = '".$user_id."' order by updated_at DESC");
// If 2 states are filtered
if (isset($state_2)) 
  { 
  $state_id_2 = lkp("state", $state_2); 
  $result = db_get_rows("test", "test_type_id = " . $test_type_id . " and (state_id = $state_id_1 or state_id = $state_id_2) and creator_id = '".$user_id."' order by updated_at DESC");
}
        }
require_once "html_header.php";
?>
<!-- PAGE SPECIFIC FILE CALLS -->
  <link href="css/dataTables.min.css" rel="stylesheet">
  <script src="js/dataTables.min.js"></script>
  <script src="js/home.js"></script>
</head>
<body>
<!-- STANDARD HEADER INFORMATION -->
<?php
require_once "navbar.php";
 ?>
<div class="container theme-showcase" role="main"> 
  <div class="row">
    <div class="col-xs-12">
      <div class="panel panel-primary">
        <div class="panel-heading">
          <h3 class="panel-title">Test Table &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Admin  &nbsp;&nbsp;
            <select form="FilterAdmin" name='TestAdmin' id = "TestAdmin" style='color:white;background-color:grey;'>";
              <option value=''>All</option>
<?php 
$admin = db_get_rows('admin');
$nA = count($admin);
for ( $i = 0; $i < $nA; $i++ ) { 
  echo "<option value='".$admin[$i]['id']."'";
  if((isset($User)) && ($User == $admin[$i]['name'])) {
    echo ' selected';
    }
  echo ">".$admin[$i]['name']."</option>";
} 
?>
              </select>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	          <input type="radio" name="option" class="opt" value="'3','4'" id= "1" 
<?php if (($state_1 == 'started') || ($state_1 == 'terminated') ) {
  echo "checked";
} 
?>
/>&nbsp;Started/Terminated  &nbsp;&nbsp;
  <input type="radio" name="option" class="opt" value="2" id="2" 
<?php if (($state_1 == 'draft') || ($state_1 == 'dormant') ) {
  echo " checked";
} 
?> />&nbsp;Draft/Dormant &nbsp;&nbsp; 
  <input type="radio" name="option" class="opt" value="3" id="3" 
<?php if ($state_1 == "archived") {
  echo "checked";
} 
?> />&nbsp;Archive &nbsp;&nbsp; 
	<input type="hidden" name="TestType" id="TestType" value="
<?php
  echo $TestType; 
?>
">
<!-- Call Modal -->
  <a href="#" data-toggle="modal" data-target="#basicModal">
    <button type="button" class="btn btn-success btn-xl">
      <span style="font-size:25px;"><strong>+</strong></span>
     </button>
  </a>
  </h3>
</div>
<div class="panel-body">
<!-- AJAX ERROR DIV START -->
<?php require_once "error_div.php"; ?>
<!-- AJAX ERROR DIV END -->
<?php
if (isset($TestType))
	{
		require_once "test_table.php";

	}
else
	{
		echo "<h1>Test Type not defined.</h1>";
	}
?>
         </div>
       </div>
     </div>
   </div>

 </div> 
<!-- /container -->
<?php
require_once "choose_num_variants.php";
require_once "prompt_clone_a_test.php";
 ?>
<!-- FOOTER -->
<?php
require_once "footer.php";
 ?>

