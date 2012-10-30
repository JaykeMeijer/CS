<?php
    include("../setup.php");
    include("../socket.php");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <head>
        <title>Hotel reservation system by Jayke Meijer</title>
        <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
    </head>

    <body>
        <h1>Free rooms</h1>
        <p>The following rooms are free:
        <ul>
        <?php
            $socket = create_and_connect($HOTEL_GATEWAY_ADDR,
                $HOTEL_GATEWAY_PORT);

            // Request to gateway
            $out = send_and_receive("list", $socket);

            // Remove trailing whitespace
            $out = trim($out);

            $freerooms = explode("\t", $out);
            $prices = array(150, 120, 100);

            for($i = 0; $i < 3; $i++)
                echo "<li>Room of type " . ($i+1) . ", &#8364;" . $prices[$i] .
                    ". Available number of rooms: " . $freerooms[$i] .
                    ". <a href=\"hotelbook.php?type=" . ($i+1) .
                    "\">Book</a></li>";
        ?>
        </ul>
        </p>
        <a href="../index.html">Go back to the start page</a>
    </body>
</html>
