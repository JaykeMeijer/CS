import java.rmi.Naming;

public class hotelserver {
   public hotelserver(String[] args) {
     try {
       Hotel c = new HotelImpl();
       Naming.rebind("rmi://" + args[0] + "/HotelService", c);
     } catch (Exception e) {
       System.out.println("Trouble: " + e);
     }
   }

   public static void main(String args[]) {
     new hotelserver(args);
   }
}
