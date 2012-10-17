import java.net.MalformedURLException; 
import java.rmi.Naming;

public class hotelclient
{
    public static void main(String[] args)
    {
        // Argument parsing
        if(args.length < 2)
            printUsage();

        try
        {
            Hotel c = (Hotel) Naming.lookup("rmi://" + args[0] +
                "/HotelService"); 
            if(args[1].compareTo("list") == 0)
                handleList(c);
            else if(args[1].compareTo("book") == 0)
                handleBook(args, c);
            else if(args[1].compareTo("guests") == 0)
                handleGuests(c);
            else
                printUsage();
        }
        catch (Exception e)
        {
            System.out.println("Received Exception:");
            System.out.println(e); 
        }
    }

    /**
     * Handle the request for a list of free rooms.
     *
     * @param   Hotel   c   The RMI object.
     */
    static private void handleList(Hotel c) {
        int[] rooms = new int[3];
        try {
            rooms = c.list();
        } catch (Exception e) {
            System.out.println("Received Exception:"); 
            System.out.println(e); 
        }
        System.out.println(rooms[0] + "\t" + rooms[1] + "\t" + rooms[2]);
    }

    /**
     * Handle the request for booking a room.
     *
     * @param   String[] args   The data necessary for booking the room.
     */
    static private void handleBook(String[] args, Hotel c) {
        String name = "";
        if(args.length < 4)
            printUsage();

        for(int i = 3; i < args.length; i++) {
            if(i != 3)
                name += " ";
            name += args[3];
        }

        try {
            if(!c.book(Integer.parseInt(args[2]), name))
                System.out.println("Failed to book");
        } catch (Exception e) {
            System.out.println("Received Exception:");
            System.out.println(e); 
        }
    }

    /**
     * Handle the request for a list of guests.
     *
     * @param   Hotel   c   The RMI object.
     */
    static private void handleGuests(Hotel c) {
        try {
            String[] guests = c.guests();
            for(int i = 0; i < guests.length; i++)
                System.out.println(guests[i]);
        } catch (Exception e) {
            System.out.println("Received Exception:");
            System.out.println(e);
        }
    }

    /**
     * Print the usage and exit.
     */
    static private void printUsage() {
        System.out.println("Available options:\n\thotelclient <address> list\n" +
                           "\thotelclient <address> book <type> <Guest name>\n" +
                           "\thotelclient <address> guests\n");
        System.exit(1);
    }
}


