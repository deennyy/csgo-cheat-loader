<?php
$api_key = $_SERVER['HTTP_X_API_KEY'];

if (strcmp($api_key, '044WrYwb3w4WR8TAGr6') != 0) {
	header('HTTP/1.0 403 Forbidden');
	exit;
}

$status = '';
$username = $_GET['username'];
$curtime = date('Y-m-d H:i:s');
require_once('../connect.php');
$query = $sql->query('SELECT expires FROM users WHERE username=?', [$username]);
$rows = $query->fetch_all(1);
$date = NULL;
foreach($rows as $row) {
    $date = $row['expires'];
}
if ($date > $curtime) {
	$status = 'OK';
} else {
	$status = 'subscription expired';
}
echo $status;
