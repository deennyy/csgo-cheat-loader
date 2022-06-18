<?php
$status = '';
if (isset($_POST['register'])) {
    $username = $_POST['username'];
    if (strlen($username) == 0 || strlen($username) > 20) {
        $status = 'username must be between 1-20 characters';
    } else {
        $password = $_POST['password'];
        if (strlen($password) == 0 || strlen($password) > 32) {
            $status = 'password must be between 1-32 characters';
        } else {
            $c_password = $_POST['c_password'];
            if (strcmp($password, $c_password) != 0) {
                $status = 'passwords do not match';
            } else {
                require_once('connect.php');
                $query = $sql->query('SELECT * FROM users WHERE username=?', [$username]);
                if (mysqli_num_rows($query) == 0) {
                    $invite = $_POST['invite'];
                    $query = $sql->query('SELECT * FROM invites WHERE invite=?', [$invite]);
                    if (mysqli_num_rows($query) == 1) {
                        $password_hash = password_hash($password, PASSWORD_DEFAULT);
                        $sql->query('INSERT INTO users (username, password) VALUES (?,?)', [$username, $password_hash]);
                        $query = $sql->query('SELECT * FROM users WHERE username=?', [$username]);
                        if (mysqli_num_rows($query) == 1) {
                            $sql->query('DELETE FROM invites WHERE invite=?', [$invite]);
                            header('Location: index.php');
                            exit;
                        } else {
                            $status = 'registration failed';
                        }
                    } else {
                        $status = 'invalid invite code';
                    }
                } else {
                    $status = 'username is already taken';
                }
            }
        }
    }
}
?>

<html>
    <body>
        <form method="POST">
            username: <input name="username" type="text"/> <br/>
            password: <input name="password" type="password"/> <br/>
            confirm password: <input name="c_password" type="password"/> <br/>
            invite: <input name="invite" type="text"/> <br/>
            <input type="submit" value="register" name="register"/> <br/>
            <?php echo $status;?>
        </form>
    </body>
</html>
