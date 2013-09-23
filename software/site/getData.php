<?php
$cant = $_GET['cant'];

//database configuration
$config['mysql_host'] = "localhost";
$config['mysql_user'] = "gary";
$config['mysql_pass'] = "garyss";
$config['db_name'] = "beagleway";
$config['table_name'] = "lecturas";

//connect to host
mysql_connect($config['mysql_host'], $config['mysql_user'], $config['mysql_pass']);
//select database
@mysql_select_db($config['db_name']) or die("Unable to select database");

//select last 'cant' items and sort them in the original order
$sql = "SELECT * FROM (SELECT idlectura, timed, acelX, gyrX, angleComplementary, angleKalman, controlPID, controlState FROM " . $config['table_name'] . " ORDER BY idlectura DESC LIMIT $cant) as tbl ORDER BY tbl.idlectura";
        
$result = mysql_query($sql);
if (!$result) {
    die('Invalid query: ' . mysql_error());
}

$acelX = '';
$angleKalman = '';
$controlPID = '';

if (mysql_num_rows($result) > 0) {
    while ($result_array = mysql_fetch_assoc($result)) {
        $acelX .= $result_array['acelX'] . '|';
        $angleKalman .= $result_array['angleKalman'] . '|';
        $controlPID .= $result_array['controlPID'] . '|';
    }
}
$data = substr($acelX, 0, -1) . '#' . substr($angleKalman, 0, -1) . '#' . substr($controlPID, 0, -1);
echo $data;
?>