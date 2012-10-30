<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <head>
        <title>Hotel reservation system by Jayke Meijer</title>
        <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
    </head>

    <body>
        <h1>Book a room</h1>
        <p>
        Room type to book:
        <?php
            $roomtype = $_GET["type"];
            echo $roomtype;
        ?>
        <form enctype="multipart/form-data" action="bookroom.php" 
            method=POST>
        Name of participant:
        <input name="name" type="text"><br>
        <input type="hidden" name="roomtype" value="<?php echo $roomtype; ?>">
        <input type="submit" value="Book the room">
        </form>
        </p>
        <a href="../index.html">Go back to the start page</a>
    </body>
</html>
