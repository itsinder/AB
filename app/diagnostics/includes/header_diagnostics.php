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
    <meta name="author" content="Nerdwallet QA Team">
    <link rel="icon" href="images/favicon.ico">

    <title>NW A/B Diagnostics Panel</title>

    <!-- Bootstrap core CSS -->
    <link href="../bootstrap-3.3.5-dist/css/bootstrap.min.css" rel="stylesheet">
    <!-- Bootstrap theme -->
    <link href="../bootstrap-3.3.5-dist/css/bootstrap-theme.min.css" rel="stylesheet">
    <!-- Custom styles for this template -->
    <link href="../css/theme.css" rel="stylesheet">

    <!-- jQuery  -->
    <script src="../js/jquery-1.11.1.min.js"></script>
    <script src="../js/bootstrap.min.js"></script>
	
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
    <a class="navbar-brand" href="diagnostics.php">NW A/B | Diagnostics</a>
    </div>
    <div id="navbar" class="navbar-collapse collapse">
    <ul class="nav navbar-nav navbar-right">
    <li ><a href="../home.php"><button type="button" class="btn btn-sm btn-success"><span class="glyphicon glyphicon-home"></span> Main App</button></a></li>
    <li ><a href="list_admin.php"><button type="button" class="btn btn-sm btn-primary"><span class="glyphicon glyphicon-list-alt"></span> Admin(s)</button></a></li>
    <li><a href="list_channel.php"><button type="button" class="btn btn-sm btn-primary"><span class="glyphicon glyphicon-list-alt"></span> Channel(s)</button></a></li>
    <li ><a href="#" data-toggle="modal" data-target="#TestUrl"><button type="button" class="btn btn-sm btn-primary">Test Url</button></a></li>
    <li ><a href="#" data-toggle="modal" data-target="#TestUserAgent"><button type="button" class="btn btn-sm btn-info">Test User Agent</button></a></li>
    <li ><a href="list_rts.php"><button type="button" class="btn btn-sm btn-primary">List RTS</button></a></li>
    <li ><a href="query_rts.php"><button type="button" class="btn btn-sm btn-primary">Query RTS</button></a></li>
    <li ><a href="show_config.php"><button type="button" class="btn btn-sm btn-info">Show Config</button></a></li>
    </ul>
    </div><!--/.nav-collapse -->
    </div>
    </nav>
<!----------------------------------------->			
    <div class="modal fade" id="AddAdmin" tabindex="-1" role="dialog" aria-labelledby="AddAdmin" aria-hidden="true">
    <div class="modal-dialog modal-lg">
    <div class="modal-content">
    <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
    <h4 class="modal-title" id="myModalLabel">Add admin</h4>
    </div>
    <form class="test" name="test" action="add_admin_process.php">
    <div class="modal-body">
    <div class="form-group">
    <label class="label" for="admin" >Admin Name:</label>
    <input class="form-control" id="admin" name="admin_name" required>
    </div>					
    </div>
    <div class="modal-footer">
    <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
    <input class="btn btn-success" type="submit" value="Add Admin" id="submit">
    </div>
    </form>
    </div>
    </div>
    </div>
<!---------------------------------->
<! -- add channel -->			
    <div class="modal fade" id="AddChannel" tabindex="-1" role="dialog" aria-labelledby="AddChannel" aria-hidden="true">
    <div class="modal-dialog modal-lg">
    <div class="modal-content">
    <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
    <h4 class="modal-title" id="myModalLabel">Add Channel</h4>
    </div>
    <form class="test" name="test" action="add_channel_process.php">
    <div class="modal-body">
    <div class="form-group">
    <label class="label" for="channel" >Channel:</label>
    <input class="form-control" id="channel" name="channel" required>
    </div>					
    </div>
    <div class="modal-footer">
    <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
    <input class="btn btn-success" type="submit" value="Add Channel" id="submit">
    </div>
    </form>
    </div>
    </div>
    </div>
<!---------------------------------->
    <div class="modal fade" id="TestUrl" tabindex="-1" role="dialog" aria-labelledby="TestUrl" aria-hidden="true">
    <div class="modal-dialog modal-lg">
    <div class="modal-content">
    <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
    <h4 class="modal-title" id="myModalLabel">Test URL</h4>
    </div>
    <form class="test" name="test" action="diagnostics_test_url.php">
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
<!---------------------------------->
    <div class="modal fade" id="TestUserAgent" tabindex="-1" role="dialog" aria-labelledby="TestUrl" aria-hidden="true">
    <div class="modal-dialog modal-lg">
    <div class="modal-content">
    <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
    <h4 class="modal-title" id="myModalLabel">Test User Agent</h4>
    </div>
    <form class="test" name="test" action="diagnostics_test_user_agent.php">
    <div class="modal-body">
    <div class="form-group">
    <label class="label" for="admin" >User Agent:</label>
    <input class="form-control" id="url" name="UserAgent" required>
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
