<?php
$api_key = $_SERVER['HTTP_X_API_KEY'];

if (strcmp($api_key, 'REDACTED') != 0) {
	header('HTTP/1.0 403 Forbidden');
	exit;
}

$status = '';

require_once('../connect.php');

$token = $_GET['token'];

$query = $sql->query('SELECT * FROM tokens WHERE token=?', [$token]);

if (mysqli_num_rows($query) == 0) {
	$status = 'invalid token';
	echo $status;
	exit;
}

$expires = NULL;

$rows = $query->fetch_all(1);

foreach ($rows as $row) {
	$expires = $row['expires'];
}

$curtime = date('Y-m-d H:i:s');

if ($curtime > $expires) {
	$status = 'invalid token';
	echo $status;
	exit;
} else {
	$sql->query('DELETE FROM tokens WHERE token=?', [$token]);
	header('Content-Disposition: attachment; filename="test.dll"');
	header('Content-Type: application/octet-stream');
	readfile('../dll/test.dll');
}
