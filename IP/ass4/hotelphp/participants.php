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
        <h1>Participants</h1>
        <p>The following persons are participating:
        <?php

            $socket = create_and_connect($HOTEL_GATEWAY_ADDR,
                $HOTEL_GATEWAY_PORT);

            // Request to gateway
            $out = send_and_receive("guests", $socket);

            if(strlen($out) <= 0) {
                echo "<br><i>No participants yet</i>";
                return;
            }

            echo "<ul>\n";

            $guests = explode("\n", $out);

            for($i = 0; $i < count($guests); $i++)
                echo "<li>". $guests[$i] . "</li>";

            echo "</ul>\n"
        ?>
        </p>
        <a href="../index.html">Go back to the start page</a>
    </body>
</html>
