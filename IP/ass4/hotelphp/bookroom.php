<?php
    include("../setup.php");
    include("../socket.php");

    $roomtype = $_POST["roomtype"];
    $name = $_POST["name"];

    $socket = create_and_connect($HOTEL_GATEWAY_ADDR, $HOTEL_GATEWAY_PORT);
    // Request to gateway
    $out = send_and_receive("book " . $roomtype . " " . $name, $socket);
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <head>
        <title>Hotel reservation system by Jayke Meijer</title>
        <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
    </head>

    <body>
        <h1>Room is booked!</h1>
        <p><a href="../index.html">Go back to the start page</a></p>
    </body>
</html>
