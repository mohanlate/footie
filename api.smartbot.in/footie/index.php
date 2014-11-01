<?php

$in = $_GET['TotalIN'];
$out = $_GET['TotalOUT'];

echo "Total incoming: " . $in;
echo "<br />";
echo "Total outgoing: " . $out;
echo "<br />";

$data = file_get_contents ('./datalog.json');
$json = json_decode($data, true);
 
//echo "Date:" . $json["data2"]["TotalIN"];

$today = date("m.d.y"); 
$time = date("H:i:s"); 
 
$json[date("Y")][date("m")][date("d")][ date("H")][ date("i")]["TotalIN"] = $in;
$json[date("Y")][date("m")][date("d")][ date("H")][ date("i")]["TotalOUT"] = $out;



$fh = fopen('./datalog.json', 'w')
      or die("Error opening output file");
fwrite($fh, json_encode($json,JSON_UNESCAPED_UNICODE));
fclose($fh); 
 
?>