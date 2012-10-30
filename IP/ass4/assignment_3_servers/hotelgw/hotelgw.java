import java.net.MalformedURLException;
import java.rmi.Naming;
import java.net.*;
import java.io.*;

public class hotelgw
{
    public static void main(String[] args)
    {
        ServerSocket serverSocket;
        try {
            serverSocket = new ServerSocket(1337);
        } catch (IOException e) {
            System.out.println("Socket creation: " + e);
            return;
        }

        while(true) {
            try {
                // Create socket and corresponding datastreams
                Socket connection = serverSocket.accept();
                DataInputStream in =
                    new DataInputStream(connection.getInputStream());
                DataOutputStream out =
                    new DataOutputStream(connection.getOutputStream());

                // Receive the command size
                int size = in.readInt();

                // Receive the command
                byte[] input = new byte[size];
                int nread = in.read(input);
                String inputString = new String(input);

                // Handle the request and send the data over the socket.
                String reply = handleRequest(inputString.substring(0,
                    detectLength(input)), args[0]);
                out.writeBytes(reply);

                // Set up for a new run
                connection.close();
                input = null;
            } catch (IOException e) {
                System.out.println("Main loop: " + e);
                break;
            }
        }
    }

    /**
     * Handle an incoming request.
     *
     * @param   String  req The request as received from the client.
     * @return  String      The answer from the hotel server.
     */
    static private String handleRequest(String req, String server) {
        String[] args = req.split(" ");
        try
        {
            Hotel c = (Hotel) Naming.lookup("rmi://" + server + 
                "/HotelService");
            if(args[0].compareTo("list") == 0)
                return handleList(c);
            else if(args[0].compareTo("book") == 0)
                return handleBook(args, c);
            else if(args[0].compareTo("guests") == 0)
                return handleGuests(c);
        }
        catch (Exception e)
        {
            System.out.println("Handle general request: " + e);
        }
        return "Unknown command";
    }

    /**
     * Handle the request for a list of free rooms.
     *
     * @param   Hotel   c   The RMI object.
     * @return  String      The answer from the hotel server.
     */
    static private String handleList(Hotel c) {
        int[] rooms = new int[3];
        try {
            rooms = c.list();
        } catch (Exception e) {
            System.out.println("Handle list: " + e);
            System.out.println(e);
        }
        return Integer.toString(rooms[0]) + "\t" + Integer.toString(rooms[1])
            + "\t" + Integer.toString(rooms[2]) + "\n";
    }

    /**
     * Handle the request for booking a room.
     *
     * @param   String[] args   The data necessary for booking the room.
     * @return  String          Empty string for compatibility.
     */
    static private String handleBook(String[] args, Hotel c) {
        String name = "";
        if(args.length < 3)
            return "false";

        // Concatenate names longer than 1 word
        for(int i = 2; i < args.length; i++) {
            if(i != 2)
                name += " ";
            name += args[i];
        }

        try {
            if(!c.book(Integer.parseInt(args[1]), name))
                System.out.println("Failed to book");
        } catch (Exception e) {
            System.out.println("Handle book: " + e);
            System.out.println(e);
        }
        return "";
    }

    /**
     * Handle the request for a list of guests.
     *
     * @param   Hotel   c   The RMI object.
     * @return  String      The answer from the hotel server.
     */
    static private String handleGuests(Hotel c) {
        String out = "";
        try {
            String[] guests = c.guests();
            for(int i = 0; i < guests.length; i++) {
                out += guests[i];
                if(i < guests.length - 1)
                    out += "\n";
            }
        } catch (Exception e) {
            System.out.println("Handle guests: " + e);
        }
        return out;
    }

    /**
     * Find the true length of a string, not the length of the buffer, by
     * searching for the first string termination character.
     *
     * @param   byte[]  in  The byte array that will become the string.
     * @return  int         The length of the string.
     */
    static private int detectLength(byte[] in) {
        int i = 0;
        while(in[i] != (byte)'\0')
            i++;
        return i;
    }
}
