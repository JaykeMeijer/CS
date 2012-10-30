<?php
    function create_and_connect($address, $port) {
        // Create socket
        if (($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP))
                == false) {
            echo "socket_create() failed: reason: " .
                socket_strerror(socket_last_error()) . "\n";
        }

        // Connect socket to gateway
        if (socket_connect($socket, $address, $port) == false) {
            echo "socket_connect() failed.\nReason: ($result) " . 
                socket_strerror(socket_last_error($socket)) . "\n";
        }

        return $socket;
    }

    function send_and_receive($request, $socket) {
        // Send command size
        socket_write($socket, pack("N", strlen($request) + 1), 4);

        // Send command
        socket_write($socket, $request, strlen($request));
        $out = "";
        while($recv = socket_read($socket, 2048)) {
            $out = $out . $recv;
        }
        socket_close($socket);
        return $out;
    }
?>
