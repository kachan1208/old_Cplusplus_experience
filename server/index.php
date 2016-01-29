<?php
require 'flight/Flight.php';

Flight::route('/login', function()
{
	$result = array('success' => "0", 'message' => "OK");
	echo json_encode($result);
});

Flight::route('/version?(app=@name:[a-zA-Z]+)(&build=@version:[0-9]+)', function()
{
	$app = $_GET['app'];
	$version = $_GET['build'];
	$result = array('build' => $app, 'updates' => $version);
	echo json_encode($result);
});

Flight::start();
?>
