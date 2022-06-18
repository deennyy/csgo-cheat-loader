<?php
$api_key = $_SERVER['HTTP_X_API_KEY'];

if (strcmp($api_key, 'REDACTED') != 0) {
	header('HTTP/1.0 403 Forbidden');
	exit;
}

$status = '';
$username = $_GET['username'];
$get_hwid = $_GET['hwid'];
require_once('../connect.php');
$query = $sql->query('SELECT hwid FROM users WHERE username=?', [$username]);
$rows = $query->fetch_all(1);
$hwid = NULL;
foreach ($rows as $row) {
    $hwid = $row['hwid'];
}
if ($hwid == NULL) {
	$sql->query('UPDATE users SET hwid=? WHERE username=?', [$get_hwid, $username]);
	$status = 'OK';
} else {
	if (strcmp($hwid, $get_hwid) == 0) {
		$status = 'OK';
	} else {
		$status = 'invalid hwid';
	}
}
echo $status;
