import java.util.ArrayList;

public class HotelImpl extends java.rmi.server.UnicastRemoteObject implements Hotel
{
    final static int[] capacity = {10, 20, 20};
    ArrayList<String>[] rooms = (ArrayList<String>[])new ArrayList[]
        {new ArrayList<String>(),
         new ArrayList<String>(),
         new ArrayList<String>()};

    // Implementations must have an explicit constructor 
    public HotelImpl() throws java.rmi.RemoteException
    {
        super();
    }

    public int[] list() throws java.rmi.RemoteException
    {
        int[] freeRooms = {capacity[0] - rooms[0].size(),
                           capacity[1] - rooms[1].size(),
                           capacity[2] - rooms[2].size()};
        return freeRooms;
    }

    public boolean book(int type, String guest) throws java.rmi.RemoteException
    {
        if(type < 1 || type > 3)
            return false;

        if(rooms[type-1].size() < capacity[type-1]) {
            if(rooms[type-1].add(guest)) {
                System.out.println("Booked room for " + guest);
                return true;
            }
        }

        return false;
    }

    public String[] guests() throws java.rmi.RemoteException
    {
        int i, j, ctr = 0;
        String[] guestList = new String[rooms[0].size()
                                        + rooms[1].size()
                                        + rooms[2].size()];
        for(i = 0; i < 3; i++) {
            for(j = 0; j < rooms[i].size(); j++) {
                guestList[ctr++] = rooms[i].get(j);
            }
        }
        return guestList;
    }
}
