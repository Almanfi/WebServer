<?php
// write basic php file to do anything amazing


//header('status: 300 asdasd sdf sdf dsf');

// read input and print it
// echo file_get_contents('php://input');

header('Content-Type: application/json');
// echo get parameters
echo json_encode($_GET);
