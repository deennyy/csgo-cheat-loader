<?php
$api_key = $_SERVER['HTTP_X_API_KEY'];

if (strcmp($api_key, 'REDACTED') != 0) {
	header('HTTP/1.0 403 Forbidden');
	exit;
}

$status = '';
$username = $_GET['username'];
if (strlen($username) == 0 || strlen($username) > 20) {
    $status = 'username must be between 1-20 characters';
} else {
    $password = $_GET['password'];
    if (strlen($password) == 0 || strlen($password) > 32) {
        $status = 'password must be between 1-32 characters';
    } else {
        require_once('../connect.php');
        $query = $sql->query('SELECT * FROM users WHERE username=?', [$username]);
        if (mysqli_num_rows($query) == 0) {
            $status = 'incorrect username/password';
        } else {
            if (password_verify($password, $query->fetch_assoc()['password'])) {
		    	$query = $sql->query('SELECT uid FROM users WHERE username=?', [$username]);
		    	$uid = NULL;
		    	$rows = $query->fetch_all(1);
		    	foreach ($rows as $row) {
					$uid = $row['uid'];
		    	}
		    	$query = $sql->query('SELECT * FROM tokens WHERE uid=?', [$uid]);
		    	if (mysqli_num_rows($query) != 0) {
					$sql->query('DELETE FROM tokens WHERE uid=?', [$uid]);
		    	}
		    	$token = rand();
		    	$token_hash = md5($token);
		    	$expires = date('Y-m-d H:i:s', time() + 120);
		    	$sql->query('INSERT INTO tokens (uid, token, expires) VALUES (?,?,?)', [$uid, $token_hash, $expires]);
		    	$query = $sql->query('SELECT * FROM tokens WHERE uid=?', [$uid]);
		    	if (mysqli_num_rows($query) != 1) {
					$status = 'failed generating token';
		    	} else {
					$status = 'OK' . $token_hash;
		    	}
            } else {
                $status = 'incorrect username/password';
            }
        }
    }
}
echo $status;
