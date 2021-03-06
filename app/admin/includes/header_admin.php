<?php
error_reporting(E_ALL);
ini_set('display_errors', '1');
require_once "../utility_functions.php";
?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
    <meta name="description" content="Assures Quality with business logic">
    <meta name="author" content="Lua Q Scrabble" >
    <link rel="icon" href="images/favicon.ico">

    <title>NW Admin Panel</title>

    <!-- Bootstrap core CSS -->
    <link href="../bootstrap-3.3.5-dist/css/bootstrap.min.css" rel="stylesheet">
    <!-- Bootstrap theme -->
    <link href="../bootstrap-3.3.5-dist/css/bootstrap-theme.min.css" rel="stylesheet">
    <!-- Custom styles for this template -->
    <link href="../css/theme.css" rel="stylesheet">
    <link href="../css/error.css" rel="stylesheet">

    <!-- jQuery  -->
    <script src="../js/jquery-1.11.1.min.js"></script>
    <script src="../js/bootstrap.min.js"></script>
		<script src="js/check_tests.js"></script>
	
  </head>

  <body >
    <!-- Fixed navbar -->
    <div id="msg" align="center" class="msg" style="z-index:100011;display:none;font-family:Tahoma,Arial;"></div>
    <nav class="navbar navbar-inverse navbar-fixed-top">
    <div class="container">
    <div class="navbar-header">
    <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
    <span class="sr-only">Toggle navigation</span>
    <span class="icon-bar"></span>
    <span class="icon-bar"></span>
    <span class="icon-bar"></span>
    </button>
    <a class="navbar-brand" href="admin.php">NW A/B | Admin</a>
    </div>
    <div id="navbar" class="navbar-collapse collapse">
    <ul class="nav navbar-nav navbar-right">
    <li ><a href="../home.php"><button type="button" class="btn btn-sm btn-success"><span class="glyphicon glyphicon-home"></span> Main App</button></a></li>
    <li ><a href="list_admin.php"><button type="button" class="btn btn-sm btn-primary"><span class="glyphicon glyphicon-list-alt"></span> Admin(s)</button></a></li>
    <li><a href="list_channel.php"><button type="button" class="btn btn-sm btn-primary"><span class="glyphicon glyphicon-list-alt"></span> Channel(s)</button></a></li>
    <li><a href="check_tests.php"><button type="button" class="btn btn-sm btn-primary"> Check Tests</button></a></li>
    <!--<li ><a href="call_reload.php"><button type="button" class="btn btn-sm btn-info"><span class="glyphicon glyphicon-refresh"></span> RTS</button></a></li>
    <li ><a href="check_rts.php"><button type="button" class="btn btn-sm btn-info"><span class="glyphicon glyphicon-check"></span> RTS</button></a></li>
    <li ><a href="chk_scn.php"><button type="button" class="btn btn-sm btn-info"> SCN</button></a></li>	
    <li ><a href="ui_monitor.php"><button type="button" class="btn btn-sm btn-info">AB Monitor</button></a></li>
    <li ><a href="ur_ui_monitor.php"><button type="button" class="btn btn-sm btn-info">UR Monitor</button></a></li>-->
    <li ><a href="show_dump.php"><button type="button" class="btn btn-sm btn-info"><span class="glyphicon glyphicon-download-alt"></span> Data</button></a></li>
    </ul>
    </div><!--/.nav-collapse -->
    </div>
    </nav>

<!---------------------------------->
    <div class="modal fade" id="TestUrl" tabindex="-1" role="dialog" aria-labelledby="TestUrl" aria-hidden="true">
    <div class="modal-dialog modal-lg">
    <div class="modal-content">
    <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
    <h4 class="modal-title" id="myModalLabel">Test URL</h4>
    </div>
    <form class="test" name="test" action="admin_test_url.php">
    <div class="modal-body">
    <div class="form-group">
    <label class="label" for="admin" >URL:</label>
    <input class="form-control" id="url" name="url" required>
    </div>					
    </div>
    <div class="modal-footer">
    <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
    <input class="btn btn-success" type="submit" value="Test" id="submit">
    </div>
    </form>
    </div>
    </div>
    </div>
